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
#ifndef MDT_ITEM_MODEL_RELATION_KEY_H
#define MDT_ITEM_MODEL_RELATION_KEY_H

#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace ItemModel{

  /*! \internal Item of a RelationKey
   */
  class RelationKeyItem
  {
   public:

    constexpr RelationKeyItem(int parentModelColumn, int childModelColumn) noexcept
     : mParentModelColumn(parentModelColumn),
       mChildModelColumn(childModelColumn)
    {
      Q_ASSERT(mParentModelColumn >= 0);
      Q_ASSERT(mChildModelColumn >= 0);
    }

    constexpr int parentModelColumn() const noexcept
    {
      return mParentModelColumn;
    }

    constexpr int childModelColumn() const noexcept
    {
      return mChildModelColumn;
    }

   private:

    int mParentModelColumn;
    int mChildModelColumn;
  };

  /*! \brief Relation map between a primary key in parent model and a foreign key in child model
   */
  class RelationKey
  {
   public:

    

   private:

    std::vector<RelationKeyItem> mItems;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_KEY_H
