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
#ifndef MDT_SQL_ASYNC_QUERY_CONNECTION_H
#define MDT_SQL_ASYNC_QUERY_CONNECTION_H

#include "AsyncQuery.h"
#include "ConnectionParameters.h"
#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QThread>
#include <QString>
#include <QVariant>
#include <memory>

namespace Mdt{ namespace Sql{

  class AsyncQueryThreadWorker;

  /*! \brief Connection to a database for async queries
   *
   * The main goal is to have one thread for one database connection.
   *  The database connection will be established in the thread
   *  that this connection creates.
   *
   * Internally, a connection name is generated for the QSqlDatabase instance
   *  that this connection will handle.
   *  That means that each instance of AsyncQueryConnection will
   *  create a new thread and also add a new QSqlDatabase connection.
   *
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
   * AsyncQueryConnection will not check if a connection refers to the same database or not:
   * \code
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * AsyncQueryConnection connection1;
   * AsyncQueryConnection connection2;
   *
   * connection1.setup(parameters.toConnectionParameters());
   * connection2.setup(parameters.toConnectionParameters());
   * \endcode
   * Here, connection1 and connection2 will reference the same SQLite database file.
   *  This file will also be accessed by 2 concurrent threads.
   *  The caller should check if this safe or not.
   *
   * It should be a good idea to only create 1 AsyncQueryConnection for a single database.
   */
  class MDT_SQL_CORE_EXPORT AsyncQueryConnection : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Create a async query connection
     *
     * \sa setup()
     */
    explicit AsyncQueryConnection(QObject *parent = nullptr);

    /*! \brief Stop the query thread and free resources
     */
    ~AsyncQueryConnection();

    /*! \brief Setup this connection
     *
     * \note If setup was previously done,
     *   the thread will be stopped, without waiting that running queries are all processed.
     */
    bool setup(const ConnectionParameters & parameters);

    /*! \brief Setup this connection
     *
     * This overload permit to pass a custom worker.
     *  This connection will take ownership of \a worker and delete it at apropriate time.
     * \note \a worker will be moved to a separate thread, so it should not be accessed anymore.
     */
    bool setup(const ConnectionParameters & parameters, AsyncQueryThreadWorker *worker);

    /*! \brief Get setup error
     */
    Mdt::Error setupError() const
    {
      return mSetupError;
    }

    /*! \brief Create a query
     */
    std::unique_ptr<AsyncQuery> createQuery();

  signals:

    /*! \internal Forwards the request comming from AsyncQuery to the thread worker
     */
    void queryRequested(const QVariant & query, int instanceId);

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

    /*! \brief Emitted whenever a error occured
     */
    void errorOccured(const Mdt::Error & error, int instanceId);

   private:

    int mQueryInstanceId = 0;
    QThread mThread;
    Mdt::Error mSetupError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_CONNECTION_H
