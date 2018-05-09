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
#ifndef MDT_INDEX_RANGE_COLUMN_RANGE_H
#define MDT_INDEX_RANGE_COLUMN_RANGE_H

#include "Mdt/Numeric/IntegralRange.h"
#include "MdtIndexRangeExport.h"

namespace Mdt{ namespace IndexRange{

  /*! \brief ColumnRange represents a range of columns in item models
   */
  class MDT_INDEXRANGE_EXPORT ColumnRange
  {
   public:

    /*! \brief Set first column
     */
    constexpr void setFirstColumn(int column) noexcept
    {
      mRange.setFirst(column);
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
      return mRange.first();
    }

    /*! \brief Set last column
     */
    constexpr void setLastColumn(int column) noexcept
    {
      mRange.setLast(column);
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
      return mRange.last();
    }

    /*! \brief Set column count
     *
     * \note This method must be called after setFirstColumn()
     */
    constexpr void setColumnCount(int count) noexcept
    {
      mRange.setCount(count);
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
      return mRange.count();
    }

    /*! \brief Check if this range contains column
     */
    constexpr bool containsColumn(int column) const noexcept
    {
      return mRange.contains(column);
    }

    /*! \brief Check if range is valid
     *
     * This range is valid if:
     *  - At least 1 boundary was set
     *  - lastRow is >= firstRow
     */
    constexpr bool isValid() const noexcept
    {
      return mRange.isValid();
    }

    /*! \brief Check if range is empty
     */
    constexpr bool isEmpty() const noexcept
    {
      return mRange.isEmpty();
    }

    /*! \brief Check if range is null
     *
     * This is the same as isEmpty()
     *
     * \sa isEmpty()
     */
    constexpr bool isNull() const noexcept
    {
      return mRange.isNull();
    }

   private:

    Mdt::Numeric::IntegralRange<int> mRange;
  };

}} // namespace Mdt{ namespace IndexRange{

#endif // #ifndef MDT_INDEX_RANGE_COLUMN_RANGE_H
