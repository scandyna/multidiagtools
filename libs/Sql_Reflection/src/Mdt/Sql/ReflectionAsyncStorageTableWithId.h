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
#ifndef MDT_SQL_REFLECTION_ASYNC_STORAGE_TABLE_WITH_ID_H
#define MDT_SQL_REFLECTION_ASYNC_STORAGE_TABLE_WITH_ID_H

#include "Mdt/Sql/Reflection/InsertStatement.h"
#include "Mdt/Sql/Reflection/UpdateStatement.h"
#include "Mdt/Sql/Reflection/DeleteStatement.h"
#include "Mdt/Sql/AsyncQueryConnection.h"
#include "Mdt/Sql/AsyncInsertQuery.h"
#include "Mdt/Sql/AsyncSelectQuery.h"
#include "Mdt/Sql/UpdateQueryAffectedRowsFailureMode.h"
#include "Mdt/Sql/AsyncUpdateQuery.h"
#include "Mdt/Sql/AsyncDeleteQuery.h"
// #include "Mdt/Sql/AsyncQuery.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Container/StlContainer.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include "Mdt/Expected.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/StructRecordAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsIdPrimaryKeyClass.h"
#include "Mdt/Reflection/TypeTraits/IsStructDefAssociatedWithReflectedStruct.h"
#include "Mdt/QueryExpression/ReflectionSelectStatement.h"
#include "Mdt/QueryExpression/ReflectionPrimaryKeyRecordAlgorithm.h"
#include "MdtSql_ReflectionExport.h"
#include <QVariant>
#include <QObject>
#include <QCoreApplication>
#include <memory>
#include <vector>
#include <type_traits>

#include <QDebug>

namespace Mdt{ namespace Sql{

  /*! \brief Base class to create a async storage table
   */
  class ReflectionAsyncStorageTableBase : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    ReflectionAsyncStorageTableBase(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr)
     : QObject(parent),
       mConnection(connection)
    {
      Q_ASSERT(mConnection.get() != nullptr);
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   Q_SIGNALS:

    /*! \brief Emitted by some getter methods when a new record was fetched from the database table
     */
    void newRecordFetched(const Mdt::Container::VariantRecord & record);

    /*! \brief Emitted when a record have been added to the database table
     */
    void newRecordAdded(const QVariant & id);

    /*! \brief Emitted when a record update in the database table have been done
     */
    void updateRecordDone();
//     void updateRecordDone(const QVariant & id);

    /*! \brief Emitted when one ore more records removal in the database table have been done
     */
    void removeRecordDone();

    /*! \brief Emitted when a error occured
     */
    void errorOccurred(const Mdt::Error & error) const;

//     template<typename Struct>
//     void newRecordAvailable(const Struct & s);

   protected:

    /*! \brief Get the query connection this table refers to
     */
    std::shared_ptr<AsyncQueryConnection> connection() const
    {
      return mConnection;
    }

   protected Q_SLOTS:

    /*! \brief Set last insert error
     *
     * \pre \a error must not be null
     */
    void setLastInsertError(const Mdt::Error & queryError)
    {
      Q_ASSERT(!error.isNull());

      const QString msg = tr("");

      auto error = Mdt::Error::newErrorFromOther(queryError);
      error.updateText(msg);

      setLastError(error);
    }

   private:

    /*! \brief Set last error
     *
     * \pre \a error must not be null
     */
    void setLastError(const Mdt::Error & error) const
    {
      Q_ASSERT(!error.isNull());

      qDebug() << "AST setLastError() ";// << error.text();
      mLastError = error;
      emit errorOccurred(error);
    }

    std::shared_ptr<AsyncQueryConnection> mConnection;
    mutable Mdt::Error mLastError;
  };

  /*! \brief Helper class to persist a reflected data struct to a database table in a asynchronous way
   *
   * When calling some async getter method, the newRecordFetched() signal will be emitted each time
   *  a record was fetched from the database.
   *  To get a domain struct representation of the record, use the static structFromRecord() method
   *  (Emitting a signal with a struct is not directly possible, due to limitations of MOC with templates).
   *
   * This storage table also provides some synchronous methods. The primary goal to provide them
   *  is to avoid having 2 accesses to a database connection from concurent threads.
   *  As adventage, the synchronous methods will not block the callers Qt event loop.
   *  But, the synchronous calls will introduce some latency, compared to blocking calls (order of 100us).
   * \sa Mdt::Async::wait()
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   * \pre \a PrimaryKey must be a id primary key class
   * \pre \a Id must be a type supported by QVariant
   *
   * \sa ReflectionStorageTableWithId
   * \sa \ref example_PersonRepository
   */
  template<typename StructDef, typename PrimaryKey, typename Id>
  class ReflectionAsyncStorageTableWithId : public ReflectionAsyncStorageTableBase
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsIdPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a id primary key class" );

    Q_DECLARE_TR_FUNCTIONS(ReflectionAsyncStorageTableWithId)

   public:

    /*! \brief Reflected struct associated with \a StructDef
     */
    using Struct = typename StructDef::reflected_struct;

