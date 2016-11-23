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
#ifndef MDT_ITEM_EDITOR_FILTER_COLUMN_H
#define MDT_ITEM_EDITOR_FILTER_COLUMN_H

#include <QtGlobal>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Data container for FilterColumn
   */
  class FilterColumnData
  {
   public:

    /*! \brief Construct data
     *
     * \pre columnIndex must be >= 0
     */
    constexpr FilterColumnData(int columnIndex) noexcept
     : mColumnIndex(columnIndex)
    {
      Q_ASSERT(mColumnIndex >= 0);
    }

    /*! \brief Get column index
     */
    constexpr int columnIndex() const noexcept
    {
      return mColumnIndex;
    }

   private:

    int mColumnIndex;
  };

  /*! \brief FilterColumn is a (boost:proto) terminal that represents a column index in a item model, used for FilterExpression
   */
  class FilterColumn
  {
   public:

    

   private:

    
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_FILTER_COLUMN_H
