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
#ifndef MDT_SQL_ASYNC_SELECT_QUERY_H
#define MDT_SQL_ASYNC_SELECT_QUERY_H

#include "AsyncQueryBase.h"
#include "AsyncQueryConnection.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "MdtSql_CoreExport.h"
#include <QVariant>

namespace Mdt{ namespace Sql{

  /*! \brief Execute a select query asynchronously
   *
   * Example for a asynchronous usage:
   * \code
   * MyTableModel model;
   * QTableView view;
   * AsyncSelectQuery query(connection);
   *
   * connect(&query, &AsyncSelectQuery::newRecordAvailable, &model, &MyTableModel::appendRecord);
   * view.setModel(&model);
   *
   * query.submitStatement(statement);
   * \endcode
   *
   * Example for a synchronous usage:
   * \code
   * AsyncSelectQuery query(connection);
   *
   * const auto result = query.execStatement(statement);
   * if(!result){
   *   // Error handling
   * }
   *
   * while(query.next()){
   *   const QString name = query.value(1).toString();
   * }
   * \endcode
   */
  class MDT_SQL_CORE_EXPORT AsyncSelectQuery : public AsyncQueryBase
  {
    Q_OBJECT

    using BaseClass = AsyncQueryBase;

   public:

    /*! \brief Constructor
     *
     * \pre \a connection must be a valid pointer
     */
    explicit AsyncSelectQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr);

    /*! \brief Execute a select statement synchronously
     */
    bool execStatement(const Mdt::QueryExpression::SelectStatement & statement);

    /*! \brief Check if this query executes in synchronous mode
     *
     * A query is synchronous after a successfull call to execStatement().
     */
    bool isSynchronous() const noexcept
    {
      return mIsSynchronous;
    }

    /*! \brief Get the count of fields the result of this query contains
     *
     * If not query have been executed, 0 is returned
     *
     * \pre This query must be in synchronous mode
     * \sa isSynchronous()
     */
    int fieldCount() const
    {
      Q_ASSERT(isSynchronous());

      return mCurrentRecord.columnCount();
    }

    /*! \brief Retrieves the next record in the result, if available, and positions the query on the retrieved record
     *
     * The behaviour of this method is similar to QSqlQuery::next()
     *
     * \pre This query must be in synchronous mode
     * \sa isSynchronous()
     */
    bool next();

    /*! \brief Returns the value of field index in the current record
     *
     * The behaviour of this method is similar to QSqlQuery::value()
     *
     * \pre This query must be in synchronous mode
     * \sa isSynchronous()
     */
    QVariant value(int fieldIndex) const
    {
      Q_ASSERT(isSynchronous());

      return mCurrentRecord.value(fieldIndex);
    }

   public Q_SLOTS:

    /*! \brief Submit a select statement to be executed asynchronously
     *
     * Once the query executed the statement, the records will be fetched.
     *
     * \sa newSqlRecordAvailable()
     * \sa newRecordAvailable()
     */
    void submitStatement(const Mdt::QueryExpression::SelectStatement & statement);

   Q_SIGNALS:

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record);

   private Q_SLOTS:

    void onNewRecordAvailable(const Mdt::Container::VariantRecord & record, int iid);
    void onFetchNexDone(bool result, int iid);

   private:

    void submitStatement(const Mdt::QueryExpression::SelectStatement & statement, bool fetchRecords);
    void submitFetchNext();

    bool mIsSynchronous = false;
    bool mFetchNextResult = false;
    Mdt::Container::VariantRecord mCurrentRecord;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_SELECT_QUERY_H
