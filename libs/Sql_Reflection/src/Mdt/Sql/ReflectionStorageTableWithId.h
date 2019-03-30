/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_REFLECTION_STORAGE_TABLE_WITH_ID_H
#define MDT_SQL_REFLECTION_STORAGE_TABLE_WITH_ID_H

// #include "Mdt/Sql/DeleteStatement.h"
// // #include "PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Sql/Reflection/InsertStatement.h"
#include "Mdt/Sql/Reflection/UpdateStatement.h"
#include "Mdt/Sql/Reflection/DeleteStatement.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Mdt/Sql/UpdateQuery.h"
#include "Mdt/Sql/DeleteQuery.h"
#include "Mdt/QueryExpression/ReflectionSelectStatement.h"
#include "Mdt/QueryExpression/ReflectionPrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsIdPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
// #include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
// #include "Mdt/Sql/DeleteStatement.h"
// #include "Mdt/Sql/PrimaryKeyRecord.h"
// #include "Mdt/Sql/FieldName.h"
#include "Mdt/Expected.h"
#include "Mdt/ErrorCode.h"
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QSqlRecord>
// #include <QLatin1String>
// #include <type_traits>

namespace Mdt{ namespace Sql{

  namespace Impl{

    /*! \brief Internal
     */
    class SetQSqlRecordValuesToStruct
    {
     public:

      SetQSqlRecordValuesToStruct(const QSqlRecord & record)
       : mRecord(record)
      {
      }

      template<typename T>
      void operator()(T & valueRef) const
      {
        Q_ASSERT(mIndex < mRecord.count());
        valueRef = mRecord.value(mIndex).value<T>();
        ++mIndex;
      }

     private:

      mutable int mIndex = 0;
      const QSqlRecord & mRecord;
    };

  } // namespace Impl{

  /*! \brief Helper class to persist a reflected data struct to a database table
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   * \pre \a PrimaryKey must be a id primary key class
   * \pre \a Id must be a type supported by QVariant
   */
  template<typename StructDef, typename PrimaryKey, typename Id>
  class ReflectionStorageTableWithId
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsIdPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a id primary key class" );

    Q_DECLARE_TR_FUNCTIONS(ReflectionStorageTableWithId)

   public:

    /*! \brief Construct a storage table
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    ReflectionStorageTableWithId(const QSqlDatabase & db)
     : mDatabase(db)
    {
      Q_ASSERT(db.isValid());
    }

    /*! \brief Add a record to the database table
     *
     * If the database table's primary key is a auto increment id,
     *  and the value that refers to this primary key in \a record is null
     *  (f.ex. 0), it will not be submitted to the database
     *  (This way, the DBMS will auto-generate a new id).
     *
     * \pre \a record must be a instance of a reflected struct associated with \a StructDef
     */
    template<typename Struct>
    Mdt::Expected<Id> add(const Struct & record)
    {
      static_assert( Mdt::Reflection::TypeTraits::IsStructDefAssociatedWithReflectedStruct<StructDef, Struct>::value,
                     "record must be a instance of a reflected struct associated with \a StructDef" );

      const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PrimaryKey>(record);
      Mdt::Sql::InsertQuery query(mDatabase);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not insert a record into table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      return query.lastInsertId().value<Id>();
    }

    /*! \brief Get a record from the database table
     *
     * Returns the requested struct if no error occured,
     *  and it exists in the database table.
     *
     * If no error occured, but the requested record does not exists
     *  in the database table, error containing Mdt::ErrorCode::NotFound is returned.
     *
     * For any other cases, a error is returned.
     *
     * Example of usage:
     * \code
     * const auto person = table.get(26);
     * if(!person){
     *   if(person->error()->isError(Mdt::ErrorCode::NotFound)){
     *     // No error, but no person with id 26 found
     *   }else{
     *     // Some other error occured
     *   }
     * }
     * usePerson(*person);
     * \endcode
     */
    Mdt::Expected<typename StructDef::reflected_struct> get(Id id) const
    {
      typename StructDef::reflected_struct s;

      Mdt::QueryExpression::ReflectionSelectStatement<StructDef> statement;
      statement.setFilterExpression( Mdt::QueryExpression::filterExpressionFromIdPrimaryKeyValue<PrimaryKey>(id) );
      statement.addAllFields();

      Mdt::Sql::SelectQuery query(mDatabase);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not get a record from table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      const QSqlRecord record = query.fetchSingleRecord();
      if(record.isEmpty()){
        if(query.lastError().isError(Mdt::ErrorCode::NotFound)){
          const QString msg = tr("Could not get a record with id '%1' from table '%2'")
                              .arg(QString::number(id), tableName());
          auto error = mdtErrorNewT(Mdt::ErrorCode::NotFound, msg, Mdt::Error::Warning, "ReflectionStorageTableWithId");
          return error;
        }
        const QString msg = tr("Could not get a record from table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      Q_ASSERT(record.count() == Mdt::Reflection::reflectedFieldCountFromStructDef<StructDef>());
      Impl::SetQSqlRecordValuesToStruct f(record);
      Mdt::Reflection::forEachValueInStruct(s, f);

      return s;
    }

    /*! \brief Update a record in the database table
     *
     * \pre \a record must be a instance of a reflected struct associated with \a StructDef
     */
    template<typename Struct>
    Mdt::ExpectedResult update(const Struct & record)
    {
      static_assert( Mdt::Reflection::TypeTraits::IsStructDefAssociatedWithReflectedStruct<StructDef, Struct>::value,
                     "record must be a instance of a reflected struct associated with \a StructDef" );

      const auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<PrimaryKey>(record);
      Mdt::Sql::UpdateQuery query(mDatabase);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not update a record in table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      return Mdt::ExpectedResultOk();
    }

    /*! \brief Remove a record from the database table
     */
    Mdt::ExpectedResult remove(Id id)
    {
      const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<PrimaryKey>(id);
      Mdt::Sql::DeleteQuery query(mDatabase);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not remove the record with id '%1' from table '%2'")
                            .arg(QString::number(id), tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      return Mdt::ExpectedResultOk();
    }

    /*! \brief Remove all record from the database table
     */
    Mdt::ExpectedResult removeAll()
    {
      const auto statement = Mdt::Sql::Reflection::deleteAllStatementFromReflected<StructDef>();
      Mdt::Sql::DeleteQuery query(mDatabase);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not remove all records from table '%1'")
                            .arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionStorageTableWithId");
        error.stackError(query.lastError());
        return error;
      }

      return Mdt::ExpectedResultOk();
    }

   private:

    static const QString tableName()
    {
      return Mdt::Reflection::nameFromStructDefQString<StructDef>();
    }

    QSqlDatabase mDatabase;
  };

}} // namespace Mdt{ namespace Sql{


#endif // #ifndef MDT_SQL_REFLECTION_STORAGE_TABLE_WITH_ID_H
