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
#ifndef MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_LIST_H
#define MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_LIST_H

#include "ColumnSortStringAttributes.h"
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief List of ColumnSortStringAttributes used by SortProxyModel
   */
  class ColumnSortStringAttributesList
  {
   public:

    /*! \brief Add a column
     */
    void addColumn(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode);

    /*! \brief Get attributes for column
     *
     * \pre column must exist in this list
     */
    const ColumnSortStringAttributes & attributesForColumn(int column) const;

   private:

    std::vector<ColumnSortStringAttributes> mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_LIST_H
