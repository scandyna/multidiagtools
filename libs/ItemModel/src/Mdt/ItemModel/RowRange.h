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
#ifndef MDT_ITEM_MODEL_ROW_RANGE_H
#define MDT_ITEM_MODEL_ROW_RANGE_H

// #include "Range.h"
#include "Mdt/IndexRange/RowRange.h"
#include "MdtItemModelExport.h"
#include <QModelIndex>

namespace Mdt{ namespace ItemModel{

  /*! \brief RowRange represents a range of rows in item models
   */
  class MDT_ITEMMODEL_EXPORT RowRange : public Mdt::IndexRange::RowRange
  {
   public:

//     /*! \brief Set first row
//      */
//     constexpr void setFirstRow(int row) noexcept
//     {
//       setFirst(row);
//     }

    /*! \brief Set first model index
     */
    constexpr void setFirstIndex(const QModelIndex & index) noexcept
    {
      setFirstRow(index.row());
    }

//     /*! \brief Get first row
//      *
//      * \warning The first row is returned as it was set.
//      *           The validity of this range should be checked
//      *           before using the returned value.
//      *
//      * \sa isValid()
//      */
//     constexpr int firstRow() const noexcept
//     {
//       return first();
//     }

//     /*! \brief Set last row
//      */
//     constexpr void setLastRow(int row) noexcept
//     {
//       setLast(row);
//     }

    /*! \brief Set last model index
     */
    constexpr void setLastIndex(const QModelIndex & index) noexcept
    {
      setLastRow(index.row());
    }

//     /*! \brief Get last row
//      *
//      * \warning The last row is returned as it was set.
//      *           The validity of this range should be checked
//      *           before using the returned value.
//      *
//      * \sa isValid()
//      */
//     constexpr int lastRow() const noexcept
//     {
//       return last();
//     }

//     /*! \brief Set row count
//      *
//      * \note This method must be called after setFirstRow()
//      */
//     constexpr void setRowCount(int count) noexcept
//     {
//       setCount(count);
//     }

//     /*! \brief Get row count
//      *
//      * Returns row count of this range if it is valid,
//      *  otherwise 0.
//      *
//      * \sa isValid()
//      */
//     constexpr int rowCount() const noexcept
//     {
//       return count();
//     }

//     /*! \brief Check if this range contains row
//      */
//     constexpr bool containsRow(int row) const noexcept
//     {
//       return contains(row);
//     }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_RANGE_H
