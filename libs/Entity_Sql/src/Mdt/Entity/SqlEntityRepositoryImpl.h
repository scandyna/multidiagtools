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
#ifndef MDT_ENTITY_SQL_ENTITY_REPOSITORY_IMPL_H
#define MDT_ENTITY_SQL_ENTITY_REPOSITORY_IMPL_H

#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Entity/PrimaryKeyRecord.h"
#include "Mdt/Entity/PrimaryKeyRecordFilterExpression.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Entity/FieldAt.h"
#include "Mdt/Entity/ValueAt.h"
#include "Mdt/Entity/Value.h"
#include "Mdt/Entity/FieldAttributes.h"
#include "Mdt/Entity/TypeTraits/IsEntity.h"
#include "Mdt/Entity/TypeTraits/IsEntityDef.h"
#include "Mdt/Entity/TypeTraits/IsEntityFieldDef.h"
#include "Mdt/Entity/TypeTraits/FieldDef.h"
#include "Mdt/Entity/SqlPrimaryKeyRecord.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/UpdateQuery.h"
#include "Mdt/Expected.h"
#include "Mdt/Error.h"
#include "MdtEntity_SqlExport.h"
#include <QString>
#include <QObject>
#include <QSqlDatabase>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <type_traits>

// #include <QDebug>

namespace Mdt{ namespace Entity{

  namespace Impl{

    template<typename Entity>
    class AddEntityFieldsToSelectStatement
    {
     public:

      AddEntityFieldsToSelectStatement(EntitySelectStatement<Entity> & stm)
       : mStatement(stm)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef & fieldDef) const
      {
        static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value, "EntityFieldDef must be a entity field definition type" );

        mStatement.addField(fieldDef);
      }

     private:

      EntitySelectStatement<Entity> & mStatement;
    };

    template<typename EntityDataStruct, typename UniqueId>
    class AddEntityFieldsToInsertQuery
    {
     public:

      AddEntityFieldsToInsertQuery(Mdt::Sql::InsertQuery & query, const EntityDataStruct & dataStruct)
       : mQuery(query),
         mDataStruct(dataStruct)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef &) const
      {
        static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value, "EntityFieldDef must be a entity field definition type" );

        using Mdt::Sql::FieldName;

