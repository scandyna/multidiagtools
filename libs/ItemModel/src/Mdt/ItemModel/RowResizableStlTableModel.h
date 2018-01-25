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
#include "StlContainerIteratorAdapter.h"
#include "StlContainer.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Resizable table model for STL compliant container
   *
   * \tparam Table Type of a STL compatible container.
   *   It must meet requirement for EditableStlTableModel,
   *    and additionaly provide:
   *     - A STL compatible insert() method
   *     - A STL compatible erase() method
   *
   * \tparam RecordAdapter Class template that meets the requirement
   *   of EditableStlTableModel, and that also provides following functions:
   *    - get a initialized record container :
   *     \code
   *     Container initializeContainer(int count, const value_type & value);
   *     \endcode
   */
  template<typename Table, typename RecordAdapter = StlContainerIteratorAdapter<typename Table::value_type> >
  class RowResizableStlTableModel : public EditableStlTableModel<Table>
  {
   public:

    using ParentClass = EditableStlTableModel<Table, RecordAdapter>;

    using record_type = typename ParentClass::record_type;
    using value_type = typename ParentClass::value_type;

    using ParentClass::rowCount;
    using ParentClass::columnCount;
    using ParentClass::index;
    using ParentClass::table;
    using ParentClass::isEmpty;

   protected:

    using ParentClass::constRecordAdapter;
    using ParentClass::beginResetModel;
    using ParentClass::endResetModel;
    using ParentClass::afterTableWasSet;
    using ParentClass::beginInsertRows;
    using ParentClass::endInsertRows;
    using ParentClass::beginRemoveRows;
    using ParentClass::endRemoveRows;

   public:

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
      insertToContainer( table(), row, count, constRecordAdapter().initializeContainer( columnCount(), value_type() ) );
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
      mColumnCount = ParentClass::columnCount();
    }

    int mColumnCount = 0;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_RESIZABLE_STL_TABLE_MODEL_H
