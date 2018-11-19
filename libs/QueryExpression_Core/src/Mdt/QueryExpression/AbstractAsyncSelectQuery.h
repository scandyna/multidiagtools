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
#ifndef MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_H

#include "SelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Base class to implement a backend specific asynchronous select query
   *
   * To display a list of persons without blocking the UI, we could use Mdt::ItemModel::AbstractReadOnlyCachedTableModel:
   * \code
   * class ListPersonTableModel : public Mdt::ItemModel::AbstractReadOnlyCachedTableModel
   * {
   *  Q_OBJECT
   *
   *   using SelectStatement = Mdt::QueryExpression::SelectStatement;
   *
   *  public:
   *
   *   ListPersonTableModel(QObject *parent = nullptr);
   *
   *  signals:
   *
   *   void fetchRecordsRequested(const SelectStatement & statement, int maxRows);
   *
   *  private:
   *
   *   bool fetchRecords(int count) override;
   *
   *   SelectStatement mStatement;
   * };
   * \endcode
   *
   * The concrete backend will be instantiated and setup in one place,
   *  for example in main, using a factory:
   * \code
   * using namespace Mdt::QueryExpression;
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * SqlAsyncSelectQueryFactory factory;
   * if(!selectQueryFacotory->setup( parameters.toConnectionParameters() )){
   *   // Error handling. use setupError() to have mor informations
   * }
   *
   * ListPersonTableModel model;
   * auto query = factory.createSelectQuery();
   * connect(query.get(), &AsyncSelectQuery::newRecordAvailable, &model, &ListPersonTableModel::fromBackendAppendRecord);
   * connect(query.get(), &AsyncSelectQuery::errorOccured, &model, &ListPersonTableModel::setLastError);
   * \endcode
   *
   * Above example used Qt signal/slots, which also reduces coupling.
   *  It is also possible to hold the query in the model:
   * \code
   * class ListPersonTableModel : public Mdt::ItemModel::AbstractReadOnlyCachedTableModel
   * {
   *  Q_OBJECT
   *
   *   using SelectStatement = Mdt::QueryExpression::SelectStatement;
   *   using AsyncSelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
   *
   *  public:
   *
   *   ListPersonTableModel(QObject *parent = nullptr);
   *
   *   void setSelectQuery(std::unique_ptr<AsyncSelectQuery> && query);
   *
   *  private:
   *
   *   bool fetchRecords(int count) override;
   *
   *   SelectStatement mStatement;
   *   std::unique_ptr<AsyncSelectQuery> mSelectQuery;
   * };
   * \endcode
   *
   * Notice that the query hold in the use case is not shared,
   *  this is to avoid recieving events from the same instance that could
   *  accidentally be shared across the application.
   *  Using std::unique_ptr enforce this:
   * \code
   * ListPersonTableModel::setSelectQuery(std::unique_ptr<AsyncSelectQuery> && query)
   * {
   *   mSelectQuery = std::move(query);
   *   connect(query.get(), &AsyncSelectQuery::newRecordAvailable, this, &ListPersonTableModel::fromBackendAppendRecord);
   *   connect(query.get(), &AsyncSelectQuery::errorOccured, this, &ListPersonTableModel::setLastError);
   * }
   * \endcode
   *
   * Then, the above main becomes:
   * \code
   * using namespace Mdt::QueryExpression;
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * SqlAsyncSelectQueryFactory factory;
   * if(!selectQueryFacotory->setup( parameters.toConnectionParameters() )){
   *   // Error handling. use setupError() to have mor informations
   * }
   *
   * ListPersonTableModel model;
   * model.setSelectQuery( factory.createSelectQuery() );
   * \endcode
   *
   * \sa AbstractAsyncSelectQueryFacory
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT AbstractAsyncSelectQuery : public QObject
  {
    using BaseClass = QObject;

   Q_OBJECT

   public:

    /*! \brief Construct a select query
     */
    AbstractAsyncSelectQuery(QObject *parent = nullptr);

   public slots:

    /*! \brief Submit a select statement
     *
     * If \a maxRows is > 0, the result of executing \a statement
     *  should return a result limited to \a maxRows .
     *
     * \pre \a maxRows must be >= 0
     */
    virtual void submitStatement(const SelectStatement & statement, int maxRows = 0) = 0;

   signals:

    /*! \brief Emitted whenever a new record is available
     */
    void newRecordAvailable(const Mdt::Container::VariantRecord & record);

    /*! \brief Emitted whenever a error occured
     */
    void errorOccured(const Mdt::Error & error);
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_H
