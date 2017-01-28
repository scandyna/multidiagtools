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
#ifndef MDT_ITEM_MODEL_ROW_RANGE_H
#define MDT_ITEM_MODEL_ROW_RANGE_H

namespace Mdt{ namespace ItemModel{

  /*! \brief RowRange represents a range of rows in item models
   *
   * \todo Replace preconditions with checkers
   */
  class RowRange
  {
   public:

    /*! \brief Set first row
     */
    constexpr void setFirstRow(int row) noexcept
    {
      mFirstRow = row;
    }

    /*! \brief Get first row
     *
     * \warning The first row is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int firstRow() const noexcept
    {
      return mFirstRow;
    }

    /*! \brief Set last row
     */
    constexpr void setLastRow(int row) noexcept
    {
      mLastRow = row;
    }

    /*! \brief Get last row
     *
     * \warning The last row is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int lastRow() const noexcept
    {
      return mLastRow;
    }

    /*! \brief Set row count
     *
     * \note This method must be called after setFirstRow()
     */
    constexpr void setRowCount(int count) noexcept
    {
      mLastRow = mFirstRow + count - 1;
    }

    /*! \brief Get row count
     *
     * Returns row count of this range if it is valid,
     *  otherwise 0.
     *
     * \sa isValid()
     */
    constexpr int rowCount() const noexcept
    {
      if(isValid()){
        return mLastRow - mFirstRow + 1;
      }
      return 0;
    }

    /*! \brief Clear range
     */
    constexpr void clear() noexcept
    {
      mFirstRow = -1;
      mLastRow = -1;
    }

    /*! \brief Check if range is valid
     *
     * This range is valid if:
     *  - Both first and last are >= 0
     *  - Last is >= first
     */
    constexpr bool isValid() const noexcept
    {
      return ( (mFirstRow >= 0) && (mLastRow >= 0) && (mLastRow >= mFirstRow) );
    }

    /*! \brief Check if range is empty
     */
    constexpr bool isEmpty() const noexcept
    {
      return ( rowCount() <= 0);
    }

    /*! \brief Check if range is null
     *
     * This is the same as isEmpty()
     *
     * \sa isEmpty()
     */
    constexpr bool isNull() const noexcept
    {
      return isEmpty();
    }

   private:

    int mFirstRow = -1;
    int mLastRow = -1;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_RANGE_H