    /*! \brief Construct a storage table
     */
    ReflectionAsyncStorageTableWithId(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr)
     : ReflectionAsyncStorageTableBase(connection, parent),
       mInsertQuery(connection),
       mSelectQuery(connection),
       mUpdateQuery(connection),
       mDeleteQuery(connection)
    {
      Q_ASSERT(connection.get() != nullptr);

      mUpdateQuery.setAffectedRowsFailureMode(UpdateQueryAffectedRowsFailureMode::FailIfNotExaclyOneRowAffected);

      connect( &mInsertQuery, &AsyncInsertQuery::newIdInserted, this, &ReflectionAsyncStorageTableWithId::newRecordAdded );
      connect( &mInsertQuery, &AsyncInsertQuery::errorOccurred, this, &ReflectionAsyncStorageTableWithId::setLastError );
      setupQuery(mSelectQuery);
      connect( &mUpdateQuery, &AsyncUpdateQuery::doneSuccessfully, this, &ReflectionAsyncStorageTableWithId::updateRecordDone );
      connect( &mUpdateQuery, &AsyncUpdateQuery::errorOccurred, this, &ReflectionAsyncStorageTableWithId::setLastError );
      connect( &mDeleteQuery, &AsyncDeleteQuery::doneSuccessfully, this, &ReflectionAsyncStorageTableWithId::removeRecordDone );
      connect( &mDeleteQuery, &AsyncDeleteQuery::errorOccurred, this, &ReflectionAsyncStorageTableWithId::setLastError );
    }

    /*! \brief Add a record to the database table asynchronously
     *
     * If the database table's primary key is a auto increment id,
     *  and the value that refers to this primary key in \a record is null
     *  (f.ex. 0), it will not be submitted to the database
     *  (This way, the DBMS will auto-generate a new id).
     *
     * Once the record have been added to the database table,
     *  newRecordAdded() will be emitted,
     *  otherwise errorOccurred() is emitted.
     */
    void addAsync(const Struct & record)
    {
      const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PrimaryKey>(record);

      mInsertQuery.submitStatement(statement);
    }

