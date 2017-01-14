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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_H

#include "ColumnList.h"
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief List of columns in a item model that represents a foreign key
   */
  class ForeignKey
  {
   public:

    /*! \brief Construct a null foreign key
     */
    ForeignKey() = default;

    /*! \brief Construct a foreign key
     *
     * \pre Each element in \a list must be >= 0
     * \pre Each element in \a list must be unique
     */
    explicit ForeignKey(std::initializer_list<int> list)
     : mColumnList(list)
    {
    }

    /*! \brief Check if this foreign key is null
     */
    bool isNull() const noexcept
    {
      return mColumnList.isEmpty();
    }

    /*! \brief Get the greates column
     *
     * Returns -1 if this list is empty.
     */
    int greatestColumn() const
    {
      return mColumnList.greatestColumn();
    }

    /*! \brief Clear
     */
    void clear()
    {
      mColumnList.clear();
    }

   private:

    ColumnList mColumnList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_H
