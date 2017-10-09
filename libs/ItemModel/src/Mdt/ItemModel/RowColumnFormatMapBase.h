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
#ifndef MDT_ITEM_MODEL_ROW_COLUMN_FORMAT_MAP_BASE_H
#define MDT_ITEM_MODEL_ROW_COLUMN_FORMAT_MAP_BASE_H

#include "RowOrColumnFormatMapItem.h"
#include "MdtItemModelExport.h"
#include <QVariant>
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Base class for RowFormatMap and ColumnFormatMap
   */
  class MDT_ITEMMODEL_EXPORT RowColumnFormatMapBase
  {
   public:

    /*! \brief Set a format for index
     *
     * \pre \a index must be >= 0
     */
    void setFormatVariantForIndex(int index, const QVariant & format);

    /*! \brief Clear format for given index
     *
     * Does nothing if no format was set for \a index .
     *
     * \pre \a index must be >= 0
     */
    void clearFormatForIndex(int index);

    /*! \brief Get format for given index
     *
     * Returns a QVariant with value of type passed in setFormatVariantForIndex()
     *  if a format was set for \a index,
     *  otherwise a null QVariant.
     *
     * \pre \a index must be >= 0
     */
    QVariant formatForIndex(int index) const;

   private:

    using iterator = std::vector<RowOrColumnFormatMapItem>::iterator;
    using const_iterator = std::vector<RowOrColumnFormatMapItem>::const_iterator;

    const_iterator iteratorForIndex(int column) const;
    iterator iteratorForIndexW(int column);

//     int itemIndexForColumn(int column) const;
  
    std::vector<RowOrColumnFormatMapItem> mItems;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_COLUMN_FORMAT_MAP_BASE_H
