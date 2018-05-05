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
#ifndef MDT_ITEM_MODEL_ROW_SELECTION_ALGORITHM_H
#define MDT_ITEM_MODEL_ROW_SELECTION_ALGORITHM_H

#include "RowRange.h"
#include "RowRangeList.h"
#include "MdtItemModelExport.h"
#include <algorithm>

namespace Mdt{ namespace ItemModel{

  /*! \brief Check if range \a a is included in range \a b
   *
   * Returns true if range \a a is completely included in range \a b .
   *  If both ranges just overlapps, no one is included
   *  and this method returns false.
   *
   * \pre ranges \a a and \a b must both be valid
   */
  bool MDT_ITEMMODEL_EXPORT isRangeIncluded(const RowRange & a, const RowRange & b);

  /*! \brief Compare ranges \a a and \a b
   *
   * Returns true if range \a a < \a b
   *
   * \pre ranges \a a and \a b must both be valid
   * \pre ranges \a a and \a b must not include each other
   */
  bool MDT_ITEMMODEL_EXPORT compareRanges(const RowRange & a, const RowRange & b);

  /*! \brief Sort the ranges in \a list
   */
  void MDT_ITEMMODEL_EXPORT sortRanges(RowRangeList & list);

  /*! \brief Check if ranges \a a and \a b are adjacent
   *
   * \pre ranges \a a and \a b must both be valid
   * \pre range \a a must be < range \a b
   * \pre ranges \a a and \a b must not include each other
   */
  bool MDT_ITEMMODEL_EXPORT isRangeAdjacent(const RowRange & a, const RowRange & b);

  /*! \brief Find adjacent row ranges
   *
   * \pre The element in the range [first, last) must be sorted
   */
  template<typename ForwardIt>
  ForwardIt findAdjacentRanges(ForwardIt first, ForwardIt last)
  {
    const auto pred = [](const RowRange & a, const RowRange & b){
      return isRangeAdjacent(a, b);
    };
    return std::adjacent_find(first, last, pred);
  }

  bool MDT_ITEMMODEL_EXPORT hasAdjacentRanges(const RowRangeList & list);

  /*! \brief Find the last row range that is adjacent to the one pointed by \a first
   *
   * \pre The elements in [first, last) must not represent a empty container
   * \pre The elements in [first, last) must be sorted and all valid
   * \return A iterator to the last asjacent range in [first, last).
   *      If 2 adjacent ranges are not found, \a first is returned.
   * \post Returns allways a dereferencable iterator
   */
  RowRangeList::const_iterator MDT_ITEMMODEL_EXPORT findLastAdjacentRange(RowRangeList::const_iterator first, RowRangeList::const_iterator last);

  /*! \brief Merge ranges \a a and \a b
   *
   * \pre ranges \a a and \a b must both be valid
   * \pre range \a a must be < range \a b
   * \pre ranges \a a and \a b must not include each other
   * 
   * \pre ranges \a a and \a b must be adjacent <-- WRONG
   */
  RowRange MDT_ITEMMODEL_EXPORT mergeRanges(const RowRange & a, const RowRange & b);

  /*! \brief Merge adjacent ranges in \a list
   */
  void MDT_ITEMMODEL_EXPORT mergeAdjacentRanges(RowRangeList & list);


}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_SELECTION_ALGORITHM_H
