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
#ifndef MDT_ITEM_MODEL_EDITABLE_STL_LIST_MODEL_H
#define MDT_ITEM_MODEL_EDITABLE_STL_LIST_MODEL_H

#include "ReadOnlyStlListModel.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Editable list model for STL compliant container
   *
   * \tparam Container Type of a STL compatible container.
   *   It must meet requirement for ReadOnlyStlListModel,
   *    and additionnaly provide:
   *     - The begin() method
   *     - The end() method
   */
  template<typename Container>
  class EditableStlListModel : public ReadOnlyStlListModel<Container>
  {
   public:

    using value_type = typename ReadOnlyStlListModel<Container>::value_type;

    using ReadOnlyStlListModel<Container>::rowCount;
    using ReadOnlyStlListModel<Container>::index;
    using ReadOnlyStlListModel<Container>::container;
    using ReadOnlyStlListModel<Container>::emitDataChangedSignal;

    /*! \brief Get flags for item at \a index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override
    {
      if(!index.isValid()){
        return ReadOnlyStlListModel<Container>::flags(index);
      }
      return ( ReadOnlyStlListModel<Container>::flags(index) | Qt::ItemIsEditable );
    }

    /*! \brief Set element at \a row
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() ).
     */
    void setValue(int row, const value_type & value)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      auto it = std::next(container().begin(), row);
      *it = value;
      const auto _index = index(row, 0);
      emitDataChangedSignal(_index, _index);
    }

    /*! \brief Set data at \a index
     *
     * If \a role is other than Qt::EditRole,
     *  no data is set and this function returns false.
     *
     * If \a index is not valid, or out of range,
     *  this method returns false.
     *
     * \pre If \a index is valid, it must be in a valid range:
     *   - its row must be in ( 0 <= index.row() < rowCount() )
     *   - its column must be 0
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
      Q_ASSERT(index.column() == 0);

      setValue( index.row(), value.value<value_type>() );

      return true;
    }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_EDITABLE_STL_LIST_MODEL_H
