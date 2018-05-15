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

#include "RowResizableStlTableModelTemplate.h"
#include "Mdt/Container/StlContainerIteratorAdapter.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Resizable table model for STL compliant container
   *
   * \tparam Table Type of a STL compatible container.
   *   It must meet requirement for EditableStlTableModel,
   *    and additionaly provide:
   *     - A STL compatible %insert() method:
   *        \code
   *        iterator insert(iterator pos, size_type count, const T & value);
   *        \endcode
   *     - A STL compatible %erase() method:
   *        \code
   *        iterator erase(iterator first, iterator last);
   *        \endcode
   *
   * \tparam RecordAdapter Class template that meets the requirement
   *   of EditableStlTableModel, and that also provides following functions:
   *    - get a initialized record container :
   *     \code
   *     Container initializeContainer(int count, const value_type & value);
   *     \endcode
   */
  template<typename Table, typename RecordAdapter = Mdt::Container::StlContainerIteratorAdapter<typename Table::value_type> >
  class RowResizableStlTableModel : public RowResizableStlTableModelTemplate<Table, RecordAdapter>
  {
   public:

    using ParentClass = RowResizableStlTableModelTemplate<Table, RecordAdapter>;

    using record_type = typename ParentClass::record_type;
    using value_type = typename ParentClass::value_type;

    using ParentClass::columnCount;
    using ParentClass::isEmpty;

   protected:

    using ParentClass::beginResetModel;
    using ParentClass::endResetModel;
    using ParentClass::afterTableWasSet;

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

   private:

    void afterTableWasSet() override
    {
      mColumnCount = ParentClass::columnCount();
    }

    int mColumnCount = 0;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_RESIZABLE_STL_TABLE_MODEL_H
