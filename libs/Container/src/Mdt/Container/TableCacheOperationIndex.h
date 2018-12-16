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
#ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_INDEX_H
#define MDT_CONTAINER_TABLE_CACHE_OPERATION_INDEX_H

#include "TableCacheOperation.h"
#include "MdtContainerExport.h"
#include "Mdt/Assert.h"
#include <QtGlobal>

namespace Mdt{ namespace Container{

  /*! \internal
   */
  class MDT_CONTAINER_EXPORT TableCacheOperationIndex
  {
   public:

    /*! \brief Construct a null index
     */
    TableCacheOperationIndex() = default;

    /*! \brief Construct a index with \a row and \a operation
     *
     * \pre \a row must be >= 0
     */
    TableCacheOperationIndex(int row, TableCacheOperation operation)
     : mRow(row),
       mOperation(operation)
    {
      Q_ASSERT(mRow >= 0);
    }

    /*! \brief Construct a index with \a row , \a column and \a operation
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    TableCacheOperationIndex(int row, int column, TableCacheOperation operation)
     : mRow(row),
       mColumn(column),
       mOperation(operation)
    {
      Q_ASSERT(mRow >= 0);
      Q_ASSERT(mColumn >= 0);
    }

    /*! \brief Copy construct a index from \a other
     */
    TableCacheOperationIndex(const TableCacheOperationIndex & other) = default;

    /*! \brief Copy assign \a other to this index
     */
    TableCacheOperationIndex & operator=(const TableCacheOperationIndex & other) = default;

    /*! \brief Move construct a index from \a other
     */
    TableCacheOperationIndex(TableCacheOperationIndex && other) = default;

    /*! \brief Move assign \a other to this index
     */
    TableCacheOperationIndex & operator=(TableCacheOperationIndex && other) = default;

    constexpr void shiftRow(int count) noexcept
    {
      mRow += count;
      MDT_ASSERT(mRow >= 0);
    }

    /*! \brief Set the row of this index
     */
    void setRow(int row)
    {
      Q_ASSERT(row >= 0);
      mRow = row;
    }

    /*! \brief Get the row of this index
     */
    int row() const
    {
      return mRow;
    }

    /*! \brief Get the column of this index
     */
    int column() const
    {
      return mColumn;
    }

    /*! \brief Set the operation of this index
     */
    void setOperation(TableCacheOperation op)
    {
      mOperation = op;
    }

    /*! \brief Get the operation of this index
     */
    TableCacheOperation operation() const
    {
      return mOperation;
    }

    /*! \brief Check if this index is null
     */
    bool isNull() const
    {
      return (mRow < 0);
    }

   private:

    int mRow = -1;
    int mColumn = -1;
    TableCacheOperation mOperation = TableCacheOperation::None;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_INDEX_H
