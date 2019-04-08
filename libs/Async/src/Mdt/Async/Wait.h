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
#ifndef MDT_ASYNC_WAIT_H
#define MDT_ASYNC_WAIT_H

#include "WaitDonePredicate.h"
#include "MdtAsyncExport.h"
#include <QObject>
#include <chrono>

namespace Mdt{ namespace Async{

  /*! \brief Wait until a operation is done without breaking the Qt event loop of the caller
   *
   * \todo Qt event loop
   *
   * There are some reasons to use such a wait function.
   *
   * While solving some problems works well with signals and slots, other onces are better solved in a blocking way,
   *  but without blocking the UI, for example.
   *
   * \todo script style + reactive UI use case
   *
   * A other use case is using Qt SQL asynchronously.
   *  A implementation of such API is available with Mdt::Sql::AsyncQuery and its related Mdt::Sql::AsyncQueryConnection .
   *  As specified in the documentation, the async connection will handle a database connection, which runs in a separate thread.
   *  One solution could be to have a instance of Mdt::Sql::AsyncQueryConnection for asynchronous tasks
   *  and a QSqlDatabase connection, instanciated from the main thread, for the blocking onces.
   *  In this case, we have 2 concurrent threads working on the same database instance.
   *  While Qt supports this on its API, it is also dependent of the database client implementation.
   *  See [Threads and the SQL Module](https://doc.qt.io/qt-5/threads-modules.html#threads-and-the-sql-module) for more informations about that.
   *  One solution is to only use Mdt::Sql::AsyncQuery and Mdt::Async::wait() to provide blocking behaviour.
   *  For a example, see Mdt::Sql::ReflectionAsyncStorageTableWithId .
   *
   * \todo Document duration usage
   *
   * Following code is a example of usage:
   * \code
   * using namespace std::chrono_literals;
   * using Mdt::Async::WaitDonePredicate;
   *
   * MyAsyncTask task;
   * MyWindow window;
   * WaitDonePredicate pred;
   *
   * QObject::connect(&task, &MyAsyncTask::newResultAvailable, &window, &MyWindow::displayNewResult);
   * QObject::connect(&task, &MyAsyncTask::errorOccured, &window, &MyWindow::handleError);
   *
   * QObject::connect(&task, &MyAsyncTask::operationSucceeded, &pred, &WaitDonePredicate::setDone);
   * QObject::connect(&task, &MyAsyncTask::errorOccured, &pred, &WaitDonePredicate::setDone);
   *
   * window.show();
   *
   * task.begin();
   * if( !Mdt::Async::wait(pred, 5s) ){
   *   // Operation timed out
   *   const auto error = ...
   *   window.handleError(error);
   * }
   * \endcode
   */
  bool MDT_ASYNC_EXPORT wait(WaitDonePredicate & pred, std::chrono::milliseconds timeout);

}} // namespace Mdt{ namespace Async{

#endif // MDT_ASYNC_WAIT_H
