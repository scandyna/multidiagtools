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
#ifndef MDT_ITEM_MODEL_RELATION_COLUMN_PAIR_H
#define MDT_ITEM_MODEL_RELATION_COLUMN_PAIR_H

#include <QtGlobal>

namespace Mdt{ namespace ItemModel{

  /*! \brief Pair of parent model coulumn and child model column in a relation
   */
  class RelationColumnPair
  {
   public:

    /*! \brief Construct a column pair
     *
     * \pre parentModelColumn must be >= 0
     * \pre childModelColumn must be >= 0
     */
    explicit constexpr RelationColumnPair(int parentModelColumn, int childModelColumn) noexcept
     : mParentModelColumn(parentModelColumn),
       mChildModelColumn(childModelColumn)
    {
      Q_ASSERT(mParentModelColumn >= 0);
      Q_ASSERT(mChildModelColumn >= 0);
    }

    /*! \brief Get parent model column
     */
    constexpr int parentModelColumn() const noexcept
    {
      return mParentModelColumn;
    }

    /*! \brief Get child model column
     */
    constexpr int childModelColumn() const noexcept
    {
      return mChildModelColumn;
    }

   private:

    int mParentModelColumn;
    int mChildModelColumn;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_COLUMN_PAIR_H
