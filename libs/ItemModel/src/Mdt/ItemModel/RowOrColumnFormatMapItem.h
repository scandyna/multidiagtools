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
#ifndef MDT_ITEM_MODEL_ROW_OR_COLUMN_FORMAT_MAP_ITEM_H
#define MDT_ITEM_MODEL_ROW_OR_COLUMN_FORMAT_MAP_ITEM_H

#include "MdtItemModelExport.h"
#include <QVariant>
#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Item of a RowFormatMap or ColumnFormatMap
   */
  class MDT_ITEMMODEL_EXPORT RowOrColumnFormatMapItem
  {
   public:

    /*! \brief Construct a item
     *
     * \pre \a index must be >= 0
     * \pre \a value must not be null
     */
    explicit RowOrColumnFormatMapItem(int index, const QVariant & value);

    /*! \brief Get index
     */
    int index() const
    {
      return mIndex;
    }

    /*! \brief Get value
     */
    QVariant value() const
    {
      return mValue;
    }

   private:

    int mIndex;
    QVariant mValue;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_OR_COLUMN_FORMAT_MAP_ITEM_H
