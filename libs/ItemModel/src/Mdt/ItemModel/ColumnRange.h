/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_COLUMN_RANGE_H
#define MDT_ITEM_MODEL_COLUMN_RANGE_H

#include "Range.h"
#include <QModelIndex>

namespace Mdt{ namespace ItemModel{

  /*! \brief ColumnRange represents a range of columns in item models
   */
  class ColumnRange : public Range
  {
   public:

    /*! \brief Set first column
     */
    constexpr void setFirstColumn(int column) noexcept
    {
      setFirst(column);
    }

    /*! \brief Set first model index
     */
    constexpr void setFirstIndex(const QModelIndex & index) noexcept
    {
      setFirstColumn(index.column());
    }

    /*! \brief Get first column
     *
     * \warning The first column is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int firstColumn() const noexcept
    {
      return first();
    }

    /*! \brief Set last column
     */
    constexpr void setLastColumn(int column) noexcept
    {
      setLast(column);
    }

    /*! \brief Set last model index
     */
    constexpr void setLastIndex(const QModelIndex & index) noexcept
    {
      setLastColumn(index.column());
    }

    /*! \brief Get last column
     *
     * \warning The last column is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int lastColumn() const noexcept
    {
      return last();
    }

    /*! \brief Set column count
     *
     * \note This method must be called after setFirstColumn()
     */
    constexpr void setColumnCount(int count) noexcept
    {
      setCount(count);
    }

    /*! \brief Get column count
     *
     * Returns column count of this range if it is valid,
     *  otherwise 0.
     *
     * \sa isValid()
     */
    constexpr int columnCount() const noexcept
    {
      return count();
    }
    
    /*! \brief Check if this range contains column
     */
    constexpr bool containsColumn(int column) const noexcept
    {
      return contains(column);
    }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_RANGE_H
