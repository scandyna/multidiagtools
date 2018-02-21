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
#ifndef MDT_ITEM_MODEL_RESIZABLE_STL_LIST_MODEL_H
#define MDT_ITEM_MODEL_RESIZABLE_STL_LIST_MODEL_H

#include "EditableStlListModel.h"
#include <type_traits>
#include <iterator>

namespace Mdt{ namespace ItemModel{

  /*! \brief Resizable list model for STL compliant container
   *
   * \tparam Container Type of a STL compatible container.
   *   It must meet requirement for EditableStlListModel,
   *    and additionaly provide:
   *     - A STL compatible insert() method
   *     - A STL compatible erase() method
   */
  template<typename Container>
  class ResizableStlListModel : public EditableStlListModel<Container>
  {
   public:

    using value_type = typename EditableStlListModel<Container>::value_type;

    using EditableStlListModel<Container>::rowCount;
    using EditableStlListModel<Container>::index;
    using EditableStlListModel<Container>::container;
    using EditableStlListModel<Container>::beginInsertRows;
    using EditableStlListModel<Container>::endInsertRows;
    using EditableStlListModel<Container>::beginRemoveRows;
    using EditableStlListModel<Container>::endRemoveRows;

    /*! \brief Append a row
     */
    void appendRow()
    {
      insertRows(rowCount(), 1);
    }

    /*! \brief Prepend a row
     */
    void prependRow()
    {
      insertRows(0, 1);
    }

    /*! \brief Implement QAbstractTableModel::insertRows()
     *
     * \pre \a row must be in valid range ( 0 <= row <= rowCount() )
     * \pre \a count must be >= 1
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(count >= 1);

      if(parent.isValid()){
        return false;
      }
      beginInsertRows(parent, row, row+count-1);
      Mdt::Container::insertToContainer(container(), row, count, value_type());
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
      Mdt::Container::removeFromContainer(container(), row, count);
      endRemoveRows();

      return true;
    }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RESIZABLE_STL_LIST_MODEL_H
