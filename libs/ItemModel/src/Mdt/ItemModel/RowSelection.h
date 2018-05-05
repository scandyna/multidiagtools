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
#ifndef MDT_ITEM_MODEL_ROW_SELECTION_H
#define MDT_ITEM_MODEL_ROW_SELECTION_H

#include "RowRange.h"
#include "RowRangeList.h"
#include "MdtItemModelExport.h"
#include <QtGlobal>

class QItemSelectionModel;

namespace Mdt{ namespace ItemModel{

  /*! \brief Selection of rows in a item model
   *
   * \code
   * const auto selection = RowSelection::fromSelectionModel(view->selectionModel());
   * for(const auto & rowRange : selection){
   *   model->removeRows(rowRange.firstRow(), rowRange.rowCount());
   * }
   * \endcode
   */
  class MDT_ITEMMODEL_EXPORT RowSelection
  {
   public:

    /*! \brief STL style const iterator
     */
    using const_iterator = RowRangeList::const_iterator;

    /*! \brief Construct a empty row selection
     */
    RowSelection() = default;

    /*! \brief Copy construct a row selection from \a other
     */
    RowSelection(const RowSelection & other) = default;

    /*! \brief Copy assign \a other to this row selection
     */
    RowSelection & operator=(const RowSelection & other) = default;

    /*! \brief Move construct a row selection from \a other
     */
    RowSelection(RowSelection && other) = default;

    /*! \brief Move assign \a other to this row selection
     */
    RowSelection & operator=(RowSelection && other) = default;

    /*! \brief Check if this row selection is empty
     */
    bool isEmpty() const
    {
      return mRangeList.empty();
    }

    /*! \brief Get count of ranges in this row selection
     */
    int rangeCount() const
    {
      return mRangeList.size();
    }

    /*! \brief Add \a range to the end of this row selection
     *
     * \pre \a range must be valid
     */
    void appendRange(const RowRange & range);

    /*! \brief Get the range at \a index
     *
     * \pre \a index must be in valid range ( 0 >= \a index < rangeCount() ).
     */
    const RowRange & rangeAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < rangeCount());
      return mRangeList[index];
    }

    /*! \brief Get a const iterator to the first range in this row selection
     */
    const_iterator cbegin() const
    {
      return mRangeList.cbegin();
    }

    /*! \brief Get a const iterator past the end in this row selection
     */
    const_iterator cend() const
    {
      return mRangeList.cend();
    }

    /*! \brief Get a const iterator to the first range in this row selection
     */
    const_iterator begin() const
    {
      return cbegin();
    }

    /*! \brief Get a const iterator past the end in this row selection
     */
    const_iterator end() const
    {
      return cend();
    }

    /*! \brief Check if \a range is included in one of the existing ranges in this row selection
     *
     * \pre \a range must be valid
     */
    bool isRangeIncluded(const RowRange & range) const;

    /*! \brief Get a row selection from \a selectionModel
     *
     * \note If \a selectionModel is a null pointer, a empty range is returned.
     */
    static RowSelection fromSelectionModel(const QItemSelectionModel *selectionModel);

   private:

    void sortRangeList();

    RowRangeList mRangeList;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_SELECTION_H
