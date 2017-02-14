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
#ifndef MDT_ITEM_MODEL_ROW_FORMAT_MAP_H
#define MDT_ITEM_MODEL_ROW_FORMAT_MAP_H

#include "RowColumnFormatMapBase.h"
#include <QVariant>
#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Stores formats for certain row in a item model
   */
  class RowFormatMap
  {
   public:

    /*! \brief Set format for given row
     *
     * \pre \a row must be >= 0
     */
    template<typename T>
    void setFormatForRow(int row, const T & format)
    {
      Q_ASSERT(row >= 0);
      mBase.setFormatVariantForIndex(row, QVariant(format));
    }

    /*! \brief Clear format for given row
     *
     * Does nothing if no format was set for \a row .
     *
     * \pre \a row must be >= 0
     */
    void clearFormatForRow(int row);

    /*! \brief Get format for given row
     *
     * Returns a QVariant with value of type passed in setFormatForRow()
     *  if a format was set for \a row,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be >= 0
     */
    QVariant formatForRow(int row) const
    {
      Q_ASSERT(row >= 0);
      return mBase.formatForIndex(row);
    }

   private:

    RowColumnFormatMapBase mBase;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ROW_FORMAT_MAP_H
