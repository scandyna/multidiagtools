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
#ifndef MDT_ITEM_MODEL_ROW_COLUMN_LIST_BASE_H
#define MDT_ITEM_MODEL_ROW_COLUMN_LIST_BASE_H

#include <QtGlobal>
#include <vector>
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief Common base for RowList and ColumnList
   */
  class RowColumnListBase
  {
   public:

    /*! \brief Const iterator
     */
    typedef std::vector<int>::const_iterator const_iterator;

    /*! \brief Construct a empty list
     */
    RowColumnListBase() noexcept = default;

    /*! \brief Construct a list of elements
     *
     * \pre Each element in \a list must be >= 0
     * \pre Each element in \a list must be unique
     */
    explicit RowColumnListBase(std::initializer_list<int> list);

    /*! \brief Get size of the list
     */
    int size() const
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.empty();
    }

    /*! \brief Get element at index
     *
     * \pre index must be >= 0
     * \pre index must be < size()
     */
    int at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return mList[index];
    }

    /*! \brief Check if this list contains a element
     */
    bool contains(int e) const;

    /*! \brief Append a element to the end of this list
     *
     * \pre \a e must be >= 0
     * \pre \a e must not allready exists in this list
     */
    void append(int e);

    /*! \brief Get the greatest element
     *
     * Returns -1 if this list is empty.
     */
    int greatest() const;

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get const iterator to the beginning of the list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Get const iterator to the end of the list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

   private:

    std::vector<int> mList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_COLUMN_LIST_BASE_H
