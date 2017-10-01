/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_H
#define MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_H

#include <Qt>
#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Hold attributes to sort strings
   */
  class ColumnSortStringAttributes
  {
   public:

    /*! \brief Construct null attributes
     */
    constexpr ColumnSortStringAttributes() noexcept
     : mColumn(-1),
       mCaseSensitivity(Qt::CaseInsensitive),
       mNumericMode(false)
    {
    }

    /*! \brief Constructor valid attributes
     *
     * \pre column must be >= 0
     */
    constexpr ColumnSortStringAttributes(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode) noexcept
     : mColumn(column),
       mCaseSensitivity(caseSensitivity),
       mNumericMode(numericMode)
    {
      Q_ASSERT(mColumn >= 0);
    }

    /*! \brief Get column
     */
    constexpr int column() const noexcept
    {
      return mColumn;
    }

    /*! \brief Get case sensitivity
     */
    constexpr Qt::CaseSensitivity caseSensitivity() const noexcept
    {
      return mCaseSensitivity;
    }

    /*! \brief Check if numerci mode in enabled
     */
    constexpr bool numericMode() const noexcept
    {
      return mNumericMode;
    }

    /*! \brief Check if attributes is null
     */
    constexpr bool isNull() const noexcept
    {
      return mColumn < 0;
    }

   private:

    int mColumn;
    Qt::CaseSensitivity mCaseSensitivity;
    bool mNumericMode;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_COLUMN_SORT_STRING_ATTRIBUTES_H
