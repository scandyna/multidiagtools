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
#ifndef MDT_ITEM_MODEL_INDEX_FORMAT_ITEM_H
#define MDT_ITEM_MODEL_INDEX_FORMAT_ITEM_H

#include "IndexFormatMapItem.h"
#include <QVariant>
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Stores format for a index in a item model
   */
  class IndexFormatMap
  {
   public:

    /*! \brief Set format for given row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    template<typename T>
    void setFormatForIndex(int row, int column, const T & format)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(column >= 0);
      setFormatVariantForIndex(row, column, QVariant(format));
    }

    /*! \brief Clear format for given row and column
     *
     * Does nothing if no format was set for \a row and \a column .
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearFormatForIndex(int row, int column);

    /*! \brief Get format for given row and column
     *
     * Returns a QVariant with value of type passed in setFormatForIndex()
     *  if a format was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    QVariant formatForIndex(int row, int column) const;

   private:

    void setFormatVariantForIndex(int row, int column, const QVariant & format);
    using iterator = std::vector<IndexFormatMapItem>::iterator;
    using const_iterator = std::vector<IndexFormatMapItem>::const_iterator;

    const_iterator iteratorForIndex(int row, int column) const;
    iterator iteratorForIndexW(int row, int column);

    std::vector<IndexFormatMapItem> mItems;
  };

}} //namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_INDEX_FORMAT_ITEM_H