        if(mustAddField<EntityFieldDef>()){
          mQuery.addValue( FieldName(EntityFieldDef::fieldName()), value<EntityDataStruct, EntityFieldDef>(mDataStruct) );
        }
      }

     private:

      template<typename EntityFieldDef>
      bool mustAddField() const noexcept
      {
        if(isNullUniqueId<EntityDataStruct, EntityFieldDef, UniqueId>(mDataStruct)){
          return false;
        }
        return true;
      }

      Mdt::Sql::InsertQuery & mQuery;
      const EntityDataStruct & mDataStruct;
    };

    template<typename EntityDataStruct>
    class AddEntityFieldsToUpdateQuery
    {
     public:

      AddEntityFieldsToUpdateQuery(Mdt::Sql::UpdateQuery & query, const EntityDataStruct & dataStruct)
       : mQuery(query),
         mDataStruct(dataStruct)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef & fieldDef) const
      {
        static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value, "EntityFieldDef must be a entity field definition type" );

        using Mdt::Sql::FieldName;

        if(mustAddField(fieldDef)){
          mQuery.addValue( FieldName(fieldDef.fieldName()), value<EntityDataStruct, EntityFieldDef>(mDataStruct) );
        }
      }

     private:

      template<typename EntityFieldDef>
      bool mustAddField(const EntityFieldDef & fieldDef) const noexcept
      {
        if(fieldDef.fieldAttributes().isPrimaryKey()){
          return false;
        }
        return true;
      }

      Mdt::Sql::UpdateQuery & mQuery;
      const EntityDataStruct & mDataStruct;
    };

  } // namespace Impl{

  class MDT_ENTITY_SQL_EXPORT SqlEntityRepositoryImpl : public QObject
  {
   Q_OBJECT

   public:

    void setDatabase(const QSqlDatabase & db)
    {
      Q_ASSERT(db.isValid());

      mDatabase = db;
    }

    /*! \brief Get the database connection
     */
    QSqlDatabase database() const
    {
      return mDatabase;
    }

    template<typename EntityData, typename UniqueId>
    Mdt::Expected<UniqueId> add(const EntityData & record)
    {
      using EntityDef = typename EntityData::entity_def_type;
      using EntityDataStruct = typename EntityData::data_struct_type;
      using IdType = typename UniqueId::value_type;

      Mdt::Expected<UniqueId> newId;
      constexpr EntityDef entityDef;
      Mdt::Sql::InsertQuery query(mDatabase);

      query.setTableName(record.def().entityName());
      Impl::AddEntityFieldsToInsertQuery<EntityDataStruct, UniqueId> op(query, record.constDataStruct());
      boost::fusion::for_each(entityDef, op);

      if(!query.exec()){
        newId = query.lastError();
        return newId;
      }
      newId = UniqueId( query.lastInsertId().value<IdType>() );

      return newId;
    }

    template<typename EntityData, typename UniqueId>
    Mdt::Expected<EntityData> getById(UniqueId id) const
    {
      using EntityTemplate = typename EntityData::entity_template_type;

      const auto pk = PrimaryKey::fromEntity<EntityTemplate>();
      Q_ASSERT(pk.fieldCount() == 1);
      PrimaryKeyRecord pkRecord;
      pkRecord.addField( PrimaryKeyRecordField(pk.fieldAt(0).fieldIndex(), id.value()) );

      return getByPrimaryKey<EntityData>(pkRecord);
    }

    template<typename EntityData>
    Mdt::Expected<EntityData> getByPrimaryKey(const PrimaryKeyRecord & pk) const
    {
      using EntityTemplate = typename EntityData::entity_template_type;
      using EntityDef = typename EntityData::entity_def_type;
      using DataStruct = typename EntityData::data_struct_type;

      Mdt::Expected<EntityData> record;
      constexpr EntityDef entityDef;

      EntitySelectStatement<EntityTemplate> stm;
      Impl::AddEntityFieldsToSelectStatement<EntityTemplate> op(stm);
      boost::fusion::for_each(entityDef, op);
      const auto filter = filterExpressionFromPrimaryKeyRecord<EntityTemplate>(pk);
      stm.setFilterExpression(filter);

      Mdt::QueryExpression::SqlSelectQuery query;
      query.setDatabase(mDatabase);
      if(!query.exec(stm)){
        record = query.lastError();
        return record;
      }
      if(!query.next()){
        record = EntityData{};
        return record;
      }
      constexpr int fc = fieldCount<EntityDef>();
      if(query.fieldCount() != fc){
        const auto msg = tr("Fetching data for '%1' returned a wrong count of fields.")
                         .arg(EntityDef::entityName());
        const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
        record = error;
        return record;
      }
      DataStruct ds;
      for(int i = 0; i < fc; ++i){
        setValueAt(ds, i, query.value(i));
      }
      Q_ASSERT(!query.next());
      EntityData data;
      data.setDataStruct(ds);
      record = data;

      return record;
    }

    template<typename EntityData>
    Mdt::Error update(const EntityData & record)
    {
      using EntityDef = typename EntityData::entity_def_type;
      using EntityDataStruct = typename EntityData::data_struct_type;

      Mdt::Error error;
      const auto pkRecord = SqlPrimaryKeyRecord::fromEntityData(record);
      Mdt::Sql::UpdateQuery query(mDatabase);
      constexpr EntityDef entityDef;

      query.setTableName(EntityDef::entityName());

      Impl::AddEntityFieldsToUpdateQuery<EntityDataStruct> op(query, record.constDataStruct());
      boost::fusion::for_each(entityDef, op);
      query.setConditions(pkRecord);

      if(!query.exec()){
        error = query.lastError();
        return error;
      }

      return error;
    }

   private:

    QSqlDatabase mDatabase;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_ENTITY_REPOSITORY_IMPL_H
