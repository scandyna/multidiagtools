/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SQL_ASYNC_QUERY_H
#define MDT_SQL_ASYNC_QUERY_H

#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Execute SQL queries asynchronously
   *
   * AsyncQuery is like a subset of QSqlQuery, but works asynchronously.
   *
   * As explained in the Qt documentation, a connection should only be used by the thread that created it.
   *  When using AsyncQuery API, it is possible (and probably recomanded) to access the same database
   *  from one thread. Because of this, creating a thread for each AsyncQuery instance is not a option.
   *  This is the reason that using AsyncQuery is a bit less simple than using QSqlQuery.
   *
   * Before using a AsyncQuery, a async connection must be created:
   * \code
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * AsyncQueryConnection connection;
   * if(!connection.setup(parameters.toConnectionParameters())){
   *   // error handling, use setupError() to get informations about the error
   * }
   * \endcode
   *
   * \sa AsyncQueryConnection
   *
   * To get a instance of a async query, the connection must be used:
   * \code
   * auto query = connection.createQuery();
   * \endcode
   *
   * At some place in the application, event comming from the query
   *  will be handled:
   * \code
   * connect( query.get(), &AsyncQuery::newRecordAvailable, myViewModel, &MyViewModel::appendNewRecord );
   * connect( query.get(), &AsyncQuery::errorOccured, myViewModel, &MyViewModel::handleError );
   * \endcode
   *
   * Finaly, a query can be submited:
   * \code
   * query.submitQuery("SELECT * FROM Person");
   * \endcode
   *
   * \note To execute queries in a blocking way, QSqlQuery should be used
   */
  class MDT_SQL_CORE_EXPORT AsyncQuery : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AsyncQuery(int instanceId, QObject *parent = nullptr);

    AsyncQuery(const AsyncQuery &) = delete;
    AsyncQuery & operator=(const AsyncQuery &) = delete;
    AsyncQuery(AsyncQuery &&) = delete;
    AsyncQuery & operator=(AsyncQuery &&) = delete;

    /*! \brief Submit a query
     */
    void submitQuery(const QString & query);

    /*! \internal Get instance ID
     */
    int instanceId() const noexcept
    {
      return mInstanceId;
    }

   public slots:

     /*! \internal Set a new record available
      */
     void setNewRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

     /*! \internal Set error
      */
     void setErrorOccured(const Mdt::Error & error, int instanceId);

   signals:

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record);

    /*! \brief Emitted whenever a error occured
     */
    void errorOccured(const Mdt::Error & error);

    /*! \internal
     */
    void queryRequested(const QString & query, int instanceId);

   private:

    int mInstanceId;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_H
