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
#ifndef MDT_SQL_ASYNC_QUERY_CONNECTION_IMPL_H
#define MDT_SQL_ASYNC_QUERY_CONNECTION_IMPL_H

#include "AbstractAsyncQueryThreadWorker.h"
#include "InsertStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
// #include "Mdt/Async/WaitDonePredicate.h"
#include "Mdt/Async/WaitDonePredicateWithError.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QThread>
#include <QVariant>
#include <chrono>

namespace Mdt{ namespace Sql{

  /*! \internal Implementation class for AsyncQueryConnection
   */
  class AsyncQueryConnectionImpl : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AsyncQueryConnectionImpl(QObject *parent = nullptr);

    /*! \brief Close the database handle and remove the connection
     */
    ~AsyncQueryConnectionImpl();

    /*! \brief Get the next instance id
     */
    int nextInstanceId() noexcept
    {
      ++mQueryInstanceId;

      return mQueryInstanceId;
    }

    template<typename Worker, typename OpenFunc>
    void setThreadWorker(Worker *worker, OpenFunc openFunc)
    {
      Q_ASSERT(worker != nullptr);
      Q_ASSERT(!mThread.isRunning());

      worker->moveToThread(&mThread);
      connect(&mThread, &QThread::finished, worker, &QObject::deleteLater);
      connect(&mThread, &QThread::started, worker, openFunc);
      connect(worker, &Worker::openDone, this, &AsyncQueryConnectionImpl::setOpenDone);
      connect(worker, &Worker::openErrorOccured, this, &AsyncQueryConnectionImpl::setOpenError);
      connect(worker, &Worker::queryDone, this, &AsyncQueryConnectionImpl::queryDone);
      connect(worker, &Worker::queryErrorOccured, this, &AsyncQueryConnectionImpl::queryErrorOccured);
      connect(this, &AsyncQueryConnectionImpl::insertStatementSubmitted, worker, &AbstractAsyncQueryThreadWorker::processInsertStatement);
      connect(worker, &AbstractAsyncQueryThreadWorker::newIdInserted, this, &AsyncQueryConnectionImpl::newIdInserted);

      mThread.start();
    }

    /*! \brief Wait until the database is open for this connection
     */
    Mdt::ExpectedResult waitOpen();

    /*! \brief Submit a insert statement
     */
    void submitInsertStatement(const InsertStatement & statement, int instanceId);

    /*! \brief Begin close the database handle
     */
    void submitClose();

    /*! \brief Close the database handle
     */
    void close();

   public Q_SLOTS:

    void setOpenDone();

    void setOpenError(const Mdt::Error & error);

   Q_SIGNALS:

    /*! \brief Forwards the statement comming from the query to the thread worker
     */
    void insertStatementSubmitted(const Mdt::Sql::InsertStatement & statement, int instanceId);

    /*! \brief Emitted when a new ID have been inserted
     */
    void newIdInserted(const QVariant & id, int instanceId);

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record, int instanceId);

    /*! \brief Emitted whenever a query is done
     */
    void queryDone(int instanceId);

    /*! \brief Emitted whenever a query error occured
     */
    void queryErrorOccured(const Mdt::Error & error, int instanceId);

    /*! \brief Emitted when the database handle have been closed
     */
    void closed();

   private:

    int mQueryInstanceId = 0;
    QThread mThread;
    Mdt::Async::WaitDonePredicateWithError mWaitOpenPredicate;
    Mdt::Error mLastOpenError;
//     Mdt::Async::WaitDonePredicate mWaitClosedPredicate;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_CONNECTION_IMPL_H