    /*! \brief Add a record to the database table synchronously
     *
     * If the database table's primary key is a auto increment id,
     *  and the value that refers to this primary key in \a record is null
     *  (f.ex. 0), it will not be submitted to the database
     *  (This way, the DBMS will auto-generate a new id).
     */
    Mdt::Expected<Id> add(const Struct & record)
    {
      const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<PrimaryKey>(record);
      AsyncInsertQuery query(connection());

      if(!query.execStatement(statement)){
        Mdt::Error error;
        const QString msg = tr("Could not insert a record into table '%1'").arg(tableName());
        const auto queryError = query.lastError();
        /// \todo Should have a way to get query error in a simpler way..
        if(queryError.isErrorType<Mdt::ErrorCode>()){
          error = mdtErrorNewT(queryError.error<Mdt::ErrorCode>(), msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        }else{
          error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        }
        error.stackError(query.lastError());
//         setLastError(error);
        return error;
      }

      return query.lastInsertId().value<Id>();
    }

    /*! \brief Get a record from the database table asynchronously
     *
     * If no error occures, and the requested record exists in the database table,
     *  newRecordFetched() will be emitted.
     *
     * If no error occured, but the requested record does not exist in the database table,
     *  errorOccurred() will be emitted with a error containing Mdt::ErrorCode::NotFound .
     *
     * For any other cases, errorOccurred() will be emitted.
     */
    void getAsync(Id id)
    {
      mSelectQuery.submitGetSingleRecordStatement( getSelectStatement(id) );
    }

    /*! \brief Get a record from the database table synchronously
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
    Mdt::Expected<Struct> get(Id id) const
    {
      Struct s;
      AsyncSelectQuery query(connection());
      setupQuery(query);

      if(!query.execStatement( getSelectStatement(id) )){
        const QString msg = tr("Could not get a record from table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return error;
      }

      qDebug() << "ST: fetchSingleRecord() ...";
      const auto record = query.fetchSingleRecord();
      qDebug() << "ST: fetchSingleRecord() Finished";
      if(record.isEmpty()){
        if(query.lastError().isError(Mdt::ErrorCode::NotFound)){
          const QString msg = tr("Could not get a record with id '%1' from table '%2'")
                              .arg(QString::number(id), tableName());
          auto error = mdtErrorNewT(Mdt::ErrorCode::NotFound, msg, Mdt::Error::Warning, "ReflectionAsyncStorageTableWithId");
//           setLastError(error);
          return error;
        }
        const QString msg = tr("Could not get a record from table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return error;
      }
      Q_ASSERT(record.columnCount() == structFieldCount());

      return structFromRecord(record);
    }

    /*! \brief Get all records from the database table asynchronously
     *
     * Will emit newRecordFetched() each time a record was fetched from the database table.
     *
     * On error, errorOccurred() is emitted.
     */
    void getAllAsync()
    {
      mSelectQuery.submitStatementAndFetchAll( getAllSelectStatement() );
    }

    /*! \brief Get all records from the database table synchronously
     *
     * \pre \a Container must be STL compatible
     * \pre \a Container must have \a Struct as value_type
     */
    template<typename Container = std::vector<Struct> >
    Mdt::Expected<Container> getAll() const
    {
      static_assert( std::is_same<typename Container::value_type, Struct>::value ,
                     "Container::value_type must be Struct" );

      Container sList;
      AsyncSelectQuery query(connection());

      if(!query.execStatement( getAllSelectStatement() )){
        const QString msg = tr("Could not get records from table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return error;
      }

      while(query.fetchRecords()){
        const int n = query.fetchedRecordCount();
        for(int i = 0; i < n; ++i){
          Mdt::Container::appendToContainer( sList, structFromRecord(query.record(i)) );
        }
      }

      return sList;
    }

    /*! \brief Update a record in the database table asynchronously
     *
     * Will emit updateRecordDone() on success,
     *  otherwise errorOccurred().
     */
    void updateAsync(const Struct & record)
    {
      const auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<PrimaryKey>(record);

      mUpdateQuery.submitStatement(statement);
    }

    /*! \brief Update a record in the database table synchronously
     */
    bool update(const Struct & record)
    {
      const auto statement = Mdt::Sql::Reflection::updateStatementFromReflected<PrimaryKey>(record);
      AsyncUpdateQuery query(connection());

      query.setAffectedRowsFailureMode(UpdateQueryAffectedRowsFailureMode::FailIfNotExaclyOneRowAffected);
      if(!query.execStatement(statement)){
        const QString msg = tr("Could not update a record in table '%1'").arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return false;
      }

      return true;
    }

    /*! \brief Remove a record from the database table asynchronously
     */
    void removeAsync(Id id)
    {
      const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<PrimaryKey>(id);

      mDeleteQuery.submitStatement(statement);
    }

    /*! \brief Remove a record from the database table synchronously
     */
    bool remove(Id id)
    {
      const auto statement = Mdt::Sql::Reflection::deleteStatementFromReflectedById<PrimaryKey>(id);
      AsyncDeleteQuery query(connection());

      if(!query.execStatement(statement)){
        const QString msg = tr("Could not remove the record with id '%1' from table '%2'")
                            .arg(QString::number(id), tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return false;
      }

      return true;
    }

    /*! \brief Remove all record from the database table asynchronously
     */
    void removeAllAsync()
    {
      const auto statement = Mdt::Sql::Reflection::deleteAllStatementFromReflected<StructDef>();

      mDeleteQuery.submitStatement(statement);
    }

    /*! \brief Remove all record from the database table synchronously
     */
    bool removeAll()
    {
      const auto statement = Mdt::Sql::Reflection::deleteAllStatementFromReflected<StructDef>();
      AsyncDeleteQuery query(connection());

      if(!query.execStatement(statement)){
        const QString msg = tr("Could not remove all records from table '%1'")
                            .arg(tableName());
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "ReflectionAsyncStorageTableWithId");
        error.stackError(query.lastError());
//         setLastError(error);
        return false;
      }

      return true;
    }

    /*! \brief Get a instance of Struct from a record
     *
     * \pre the count of columns in \a record must match the count of fields in \a Strcut
     */
    static Struct structFromRecord(const Mdt::Container::VariantRecord & record)
    {
      return Mdt::Reflection::structFromVariantRecord<Struct>(record);
    }

   private:

    static Mdt::QueryExpression::ReflectionSelectStatement<StructDef> getSelectStatement(Id id)
    {
      Mdt::QueryExpression::ReflectionSelectStatement<StructDef> statement;
      statement.setFilterExpression( Mdt::QueryExpression::filterExpressionFromIdPrimaryKeyValue<PrimaryKey>(id) );
      statement.addAllFields();

      return statement;
    }

    static Mdt::QueryExpression::ReflectionSelectStatement<StructDef> getAllSelectStatement()
    {
      Mdt::QueryExpression::ReflectionSelectStatement<StructDef> statement;
      statement.addAllFields();

      return statement;
    }

    static constexpr int structFieldCount() noexcept
    {
      return Mdt::Reflection::reflectedFieldCountFromStructDef<StructDef>();
    }

    static const QString tableName()
    {
      return Mdt::Reflection::nameFromStructDefQString<StructDef>();
    }

    void setupQuery(AsyncSelectQuery & query) const
    {
      connect( &query, &AsyncSelectQuery::newRecordAvailable, this, &ReflectionAsyncStorageTableWithId::newRecordFetched );
      connect( &query, &AsyncSelectQuery::errorOccurred, this, &ReflectionAsyncStorageTableWithId::setLastError, Qt::DirectConnection );
    }

    AsyncInsertQuery mInsertQuery;
    AsyncSelectQuery mSelectQuery;
    AsyncUpdateQuery mUpdateQuery;
    AsyncDeleteQuery mDeleteQuery;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_REFLECTION_ASYNC_STORAGE_TABLE_WITH_ID_H
