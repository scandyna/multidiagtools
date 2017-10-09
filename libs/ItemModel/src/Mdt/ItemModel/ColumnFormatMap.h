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
#ifndef MDT_ITEM_MODEL_COLUMN_FORMAT_MAP_H
#define MDT_ITEM_MODEL_COLUMN_FORMAT_MAP_H

#include "RowColumnFormatMapBase.h"
#include "MdtItemModelExport.h"
#include <QVariant>
#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Stores formats for certain column in a item model
   */
  class MDT_ITEMMODEL_EXPORT ColumnFormatMap
  {
   public:

    /*! \brief Set format for given column
     *
     * \pre \a column must be >= 0
     */
    template<typename T>
    void setFormatForColumn(int column, const T & format)
    {
      Q_ASSERT(column >= 0);
      mBase.setFormatVariantForIndex(column, QVariant(format));
    }

    /*! \brief Clear format for given column
     *
     * Does nothing if no format was set for \a column .
     *
     * \pre \a column must be >= 0
     */
    void clearFormatForColumn(int column);

    /*! \brief Get format for given column
     *
     * Returns a QVariant with value of type passed in setFormatForColumn()
     *  if a format was set for \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a column must be >= 0
     */
    QVariant formatForColumn(int column) const
    {
      Q_ASSERT(column >= 0);
      return mBase.formatForIndex(column);
    }

   private:

    RowColumnFormatMapBase mBase;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_FORMAT_MAP_H
