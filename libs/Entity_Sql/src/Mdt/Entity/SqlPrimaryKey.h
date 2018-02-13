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
#ifndef MDT_ENTITY_SQL_PRIMARY_KEY_H
#define MDT_ENTITY_SQL_PRIMARY_KEY_H

// #include "SqlField.h"
#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyType.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include "MdtEntity_SqlExport.h"
#include <QMetaType>

namespace Mdt{ namespace Entity{

  /*! \brief Helper to get a SQL primary key from a entity
   */
  class MDT_ENTITY_SQL_EXPORT SqlPrimaryKey
  {
   public:

//     /*! \brief Get a SQL primary key container from \a entityPrimaryKey
//      */
//     template<typename EntityDataStruct, typename EntityDef>
//     static Mdt::Sql::Schema::PrimaryKeyContainer fromEntityPrimaryKey(const PrimaryKey & entityPrimaryKey)
//     {
//     }

    /*! \brief Get SQL primary key type from \a entityPrimaryKey
     */
    static Mdt::Sql::Schema::PrimaryKeyType primaryKeyType(const PrimaryKey & entityPrimaryKey)
    {
      if(entityPrimaryKey.fieldCount() < 1){
        return Mdt::Sql::Schema::PrimaryKeyType::Unknown;
      }
      if(entityPrimaryKey.fieldCount() != 1){
        return Mdt::Sql::Schema::PrimaryKeyType::PrimaryKey;
      }
      switch(entityPrimaryKey.fieldAt(0).fieldType()){
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
          return Mdt::Sql::Schema::PrimaryKeyType::AutoIncrementPrimaryKey;
        default:
          break;
      }
      return Mdt::Sql::Schema::PrimaryKeyType::Unknown;
    }

    /*! \brief Get a auto increment primary key from \a entityPrimaryKey
     *
     * \pre \a entityPrimaryKey must contain exactly 1 field
     */
    template<typename EntityDef>
    static Mdt::Sql::Schema::AutoIncrementPrimaryKey autoIncrementPrimaryKey(const PrimaryKey & entityPrimaryKey, const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap)
    {
      Q_ASSERT(entityPrimaryKey.fieldCount() == 1);
      const auto sqlFieldType = fieldTypeMap.fieldTypeFromQMetaType( entityPrimaryKey.fieldAt(0).fieldType(), Mdt::Sql::Schema::FieldLength() );
      return Mdt::Sql::Schema::AutoIncrementPrimaryKey( entityPrimaryKey.fieldAt(0).fieldName<EntityDef>(), sqlFieldType );
    }

    /*! \brief Get a primary key from \a entityPrimaryKey
     */
    template<typename EntityDef>
    static Mdt::Sql::Schema::PrimaryKey primaryKey(const PrimaryKey & entityPrimaryKey)
    {
      Q_ASSERT(entityPrimaryKey.fieldCount() > 0);
      Mdt::Sql::Schema::PrimaryKey pk;
      pk.setFieldNameList( entityPrimaryKey.toFieldNameList<EntityDef>() );
      return pk;
    }

    /*! \brief Get a SQL primary key container from a entity
     */
    template<typename EntityDataStruct, typename EntityDef>
    static Mdt::Sql::Schema::PrimaryKeyContainer fromEntity(const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap)
    {
      Mdt::Sql::Schema::PrimaryKeyContainer pkc;
      const auto entityPk = PrimaryKey::fromEntity<EntityDataStruct, EntityDef>();
      const auto pkType = primaryKeyType(entityPk);
      switch(pkType){
        case Mdt::Sql::Schema::PrimaryKeyType::AutoIncrementPrimaryKey:
          pkc.setPrimaryKey( autoIncrementPrimaryKey<EntityDef>(entityPk, fieldTypeMap) );
          break;
        case Mdt::Sql::Schema::PrimaryKeyType::PrimaryKey:
          pkc.setPrimaryKey( primaryKey<EntityDef>(entityPk) );
          break;
        case Mdt::Sql::Schema::PrimaryKeyType::Unknown:
          break;
      }

      return pkc;
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_PRIMARY_KEY_H
