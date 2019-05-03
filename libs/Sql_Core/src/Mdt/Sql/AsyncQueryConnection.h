/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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

#include "AsyncQueryConnectionImpl.h"
#include "ConnectionParameters.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <memory>
#include <type_traits>

namespace Mdt{ namespace Sql{

  class AbstractAsyncQueryThreadWorker;

  /*! \brief %Connection to a database for async queries
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
   * auto connection = std::make_shared<SQLiteAsyncQueryConnection>();
   * const auto result = connection->open(parameters);
   * if(!result){
   *   handleError(result.error());
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
   * auto connection1 = std::make_shared<SQLiteAsyncQueryConnection>();
   * auto connection2 = std::make_shared<SQLiteAsyncQueryConnection>();
   *
   * connection1->open(parameters);
   * connection2->open(parameters);
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

   friend class AsyncQueryBase;

   public:

    /*! \brief Create a async query connection
     */
    explicit AsyncQueryConnection(QObject *parent = nullptr);

    AsyncQueryConnection(const AsyncQueryConnection &) = delete;
    AsyncQueryConnection & operator=(const AsyncQueryConnection &) = delete;
    AsyncQueryConnection(AsyncQueryConnection &&) = delete;
    AsyncQueryConnection & operator=(AsyncQueryConnection &&) = delete;

    /*! \brief Tell the query thread to close the database handle
     *
     * The query thread will close the database handle
     *  using QSqlDatabase::close().
     *
     * The database connection will then be removed
     *  using QSqlDatabase::removeDatabase().
     *
     * Finally, the query thread will be stopped
     *  using QThread::quit().
     *
     * Once done, the closed() signal is emitted.
     *
     * \sa close()
     */
    void submitClose();

    /*! \brief Close the database handle
     *
     * \sa submitClose()
     */
    void close();

  Q_SIGNALS:

    /*! \brief Emitted when the database handle have been closed
     */
    void closed();

   protected:

    /*! \brief Set a thread worker for this connection
     *
     * This method will first move \a worker to a separate thread,
     *  do the necessary signal/slot connections,
     *  then start the thread.
     *
     * Once the thread started, \a openFunc is called.
     *
     * \code
     * void SQLiteAsyncQueryConnection::submitOpenExisting(const SQLiteConnectionParameters & parameters)
     * {
     *   close();
     *
     *   auto *worker = new SQLiteAsyncQueryThreadWorker(parameters);
     *
     *   setThreadWorker(worker, &SQLiteAsyncQueryThreadWorker::processOpenExisting);
     *   // Starting from here, do not call any non thread safe method on worker.
     *   // The lifetime from worker is also now handled by the separate thread
     * }
     * \endcode
     *
     * \pre This connection must be closed before
     */
    template<typename Worker, typename OpenFunc>
    void setThreadWorker(Worker *worker, OpenFunc openFunc)
    {
      static_assert( std::is_base_of<AbstractAsyncQueryThreadWorker, Worker>::value, "Worker must be a subclass of AbstractAsyncQueryThreadWorker" );
      Q_ASSERT(worker != nullptr);

      mImpl.setThreadWorker(worker, openFunc);
    }

    /*! \brief Wait until the database is open for this connection
     */
    Mdt::ExpectedResult waitOpen()
    {
      return mImpl.waitOpen();
    }

   private:

    /*! \brief Get a pointer to the connection implementation
     *
     * The returned pointer should only be used to make signal/slot connections.
     */
    AsyncQueryConnectionImpl *impl()
    {
      return &mImpl;
    }

    /*! \brief Get a pointer to the connection implementation
     *
     * The returned pointer should only be used to make signal/slot connections.
     */
    const AsyncQueryConnectionImpl *constImpl() const
    {
      return &mImpl;
    }

    AsyncQueryConnectionImpl mImpl;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_CONNECTION_H
