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
#include "StlContainerIteratorAdapter.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Editable table model for STL compliant container
   *
   * \tparam Table Type of a STL compatible container.
   *   It must meet requirements for ReadOnlyStlTableModel,
   *    and additionaly provide:
   *     - The begin() method
   *     - The end() method
   *
   * \tparam RecordAdapter Class template that meets the requirement
   *   of ReadOnlyStlTableModel, and that also provides following functions:
   *    - set a value at a index in the record container :
   *     \code
   *     void setValueAtIndex(Container & container, int index, const value_type & value);
   *     \endcode
   */
  template<typename Table, typename RecordAdapter = StlContainerIteratorAdapter<typename Table::value_type> >
  class EditableStlTableModel : public ReadOnlyStlTableModel<Table, RecordAdapter>
  {
   public:

    using ParentClass = ReadOnlyStlTableModel<Table, RecordAdapter>;

    using record_type = typename ParentClass::record_type;
    using value_type = typename ParentClass::value_type;

    using ParentClass::ParentClass;
    using ParentClass::rowCount;
    using ParentClass::columnCount;
    using ParentClass::index;
    using ParentClass::table;

   protected:

    using ParentClass::recordAdapter;
    using ParentClass::emitDataChangedSignal;

   public:

    /*! \brief Get flags for item at \a index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override
    {
      if(!index.isValid()){
        return ParentClass::flags(index);
      }
      return ( ParentClass::flags(index) | Qt::ItemIsEditable );
    }

    /*! \brief Access record at \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    record_type & record(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return *std::next(table().begin(), row);
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

      recordAdapter().setValueAtIndex( record(row), column, value );
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
