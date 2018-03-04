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
#ifndef MDT_ENTITY_SQL_FOREIGN_KEY_H
#define MDT_ENTITY_SQL_FOREIGN_KEY_H

#include "Mdt/Entity/Relation.h"
#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Entity/FieldAt.h"
#include "Mdt/Entity/TypeTraits/IsEntity.h"
#include "Mdt/Entity/TypeTraits/IsEntityDef.h"
#include "Mdt/Entity/TypeTraits/IsEntityFieldDef.h"
#include "Mdt/Entity/TypeTraits/IsRelation.h"
#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/ForeignField.h"
#include "MdtEntity_SqlExport.h"
#include <initializer_list>

namespace Mdt{ namespace Entity{

  namespace Impl{ namespace SqlForeignKey{

    void addFieldPairsToFk(Mdt::Sql::Schema::ForeignKey & sqlFk, const Mdt::Entity::PrimaryKey & pk);

    template<typename PrimaryEntityDef, typename ForeignEntityDef, typename ForeignEntityField, typename ...ForeignEntityFields>
    void addFieldPairsToFk(Mdt::Sql::Schema::ForeignKey & sqlFk, const Mdt::Entity::PrimaryKey & pk)
    {
      static_assert( TypeTraits::IsEntityDef<PrimaryEntityDef>::value, "PrimaryEntityDef must be a entity definition type" );
      static_assert( TypeTraits::IsEntityDef<ForeignEntityDef>::value, "ForeignEntityDef must be a entity definition type" );
      static_assert( TypeTraits::IsEntityFieldDef<ForeignEntityField>::value, "ForeignEntityField must be a entity field definition type" );

      const auto pkIndex = sqlFk.fieldPairCount();
      Q_ASSERT(pkIndex < pk.fieldCount());

      sqlFk.addFieldNames( ForeignEntityField::fieldName(), Mdt::Sql::Schema::ForeignField( pk.fieldAt(pkIndex).fieldName<PrimaryEntityDef>() ) );
    }

    template<typename PrimaryEntity, typename ForeignEntity, typename ...ForeignEntityFields>
    void setupSqlForeignKey(const Mdt::Entity::Relation<PrimaryEntity, ForeignEntity, ForeignEntityFields...> &, Mdt::Sql::Schema::ForeignKey & sqlFk)
    {
      static_assert( TypeTraits::IsEntity<PrimaryEntity>::value, "PrimaryEntity must be a entity type" );
      static_assert( TypeTraits::IsEntity<ForeignEntity>::value, "ForeignEntity must be a entity type" );

      using primary_entity_def_type = typename PrimaryEntity::def_type;
      using foreign_entity_def_type = typename ForeignEntity::def_type;

      sqlFk.setParentTableName( primary_entity_def_type::entityName() );
      sqlFk.setChildTableName( foreign_entity_def_type::entityName() );

      const auto pk = Mdt::Entity::PrimaryKey::fromEntity<PrimaryEntity>();
      Q_ASSERT(sizeof...(ForeignEntityFields) == pk.fieldCount());

      (void)std::initializer_list<int>{ (addFieldPairsToFk<primary_entity_def_type, foreign_entity_def_type, ForeignEntityFields>(sqlFk, pk) ,0)... };
    }

  }} // namespace Impl{ namespace SqlForeignKey{

  /*! \brief Helper to get a SQL foreign key from a entity
   *
   * | %Entity world  | SQL world                         |
   * |:---------------|:----------------------------------|
   * | Primary entity | Parent table, or referenced table |
   * | Foreign entity | Child table                       |
   */
  class MDT_ENTITY_SQL_EXPORT SqlForeignKey
  {
   public:

    /*! \brief Get a SQL foreign key from a entity relation
     *
     *  Returns a SQL foreign key for the foreign entity in the relation.
     *   This is the case because foreign keys are declared in the child table.
     */
    template<typename EntityRelation>
    static Mdt::Sql::Schema::ForeignKey fromEntityRelation(const Mdt::Sql::Schema::ForeignKeySettings & sqlFkSettings)
    {
      static_assert(TypeTraits::IsRelation<EntityRelation>::value, "EntityRelation must be a entity relation type");

      Mdt::Sql::Schema::ForeignKey sqlFk;

      Impl::SqlForeignKey::setupSqlForeignKey(EntityRelation{}, sqlFk);
      sqlFk.setSettings(sqlFkSettings);

      return sqlFk;
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_FOREIGN_KEY_H
