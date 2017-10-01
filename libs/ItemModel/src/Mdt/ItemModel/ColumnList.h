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
#ifndef MDT_ITEM_MODEL_COLUMN_LIST_H
#define MDT_ITEM_MODEL_COLUMN_LIST_H

#include "RowColumnListBase.h"
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief A list of column indexes in a item model
   */
  class ColumnList : public RowColumnListBase
  {
   public:

    /*! \brief Construct a empty list
     */
    ColumnList() noexcept = default;

    /*! \brief Construct a list of columns
     *
     * \pre Each column in \a list must be >= 0
     * \pre Each column in \a list must be unique
     */
    explicit ColumnList(std::initializer_list<int> list)
     : RowColumnListBase(list)
    {}

    ColumnList(const ColumnList &) = default;
    ColumnList & operator=(const ColumnList &) = default;
    ColumnList(ColumnList &&) = default;
    ColumnList & operator=(ColumnList &&) = default;

    /*! \brief Get the greatest column
     *
     * Returns -1 if this list is empty.
     */
    int greatestColumn() const
    {
      return greatest();
    }

  };

}} // namespace Mdt{ namespace ItemModel{

Q_DECLARE_METATYPE(Mdt::ItemModel::ColumnList)

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_LIST_H
