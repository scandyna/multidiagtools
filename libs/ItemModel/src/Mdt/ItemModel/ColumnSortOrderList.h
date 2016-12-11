/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_COLUMN_SORT_ORDER_LIST_H
#define MDT_ITEM_MODEL_COLUMN_SORT_ORDER_LIST_H

#include "ColumnSortOrder.h"
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief List of column sort order used by SortProxyModel
   */
  class ColumnSortOrderList
  {
   public:

    /*! \brief Const iterator
     */
    typedef std::vector<ColumnSortOrder>::const_reverse_iterator const_reverse_iterator;

    /*! \brief Add a column to sort order
     */
    void addColumn(int column, Qt::SortOrder sortOrder)
    {
      mList.emplace_back(column, sortOrder);
    }

    /*! \brief Get a reverse iterator to the beginning
     */
    const_reverse_iterator crbegin() const
    {
      return mList.crbegin();
    }

    /*! \brief Get a reverse iterator to the end
     */
    const_reverse_iterator crend() const
    {
      return mList.crend();
    }

    /*! \brief Check if list is empty
     */
    bool isEmpty() const
    {
      return mList.empty();
    }

    /*! \brief Clear list
     */
    void clear()
    {
      mList.clear();
    }

   private:

    std::vector<ColumnSortOrder> mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // MDT_ITEM_MODEL_COLUMN_SORT_ORDER_LIST_H
