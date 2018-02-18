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
#ifndef MDT_ITEM_MODEL_PRIMARY_KEY_H
#define MDT_ITEM_MODEL_PRIMARY_KEY_H

#include "ColumnList.h"
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace ItemModel{

  /*! \brief List of columns in a item model that represents a primary key
   */
  class PrimaryKey
  {
   public:

    /*! \brief STL const iterator
     */
    typedef ColumnList::const_iterator const_iterator;

    /*! \brief Construct a null primary key
     */
    PrimaryKey() = default;

    /*! \brief Construct a primary key
     *
     * \pre Each element in \a list must be >= 0
     * \pre Each element in \a list must be unique
     */
    explicit PrimaryKey(std::initializer_list<int> list)
     : mColumnList(list)
    {
    }

    PrimaryKey(const PrimaryKey &) = default;
    PrimaryKey & operator=(const PrimaryKey &) = default;
    PrimaryKey(PrimaryKey &&) = default;
    PrimaryKey & operator=(PrimaryKey &&) = default;

    /*! \brief Add \a column to this primary key
     *
     * \pre \a column must be >= 0
     * \pre \a column must not allready exist in this primary key
     */
    void addColumn(int column)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(!containsColumn(column));
      mColumnList.append(column);
    }

    /*! \brief Get count of columns
     */
    int columnCount() const
    {
      return mColumnList.size();
    }

    /*! \brief Check if this primary key is null
     */
    bool isNull() const noexcept
    {
      return mColumnList.isEmpty();
    }

    /*! \brief Get column at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < columnCount() ).
     */
    int columnAt(int index) const noexcept
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < columnCount());
      return mColumnList.at(index);
    }

    /*! \brief Get the greatest column
     *
     * Returns -1 if this list is empty.
     */
    int greatestColumn() const
    {
      return mColumnList.greatestColumn();
    }

    /*! \brief Check if this primary key contains a column
     */
    bool containsColumn(int column) const
    {
      return mColumnList.contains(column);
    }

    /*! \brief Clear
     */
    void clear()
    {
      mColumnList.clear();
    }

    /*! \brief Get const iterator to the first column
     */
    const_iterator begin() const
    {
      return mColumnList.begin();
    }

    /*! \brief Get const iterator to the last
     */
    const_iterator end() const
    {
      return mColumnList.end();
    }

    /*! \brief Get a primary key from a column list
     */
    static PrimaryKey fromColumnList(const ColumnList & columnList)
    {
      PrimaryKey pk;
      pk.mColumnList = columnList;
      return pk;
    }

    /*! \brief Get columns list
     */
    ColumnList toColumnList() const
    {
      return mColumnList;
    }

   private:

    ColumnList mColumnList;
  };

}} // namespace Mdt{ namespace ItemModel{

Q_DECLARE_METATYPE(Mdt::ItemModel::PrimaryKey)

#endif // #ifndef MDT_ITEM_MODEL_PRIMARY_KEY_H
