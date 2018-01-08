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
#ifndef MDT_ITEM_MODEL_ROW_RESIZABLE_STL_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ROW_RESIZABLE_STL_TABLE_MODEL_H

#include "EditableStlTableModel.h"
#include "StlContainer.h"
#include "MdtItemModelExport.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Resizable table model for STL compliant container
   *
   * \tparam Table Type of a STL compatible container.
   *   It must meet requirement for EditableStlTableModel,
   *    and additionaly provide:
   *     - A STL compatible insert() method
   *     - A STL compatible erase() method
   *     - The record_type must also provide insert() and erase().
   *     - The record_type must provide a constructor of type: record_type(size_type count)
   *     - The value_type must be default constructible
   */
  template<typename Table>
  class MDT_ITEMMODEL_EXPORT RowResizableStlTableModel : public EditableStlTableModel<Table>
  {
   public:

    using record_type = typename EditableStlTableModel<Table>::record_type;
    using value_type = typename EditableStlTableModel<Table>::value_type;

    using EditableStlTableModel<Table>::rowCount;
    using EditableStlTableModel<Table>::columnCount;
    using EditableStlTableModel<Table>::index;
    using EditableStlTableModel<Table>::table;
    using EditableStlTableModel<Table>::isEmpty;
    using EditableStlTableModel<Table>::beginResetModel;
    using EditableStlTableModel<Table>::endResetModel;
    using EditableStlTableModel<Table>::afterTableWasSet;
    using EditableStlTableModel<Table>::beginInsertRows;
    using EditableStlTableModel<Table>::endInsertRows;
    using EditableStlTableModel<Table>::beginRemoveRows;
    using EditableStlTableModel<Table>::endRemoveRows;

    /*! \brief Set column count
     *
     * \pre internal table must be empty
     * \sa isEmpty()
     */
    void setColumnCount(int count)
    {
      Q_ASSERT(isEmpty());

      beginResetModel();
      mColumnCount = count;
      endResetModel();
    }

    /*! \brief Get column count
     */
    int columnCount() const override
    {
      return mColumnCount;
    }

    /*! \brief Append a row
     *
     * \pre columnCount() must be > 0
     */
    void appendRow()
    {
      insertRows(rowCount(), 1);
    }

    /*! \brief Prepend a row
     *
     * \pre columnCount() must be > 0
     */
    void prependRow()
    {
      insertRows(0, 1);
    }

    /*! \brief Insert \a count rows before \a row
     *
     * \pre \a row must be in valid range ( 0 <= row <= rowCount() )
     * \pre \a count must be > 0
     * \pre columnCount() must be > 0
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(count > 0);
      Q_ASSERT(columnCount() > 0);

      if(parent.isValid()){
        return false;
      }
      beginInsertRows(parent, row, row+count-1);
      insertToTable(table(), row, count, columnCount(), value_type());
      endInsertRows();

      return true;
    }

    /*! \brief Remove first row
     */
    void removeFirstRow()
    {
      if(rowCount() < 1){
        return;
      }
      removeRows(0, 1);
    }

    /*! \brief Remove last row
     */
    void removeLastRow()
    {
      if(rowCount() < 1){
        return;
      }
      removeRows( rowCount()-1, 1 );
    }

    /*! \brief Implement QAbstractTableModel::removeRows()
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     * \pre \a row + \a count must be in correct range ( 0 <= \a row + \a count <= rowCount() )
     */
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(count >= 1);
      Q_ASSERT( (row + count ) <= rowCount() );

      if(parent.isValid()){
        return false;
      }
      beginRemoveRows(parent, row, row+count-1);
      removeFromContainer(table(), row, count);
      endRemoveRows();

      return true;
    }

   private:

    void afterTableWasSet() override
    {
      mColumnCount = EditableStlTableModel<Table>::columnCount();
    }

    int mColumnCount = 0;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_RESIZABLE_STL_TABLE_MODEL_H
