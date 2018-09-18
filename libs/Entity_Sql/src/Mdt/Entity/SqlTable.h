/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_SQL_TABLE_H
#define MDT_ENTITY_SQL_TABLE_H

#include "SqlField.h"
#include "SqlPrimaryKey.h"
#include "SqlForeignKey.h"
#include "Mdt/Entity/FieldAttributes.h"
#include "Mdt/Entity/UniqueConstraintAlgorithm.h"
#include "Mdt/Entity/TypeTraits/IsEntity.h"
#include "Mdt/Entity/TypeTraits/IsEntityDef.h"
#include "Mdt/Entity/TypeTraits/IsEntityFieldDef.h"
#include "Mdt/Entity/TypeTraits/IsRelation.h"
#include "Mdt/Entity/TypeTraits/IsUniqueConstraint.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/Index.h"
#include "MdtEntity_SqlExport.h"
#include <QVariant>
#include <boost/fusion/include/for_each.hpp>
#include <type_traits>

namespace Mdt{ namespace Entity{

  namespace Impl{

    /*! \internal Functor called from SqlTable::fromEntity() to add fields
     */
    template<typename EntityDataStruct>
    class AddEntityFieldsToSqlTable
    {
     public:

      AddEntityFieldsToSqlTable(Mdt::Sql::Schema::Table & table, const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap)
       : mTable(table),
         mFieldTypeMap(fieldTypeMap)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef &) const
      {
        static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value, "EntityFieldDef must be a entity field definition type" );

        const auto sqlField = SqlField::fromEntityField<EntityDataStruct, EntityFieldDef>(mFieldTypeMap);
        Q_ASSERT(!sqlField.isNull());
        mTable.addField(sqlField);
      }

     private:

      Mdt::Sql::Schema::Table & mTable;
      const Mdt::Sql::Schema::FieldTypeMap & mFieldTypeMap;
    };

  } // namespace Impl{

  /*! \brief Helper to reflect a entity to a SQL table
   */
  class MDT_ENTITY_SQL_EXPORT SqlTable
  {
   public:

    /*! \brief Create a SQL table from a entity
     */
    template<typename Entity>
    static Mdt::Sql::Schema::Table fromEntity(const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make())
    {
      static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

      Mdt::Sql::Schema::Table table;
      using data_struct_type = typename Entity::data_struct_type;
      constexpr typename Entity::def_type entityDef;

      table.setTableName(entityDef.entityName());
      Impl::AddEntityFieldsToSqlTable<data_struct_type> op(table, fieldTypeMap);
      boost::fusion::for_each(entityDef, op);
      table.setPrimaryKeyContainer( SqlPrimaryKey::fromEntity<Entity>(fieldTypeMap) );

      return table;
    }

    /*! \brief Add a foreign key from a entity relation to \a table
     */
    template<typename EntityRelation>
    static void addForeignKeyFromRelationToTable(Mdt::Sql::Schema::Table & table, const Mdt::Sql::Schema::ForeignKeySettings & foreignKeySettings)
    {
      static_assert(TypeTraits::IsRelation<EntityRelation>::value, "EntityRelation must be a entity relation type");

      table.addForeignKey( Mdt::Entity::SqlForeignKey::fromEntityRelation<EntityRelation>(foreignKeySettings) );
    }

    /*! \brief Add a unique constraint to \a table
     *
     * \pre \a UniqueConstraint must be based on UniqueConstraint
     */
    template<typename UniqueConstraint>
    static void addUniqueConstraintToTable(Mdt::Sql::Schema::Table & table)
    {
      static_assert( TypeTraits::IsUniqueConstraint<UniqueConstraint>::value, "UniqueConstraint must be based on UniqueConstraint" );

      Mdt::Sql::Schema::Index index;

      index.setUnique(true);
      index.setTableName( table.tableName() );
      index.setFieldNameList( getUniqueConstraintFieldNameList<UniqueConstraint>() );
      index.generateName();
      table.addIndex(index);
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_TABLE_H
