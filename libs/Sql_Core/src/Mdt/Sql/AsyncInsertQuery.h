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
#ifndef MDT_SQL_ASYNC_INSERT_QUERY_H
#define MDT_SQL_ASYNC_INSERT_QUERY_H

#include "AsyncQueryBase.h"
#include "AsyncQueryConnection.h"
#include "InsertStatement.h"
#include "MdtSql_CoreExport.h"
#include <QVariant>

namespace Mdt{ namespace Sql{

  /*! \brief Execute a SQL insert query asynchronously
   */
  class MDT_SQL_CORE_EXPORT AsyncInsertQuery : public AsyncQueryBase
  {
    Q_OBJECT

    using BaseClass = AsyncQueryBase;

   public:

    /*! \brief Constructor
     *
     * \pre \a connection must be a valid pointer
     */
    explicit AsyncInsertQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr);

    /*! \brief Execute a insert statement synchronously
     */
    bool execStatement(const InsertStatement & statement);

    /*! \brief Check if this query executes in synchronous mode
     *
     * A query is synchronous after a successfull call to execStatement().
     */
    bool isSynchronous() const noexcept
    {
      return mIsSynchronous;
    }

    /*! \brief Get last insert id
     *
     * Returns the most recent insert ID after a successfull call of execStatement()
     *  if the database supports it, otherwise a null QVariant.
     *  Internally, QSqlQuery::lastInsertId() is used.
     *
     * \pre This query must be in synchronous mode
     * \sa isSynchronous()
     */
    QVariant lastInsertId() const
    {
      Q_ASSERT(isSynchronous());

      return mLastInertId;
    }

   public Q_SLOTS:

    /*! \brief Submit a insert statement to be executed asynchronously
     *
     * On success, the newIdInserted() signal is emitted with the most recent ID
     *  if the database supports it, otherwise with a null QVariant.
     *  Internally, QSqlQuery::lastInsertId() is used.
     *
     * On error, the errorOccurred() signal is emitted.
     */
    void submitStatement(const InsertStatement & statement);

   Q_SIGNALS:

    /*! \brief Emitted when a new ID have been inserted
     *
     * \sa submitStatement()
     */
    void newIdInserted(const QVariant & id);

   private Q_SLOTS:

    /*! \brief Set new inserted ID
     */
    void setNewInsertedId(const QVariant & id, int iid);

   private:

    bool mIsSynchronous = false;
    QVariant mLastInertId;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_INSERT_QUERY_H
