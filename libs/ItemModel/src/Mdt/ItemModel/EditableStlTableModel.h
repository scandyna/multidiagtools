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
#ifndef MDT_ITEM_MODEL_EDITABLE_STL_TABLE_MODEL_H
#define MDT_ITEM_MODEL_EDITABLE_STL_TABLE_MODEL_H

#include "ReadOnlyStlTableModel.h"
#include "MdtItemModelExport.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Editable table model for STL compliant container
   *
   * \tparam Table Type of a STL compatible container.
   *   It must meet requirement for ReadOnlyStlTableModel,
   *    and additionaly provide:
   *     - The begin() method
   *     - The end() method
   *     - The record_type must also provide begin() and end().
   */
  template<typename Table>
  class MDT_ITEMMODEL_EXPORT EditableStlTableModel : public ReadOnlyStlTableModel<Table>
  {
   public:

    using record_type = typename ReadOnlyStlTableModel<Table>::record_type;
    using value_type = typename ReadOnlyStlTableModel<Table>::value_type;

    using ReadOnlyStlTableModel<Table>::rowCount;
    using ReadOnlyStlTableModel<Table>::columnCount;
    using ReadOnlyStlTableModel<Table>::index;
    using ReadOnlyStlTableModel<Table>::table;
    using ReadOnlyStlTableModel<Table>::emitDataChangedSignal;

    /*! \brief Get flags for item at \a index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override
    {
      if(!index.isValid()){
        return ReadOnlyStlTableModel<Table>::flags(index);
      }
      return ( ReadOnlyStlTableModel<Table>::flags(index) | Qt::ItemIsEditable );
    }

    /*! \brief Set element at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setValue(int row, int column, const value_type & value)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      *iteratorAtRowColumn(table(), row, column) = value;
      const auto _index = index(row, column);
      emitDataChangedSignal(_index, _index);
    }

    /*! \brief Set data at \a index
     *
     * If \a role is other than Qt::EditRole,
     *  no data is set and this function returns false.
     *
     * If \a index is not valid,
     *  this method returns false.
     *
     * \pre If \a index is valid:
     *   - its row must be in valid range ( 0 <= index.row() < rowCount() )
     *   - its column must be in valid range ( 0 <= index.column() < columnCount() )
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override
    {
      if(!index.isValid()){
        return false;
      }
      if(role != Qt::EditRole){
        return false;
      }
      Q_ASSERT(index.row() >= 0);
      Q_ASSERT(index.row() < rowCount());
      Q_ASSERT(index.column() >= 0);
      Q_ASSERT(index.column() < columnCount());

      setValue( index.row(), index.column(), value.value<value_type>() );

      return true;
    }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_EDITABLE_STL_TABLE_MODEL_H
