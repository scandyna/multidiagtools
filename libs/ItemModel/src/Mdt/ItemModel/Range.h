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
#ifndef MDT_ITEM_MODEL_RANGE_H
#define MDT_ITEM_MODEL_RANGE_H

namespace Mdt{ namespace ItemModel{

  /*! \brief Range represents a range of rows or columns in item models
   */
  class Range
  {
   public:

    /*! \brief Clear range
     */
    constexpr void clear() noexcept
    {
      mFirst = -1;
      mLast = -1;
    }

    /*! \brief Check if range is valid
     *
     * This range is valid if:
     *  - Both first and last are >= 0
     *  - Last is >= first
     */
    constexpr bool isValid() const noexcept
    {
      return ( (mFirst >= 0) && (mLast >= 0) && (mLast >= mFirst) );
    }

    /*! \brief Check if range is empty
     */
    constexpr bool isEmpty() const noexcept
    {
      return ( count() <= 0);
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

   protected:

    /*! \brief Set first
     */
    constexpr void setFirst(int f) noexcept
    {
      mFirst = f;
    }

    /*! \brief Get first
     *
     * \warning first is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int first() const noexcept
    {
      return mFirst;
    }

    /*! \brief Set last
     */
    constexpr void setLast(int l) noexcept
    {
      mLast = l;
    }

    /*! \brief Get last
     *
     * \warning last is returned as it was set.
     *           The validity of this range should be checked
     *           before using the returned value.
     *
     * \sa isValid()
     */
    constexpr int last() const noexcept
    {
      return mLast;
    }

    /*! \brief Set count
     *
     * \note This method must be called after setFirst()
     */
    constexpr void setCount(int count) noexcept
    {
      mLast = mFirst + count - 1;
    }

    /*! \brief Get count
     *
     * Returns count of element in this range if it is valid,
     *  otherwise 0.
     *
     * \sa isValid()
     */
    constexpr int count() const noexcept
    {
      if(isValid()){
        return mLast - mFirst + 1;
      }
      return 0;
    }

   private:

    int mFirst = -1;
    int mLast = -1;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RANGE_H
