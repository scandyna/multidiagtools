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
#ifndef MDT_ITEM_MODEL_COLUMN_SORT_ORDER_H
#define MDT_ITEM_MODEL_COLUMN_SORT_ORDER_H

#include <Qt>
#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Item of a ColumnSortOrderList
   */
  class ColumnSortOrder
  {
   public:

    /*! \brief Constructor
     *
     * \pre column must be >= 0
     */
    constexpr ColumnSortOrder(int column, Qt::SortOrder sortOrder) noexcept
     : mColumn(column),
       mSortOrder(sortOrder)
    {
      Q_ASSERT(mColumn >= 0);
    }

    /*! \brief Get column
     */
    constexpr int column() const noexcept
    {
      return mColumn;
    }

    /*! \brief Get sort order
     */
    constexpr Qt::SortOrder sortOrder() const noexcept
    {
      return mSortOrder;
    }

   private:

    int mColumn;
    Qt::SortOrder mSortOrder;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // MDT_ITEM_MODEL_COLUMN_SORT_ORDER_H
