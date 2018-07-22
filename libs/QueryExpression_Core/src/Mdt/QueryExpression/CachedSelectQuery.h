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
#ifndef MDT_QUERY_EXPRESSION_CACHED_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_CACHED_SELECT_QUERY_H

#include "AbstractSelectQuery.h"
#include "SelectStatement.h"
#include "SelectField.h"
#include "Mdt/Error.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <QVariant>
#include <memory>
#include <vector>

namespace Mdt{ namespace QueryExpression{

  /*! \brief CachedSelectQuery can execute a SelectStatement
   *
   * Like SelectQuery, CachedSelectQuery has value sementics, but it is a handle to a implementation instance
   *  (internally implemented using std::shared_ptr).
   *
   * \sa SelectQuery
   * \sa Mdt::ItemModel::SelectQueryTableModel
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT CachedSelectQuery
  {
    using Record = std::vector<QVariant>;
    using Table = std::vector<Record>;

   public:

    /*! \brief Construct a null query
     */
    CachedSelectQuery() noexcept = default;

    /*! \brief Copy construct a query from \a other
     */
    CachedSelectQuery(const CachedSelectQuery & other) = default;

    /*! \brief Copy assign \a other to this query
     */
    CachedSelectQuery & operator=(const CachedSelectQuery & other) = default;

    /*! \brief Move construct a query from \a other
     */
    CachedSelectQuery(CachedSelectQuery && other) = default;

    /*! \brief Move assign \a other to this query
     */
    CachedSelectQuery & operator=(CachedSelectQuery && other) = default;

    /*! \brief Check if this query is null
     */
    bool isNull() const
    {
      return (mImpl.get() == nullptr) || (mCache.get() == nullptr);
    }

    /*! \brief Execute \a statement
     *
     * \pre This query must not be null
     */
    bool exec(const SelectStatement & statement);

    /*! \brief Get the count of rows in the cache of this query
     */
    int rowCount() const noexcept
    {
      if(isNull()){
        return 0;
      }
      return mCache->size();
    }

    /*! \brief Get the count of columns in the cache of this query
     */
    int columnCount() const noexcept
    {
      if(isNull()){
        return 0;
      }
      return mImpl->fieldCount();
    }

    /*! \brief Get the value at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     * \pre This query must not be null
     */
    QVariant value(int row, int column) const noexcept
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      Q_ASSERT(!isNull());

      return (*mCache)[row][column];
    }

    /*! \brief Get last error
     *
     * \pre This query must not be null
     */
    Mdt::Error lastError() const;

    /*! \brief Create a select query with a concrete implementation
     */
    template<typename QueryImpl>
    static CachedSelectQuery make()
    {
      return CachedSelectQuery( std::make_shared<QueryImpl>() );
    }

    /*! \brief Access the hold query implementation
     *
     * Returns a reference to the implementation.
     *
     * \pre \a QueryImpl must be the real implementation
     */
    template<typename QueryImpl>
    QueryImpl & impl()
    {
      Q_ASSERT(!isNull());
      Q_ASSERT(dynamic_cast<QueryImpl*>( mImpl.get()) != nullptr);
      return *( dynamic_cast<QueryImpl*>( mImpl.get()) );
    }

   private:

    CachedSelectQuery(std::shared_ptr<AbstractSelectQuery> && queryImpl)
     : mImpl(queryImpl),
       mCache( std::make_shared<Table>() )
    {
    }

    Record buildCurrentRecord() const;

    std::shared_ptr<AbstractSelectQuery> mImpl;
    std::shared_ptr<Table> mCache;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_CACHED_SELECT_QUERY_H
