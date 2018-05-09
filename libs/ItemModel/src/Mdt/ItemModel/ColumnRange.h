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
#ifndef MDT_ITEM_MODEL_COLUMN_RANGE_H
#define MDT_ITEM_MODEL_COLUMN_RANGE_H

#include "Mdt/IndexRange/ColumnRange.h"
#include "MdtItemModelExport.h"
#include <QModelIndex>

namespace Mdt{ namespace ItemModel{

  /*! \brief ColumnRange represents a range of columns in item models
   */
  class MDT_ITEMMODEL_EXPORT ColumnRange : public Mdt::IndexRange::ColumnRange
  {
   public:

    /*! \brief Set first model index
     */
    constexpr void setFirstIndex(const QModelIndex & index) noexcept
    {
      setFirstColumn(index.column());
    }

    /*! \brief Set last model index
     */
    constexpr void setLastIndex(const QModelIndex & index) noexcept
    {
      setLastColumn(index.column());
    }
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_RANGE_H
