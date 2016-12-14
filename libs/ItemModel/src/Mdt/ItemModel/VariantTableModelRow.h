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
#ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ROW_H
#define MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ROW_H

#include "VariantTableModelItem.h"
#include "VariantTableModelStorageRule.h"
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Container for VariantTableModel
   */
  class VariantTableModelRow
  {
   public:

    /*! \brief Construct a row with colCount columns
     */
    VariantTableModelRow(VariantTableModelStorageRule storageRule, int colCount)
     : mRowData(colCount, VariantTableModelItem(storageRule))
    {
    }

    /*! \brief Get column count
     */
    int columnCount() const
    {
      return mRowData.size();
    }

    /*! \brief Get data at column
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * \pre column must be in valid range
     *      ( 0 <= column < columnCount() )
     * \pre role must be Qt::DisplayRole or Qt::EditRole
     */
    QVariant data(int column, int role) const
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      Q_ASSERT( (role == Qt::DisplayRole) || (role == Qt::EditRole) );
      return mRowData[column].data(role);
    }

    /*! \brief Set item enabled at column
     *
     * \pre column must be in valid range
     *      ( 0 <= column < columnCount() )
     */
    void setItemEnabled(int column, bool enable)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      mRowData[column].setEnabled(enable);
    }

    /*! \brief Set item editable at column
     *
     * \pre column must be in valid range
     *      ( 0 <= column < columnCount() )
     */
    void setItemEditable(int column, bool editable)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      mRowData[column].setEditable(editable);
    }

    /*! \brief Get flags at column
     *
     * Returns currentFlags with Qt::ItemIsEditable and ItemIsEnabled
     *  set or unset regarding what was set ba setItemEditable() and setItemEnabled().
     *
     * \pre column must be in valid range
     *      ( 0 <= column < columnCount() )
     */
    Qt::ItemFlags flags(int column, Qt::ItemFlags currentFlags) const
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      return mRowData[column].flags(currentFlags);
    }

    /*! \brief Set data at column
     *
     * If storage rule (passed in constructor) is SeparateDisplayAndEditRoleData,
     *  data for Qt::EditRole will be distinct from data for Qt::DisplayRole.
     *
     * \pre column must be in valid range
     *      ( 0 <= column < columnCount() )
     * \pre role must be Qt::DisplayRole or Qt::EditRole
     */
    void setData(int column, const QVariant & value, int role = Qt::EditRole)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      Q_ASSERT( (role == Qt::DisplayRole) || (role == Qt::EditRole) );
      mRowData[column].setData(value, role);
    }

    /*! \brief Resize row to columns count of columns
     *
     * For new count columns that is greater than current count,
     *  new null elements are added.
     *  For count that is less than current one, elements are removed.
     *
     * \pre columns must be >= 0
     */
    void resize(int columns, VariantTableModelStorageRule storageRule)
    {
      Q_ASSERT(columns >= 0);
      mRowData.resize(columns, storageRule);
    }

    /*! \brief Insert \a count columns before \a column
     *
     * If column is 0, new columns are prepended.
     * If column is columnCount(), columns are appended.
     *
     * \pre \a column must be in correct range ( 0 <= column <= columnCount() )
     * \pre \a count must be >= 1
     */
    void insertColumns(int column, int count, VariantTableModelStorageRule storageRule)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column <= columnCount());
      Q_ASSERT(count >= 1);
      mRowData.insert( mRowData.cbegin() + column, count, storageRule );
    }

    /*! \brief Remove \a count columns starting with \a column
     *
     * \pre \a column must be >= 0
     * \pre \a count must be >= 1
     * \pre \a column + \a count must be in correct range ( 0 <= column <= columnCount() )
     */
    void removeColumns(int column, int count)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(count >= 1);
      Q_ASSERT( (column + count ) <= columnCount() );
      mRowData.erase(mRowData.cbegin()+column, mRowData.cbegin()+column+count);
    }

   private:

    std::vector<VariantTableModelItem> mRowData;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_VARIANT_TABLE_MODEL_ROW_H
