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
     *
     * \pre No attribute must allready been set for \a column
     * \pre column must be >= 0
     */
    void addColumn(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode);

    /*! \brief Add or update a column
     *
     * If \a column not allready exists, it will be added, else it will be updated
     *
     * \pre column must be >= 0
     */
    void setColumn(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode);

    /*! \brief Get attributes for column
     *
     * If column not exists in this list,
     *  null attributes is returned.
     *
     * \pre column must be >= 0
     */
    ColumnSortStringAttributes attributesForColumn(int column) const;

    /*! \brief Clear
     */
    void clear();

   private:

    std::vector<ColumnSortStringAttributes> mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_LIST_H
