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

#include <vector>
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief A list of column indexes in a item model
   */
  class ColumnList
  {
   public:

    /*! \brief Const iterator
     */
    typedef std::vector<int>::const_iterator const_iterator;

    /*! \brief Construct a empty list
     */
    ColumnList() noexcept = default;

    /*! \brief Construct a list of columns
     *
     * \pre Each element in \a list must be >= 0
     * \pre Each element in \a list must be unique
     */
    ColumnList(std::initializer_list<int> list);

    /*! \brief Get count of columns
     */
    int size() const
    {
      return mColumnList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mColumnList.empty();
    }

    /*! \brief Get const iterator to the beginning of the list
     */
    const_iterator begin() const
    {
      return mColumnList.cbegin();
    }

    /*! \brief Get const iterator to the end of the list
     */
    const_iterator end() const
    {
      return mColumnList.cend();
    }

   private:

    std::vector<int> mColumnList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_LIST_H
