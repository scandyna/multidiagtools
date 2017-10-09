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
#ifndef MDT_ITEM_MODEL_FORMAT_MAP_H
#define MDT_ITEM_MODEL_FORMAT_MAP_H

#include "IndexFormatMap.h"
#include "RowFormatMap.h"
#include "ColumnFormatMap.h"
#include "MdtItemModelExport.h"
#include <array>

namespace Mdt{ namespace ItemModel{

  /*! \brief Format map priority
   */
  enum class FormatMapPriority
  {
    Index,
    Row,
    Column
  };

  /*! \brief Stores formats for certain row, column and index in a item model
   */
  class MDT_ITEMMODEL_EXPORT FormatMap
  {
   public:

    /*! \brief Construct a format map
     */
    explicit FormatMap();

    /*! \brief Set priority when format conflics
     *
     * Define which format should be choosen when
     *  many where set for a specific index.
     *
     * For example:
     * \code
     * setPriority({FormatMapPriority::Row, FormatMapPriority::Column, FormatMapPriority::Index});
     * \endcode
     *
     * Default is index, then row, then column.
     */
    void setPriority(const std::array<FormatMapPriority, 3> & priority);

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
      mIndexMap.setFormatForIndex(row, column, format);
    }

    /*! \brief Clear format for given row and column
     *
     * Does nothing if no format was set for \a row and \a column .
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearFormatForIndex(int row, int column);

    /*! \brief Set format for given row
     *
     * \pre \a row must be >= 0
     */
    template<typename T>
    void setFormatForRow(int row, const T & format)
    {
      Q_ASSERT(row >= 0);
      mRowMap.setFormatForRow(row, format);
    }

    /*! \brief Clear format for given row
     *
     * Does nothing if no format was set for \a row .
     *
     * \pre \a row must be >= 0
     */
    void clearFormatForRow(int row);

    /*! \brief Set format for given column
     *
     * \pre \a column must be >= 0
     */
    template<typename T>
    void setFormatForColumn(int column, const T & format)
    {
      Q_ASSERT(column >= 0);
      mColumnMap.setFormatForColumn(column, format);
    }

    /*! \brief Clear format for given column
     *
     * Does nothing if no format was set for \a column .
     *
     * \pre \a column must be >= 0
     */
    void clearFormatForColumn(int column);

    /*! \brief Get format for given row and column
     *
     * Returns a QVariant with value of type passed in
     *  setFormatForRow() , setFormatForColumn() or setFormatForIndex() ,
     *  if a format was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    QVariant formatForIndex(int row, int column) const;

   private:

    IndexFormatMap mIndexMap;
    RowFormatMap mRowMap;
    ColumnFormatMap mColumnMap;
    std::array<FormatMapPriority, 3> mPriority;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FORMAT_MAP_H
