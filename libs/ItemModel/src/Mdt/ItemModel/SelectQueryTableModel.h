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
#ifndef MDT_ITEM_MODEL_SELECT_QUERY_TABLE_MODEL_H
#define MDT_ITEM_MODEL_SELECT_QUERY_TABLE_MODEL_H

#include "AbstractTableModel.h"
#include "Mdt/QueryExpression/CachedSelectQuery.h"
#include "MdtItemModelExport.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Table model to represent the result of a Mdt::QueryExpression::SelectStatement
   *
   * SelectQueryTableModel takes one of the concept of the \ref libs_QueryExpression library,
   *  which is to not depend to a storage backend in the business code.
   *
   * The following example shows how to display the result of a select statement
   *  using a SQL backend:
   * \code
   * using namespace Mdt::ItemModel;
   * using namespace Mdt::QueryExpression;
   *
   * auto query = CachedSelectQuery::make<SqlSelectQuery>();
   *
   * SelectQueryTableModel model;
   * model.setQuery(query);
   * if( !model.setStatement( PersonAbove29Statement() ) ){
   *   // Error handling
   * }
   *
   * QTableView view;
   * view.setModel(&model);
   * view.show();
   * \endcode
   *
   * To see how to setup the query and the select statement,
   *  see the examples from the \ref libs_QueryExpression library.
   */
  class MDT_ITEMMODEL_EXPORT SelectQueryTableModel : public AbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    SelectQueryTableModel(QObject* parent = nullptr);

    /*! \brief Set \a query to this model
     */
    void setQuery(const Mdt::QueryExpression::CachedSelectQuery & query);

    /*! \brief Set \a statement to this model
     *
     * \pre A valid query must have been set to this model
     */
    bool setStatement(const Mdt::QueryExpression::SelectStatement & statement);

   private:

    int rowCountImpl() const override
    {
      return mQuery.rowCount();
    }

    int columnCountImpl() const override
    {
      return mQuery.columnCount();
    }

    QVariant displayRoleData(int row, int column) const override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCountImpl());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCountImpl());

      if(mQuery.isNull()){
        return QVariant();
      }
      return mQuery.value(row, column);
    }

    Mdt::QueryExpression::CachedSelectQuery mQuery;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_SELECT_QUERY_TABLE_MODEL_H
