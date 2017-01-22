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
#include "RelationKey.h"
#include "PrimaryKey.h"
#include "ForeignKey.h"
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace ItemModel{

void RelationKey::addColumnPair(int parentModelColumn, int childModelColumn)
{
  Q_ASSERT(parentModelColumn >= 0);
  Q_ASSERT(childModelColumn >= 0);

  mColumnPairList.emplace_back(parentModelColumn, childModelColumn);
}

void RelationKey::setKey(const PrimaryKey & parentModelPk, const ForeignKey & childModelFk)
{
  Q_ASSERT(!parentModelPk.isNull());
  Q_ASSERT(!childModelFk.isNull());
  Q_ASSERT(parentModelPk.columnCount() == childModelFk.columnCount());

  clear();
  mColumnPairList.reserve(parentModelPk.columnCount());
  const auto op = [](int parentModelColumn, int childModelColumn){
    return RelationColumnPair(parentModelColumn, childModelColumn);
  };
  std::transform(parentModelPk.begin(), parentModelPk.end(), childModelFk.begin(), std::back_inserter(mColumnPairList), op);
}

bool RelationKey::containsColumnPair(int parentModelColumn, int childModelColumn) const
{
  const auto pred = [parentModelColumn, childModelColumn](const RelationColumnPair & pair){
    return ( pair.parentModelColumn() == parentModelColumn && pair.childModelColumn() == childModelColumn );
  };
  return ( std::find_if(mColumnPairList.cbegin(), mColumnPairList.cend(), pred) != mColumnPairList.cend() );
}

int RelationKey::greatestParentModelColumn() const
{
  const auto cmp = [](const RelationColumnPair & a, const RelationColumnPair & b){
    return (a.parentModelColumn() < b.parentModelColumn());
  };
  const auto it = std::max_element(mColumnPairList.cbegin(), mColumnPairList.cend(), cmp);
  if(it == mColumnPairList.cend()){
    return -1;
  }
  return it->parentModelColumn();
}

int RelationKey::greatestChildModelColumn() const
{
  const auto cmp = [](const RelationColumnPair & a, const RelationColumnPair & b){
    return (a.childModelColumn() < b.childModelColumn());
  };
  const auto it = std::max_element(mColumnPairList.cbegin(), mColumnPairList.cend(), cmp);
  if(it == mColumnPairList.cend()){
    return -1;
  }
  return it->childModelColumn();
}

void RelationKey::clear()
{
  mColumnPairList.clear();
}

}} // namespace Mdt{ namespace ItemModel{
