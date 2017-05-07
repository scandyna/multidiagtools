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
#include "ForeignKeyColumnVisibilityMap.h"

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

ForeignKeyColumnVisibilityMapItem::ForeignKeyColumnVisibilityMapItem(const ForeignKey& fk)
 : mFk(fk)
{
}

void ForeignKeyColumnVisibilityMapItem::setForeignKeyHidden(bool hide)
{
  mIsFkHidden = hide;
}

void ForeignKeyColumnVisibilityMap::setForeignKeyList(const ForeignKeyList & list)
{
  mMap.clear();
  auto it = list.cbegin();
  while(it != list.cend()){
    mMap.insert( it.key(), it.value() );
    ++it;
  }
}

ForeignKeyList ForeignKeyColumnVisibilityMap::getForeignKeyList() const
{
  ForeignKeyList list;

  auto it = mMap.cbegin();
  while(it != mMap.cend()){
    list.addForeignKey(it.key(), it.value().foreignKey());
    ++it;
  }

  return list;
}

void ForeignKeyColumnVisibilityMap::setAllForeignKeysHidden(bool hide)
{

}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToShow() const
{
  ColumnList list;

  

  return list;
}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToHide() const
{
  ColumnList list;

  

  return list;
}

}} // namespace Mdt{ namespace ItemEditor{
