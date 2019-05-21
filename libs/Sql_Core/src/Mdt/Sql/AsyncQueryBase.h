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
#ifndef MDT_SQL_ASYNC_QUERY_BASE_H
#define MDT_SQL_ASYNC_QUERY_BASE_H

#include "AsyncQueryConnection.h"
#include "AsyncQueryOperationType.h"
#include "Mdt/Error.h"
#include "Mdt/Async/WaitDonePredicateWithError.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <memory>

namespace Mdt{ namespace Sql{

  /*! \brief Common base for some async queries
   */
  class MDT_SQL_CORE_EXPORT AsyncQueryBase : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a connection must be a valid pointer
     */
    explicit AsyncQueryBase(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr);

    /*! \brief Destructor
     */
    ~AsyncQueryBase();

    AsyncQueryBase(const AsyncQueryBase &) = delete;
    AsyncQueryBase & operator=(const AsyncQueryBase &) = delete;
    AsyncQueryBase(AsyncQueryBase &&) = delete;
    AsyncQueryBase & operator=(AsyncQueryBase &&) = delete;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   Q_SIGNALS:

    /*! \brief Emitted once the query was done successfully
     */
    void doneSuccessfully();

    /*! \brief Emitted when an error occurred
     */
    void errorOccurred(const Mdt::Error & error);

   protected:

    /*! \brief Get a pointer to the connection implementation
     *
     * The returned pointer should only be used to make signal/slot connections.
     */
    AsyncQueryConnectionImpl *connectionImpl() const
    {
      return mConnection->impl();
    }

    /*! \brief Get instance ID
     */
    int instanceId() const noexcept
    {
      return mInstanceId;
    }

    /*! \brief Wait until a operation finished
     *
     * This method can be used to implement synchronous methods.
     *
     * Returns true if all goes well, otherwise false.
     *
     * If the query sends a error, lastError() will return it.
     *  On timeout, lastError() will return a error of type Mdt::ErrorCode::TimeoutError.
     *
     * \todo On timeout, the query should be cancelled, this is not solved...
     */
    bool waitOperationFinished();

   private Q_SLOTS:

    /*! \brief Set done
     */
    void setOperationDone(AsyncQueryOperationType operationType, int instanceId);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error, int instanceId);

   private:

    int mInstanceId;
    std::shared_ptr<AsyncQueryConnection> mConnection;
    Mdt::Async::WaitDonePredicateWithError mWaitPredicate;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_QUERY_BASE_H
