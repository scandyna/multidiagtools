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
#include <algorithm>
#include <iterator>

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
  const auto previousMap = mMap;
  mMap.clear();
  auto it = list.cbegin();
  while(it != list.cend()){
    mMap.insert( it.key(), it.value() );
    ++it;
  }
  setColumnsNoLongerPartOfForeignKey(previousMap);
  updateColumnsToBeShownAndHidden();
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

void ForeignKeyColumnVisibilityMap::setForeignKeyHidden(const QString & foreignEntityName, bool hide)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  mMap[foreignEntityName].setForeignKeyHidden(hide);
  updateColumnsToBeShownAndHidden();
}

void ForeignKeyColumnVisibilityMap::setAllForeignKeysHidden(bool hide)
{
  for(auto & item : mMap){
    item.setForeignKeyHidden(hide);
  }
  updateColumnsToBeShownAndHidden();
}

bool ForeignKeyColumnVisibilityMap::isForeignKeyHidden(const QString & foreignEntityName) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  return mMap.value(foreignEntityName).isForeignKeyHidden();
}

bool ForeignKeyColumnVisibilityMap::isColumnVisible(int column) const
{
  for(const auto & item : mMap){
    if( item.foreignKeyContainsColumn(column) && !item.isForeignKeyHidden() ){
      return true;
    }
  }
  return false;
}

void ForeignKeyColumnVisibilityMap::updateColumnsToBeShownAndHidden()
{
  auto columnsToBeVisible = getColumnsToBeVisible();
  auto columnsToBeHidden = getColumnsToBeHidden();

  std::sort(columnsToBeVisible.begin(), columnsToBeVisible.end());
  std::sort(columnsToBeHidden.begin(), columnsToBeHidden.end());
  std::sort(mHiddenColumns.begin(), mHiddenColumns.end());
  // Columns to show: columns that must be visible and that are hidden + columns that are no longer part of a foreign key
  mColumnsToShow.clear();
  std::set_intersection( columnsToBeVisible.cbegin(), columnsToBeVisible.cend(), mHiddenColumns.cbegin(), mHiddenColumns.cend(), std::back_inserter(mColumnsToShow) );
  const auto copyPred = [this](int column){
    return !mColumnsToShow.contains(column);
  };
  std::copy_if(mColumnsNoLongerPartOfMap.cbegin(), mColumnsNoLongerPartOfMap.cend(), std::back_inserter(mColumnsToShow), copyPred);
  // Columns to hide: columns that must be hidden, and that are not actually hidden
  mColumnsToHide.clear();
  std::set_difference( columnsToBeHidden.cbegin(), columnsToBeHidden.cend(), mHiddenColumns.cbegin(), mHiddenColumns.cend(), std::back_inserter(mColumnsToHide) );
  // Hidden columns: the expected result, i.e. columns to be hidden
  mHiddenColumns.clear();
  std::copy(columnsToBeHidden.cbegin(), columnsToBeHidden.cend(), std::back_inserter(mHiddenColumns));
}

void ForeignKeyColumnVisibilityMap::setColumnsNoLongerPartOfForeignKey(const QHash<QString, ForeignKeyColumnVisibilityMapItem> & previousMap)
{
  ColumnList previousColumnList;
  ColumnList currentColumnList;

  for(const auto & item : previousMap){
    copyItemFkToList(item, previousColumnList);
  }
  for(const auto & item : mMap){
    copyItemFkToList(item, currentColumnList);
  }
  std::sort(previousColumnList.begin(), previousColumnList.end());
  std::sort(currentColumnList.begin(), currentColumnList.end());
  // Columns to show: columns that exists in previous list, but no longer exists in current list
  mColumnsNoLongerPartOfMap.clear();
  std::set_difference( previousColumnList.cbegin(), previousColumnList.cend(), currentColumnList.cbegin(), currentColumnList.cend(), std::back_inserter(mColumnsNoLongerPartOfMap) );
}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToShow() const
{
  mColumnsNoLongerPartOfMap.clear();
  return mColumnsToShow;
/*
  ColumnList list;
  auto columnsToBeVisible = getColumnsToBeVisible();

  std::sort(columnsToBeVisible.begin(), columnsToBeVisible.end());
  std::set_intersection( columnsToBeVisible.cbegin(), columnsToBeVisible.cend(), mHiddenColumns.cbegin(), mHiddenColumns.cend(), std::back_inserter(list) );

//   for(const auto & item : mMap){
//     if(!item.isForeignKeyHidden()){
//       copyItemFkToList(item, list);
//     }
//   }

  return list;*/
}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToHide() const
{
  return mColumnsToHide;

//   ColumnList list;
//   auto columnsToBeHidden = getColumnsToBeHidden();
// 
//   std::sort(columnsToBeHidden.begin(), columnsToBeHidden.end());
//   std::set_difference( columnsToBeHidden.cbegin(), columnsToBeHidden.cend(), mHiddenColumns.cbegin(), mHiddenColumns.cend(), std::back_inserter(list) );
//   //mHiddenColumns.clear();
// //   std::copy( list.cbegin(), list.cend(), std::back_inserter(mHiddenColumns) );
// 
// //   for(const auto & item : mMap){
// //     if(item.isForeignKeyHidden()){
// //       copyItemFkToList(item, list);
// //     }
// //   }
// 
//   return list;
}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToBeHidden() const
{
  ColumnList list;

  const auto copyPred = [&list, this](int column){
    return ( (std::find(list.cbegin(), list.cend(), column) == list.cend()) && !isColumnVisible(column)  );
  };
  for(const auto & item : mMap){
    if(item.isForeignKeyHidden()){
      const auto & fk = item.foreignKey();
      std::copy_if( fk.cbegin(), fk.cend(), std::back_inserter(list), copyPred );
//       copyItemFkToList(item, list, false);
    }
  }

  return list;
}

ColumnList ForeignKeyColumnVisibilityMap::getColumnsToBeVisible() const
{
  ColumnList list;

  const auto copyPred = [&list, this](int column){
    return ( (std::find(list.cbegin(), list.cend(), column) == list.cend()) && isColumnVisible(column)  );
  };
  for(const auto & item : mMap){
    if(!item.isForeignKeyHidden()){
      const auto & fk = item.foreignKey();
      std::copy_if( fk.cbegin(), fk.cend(), std::back_inserter(list), copyPred );
//       copyItemFkToList(item, list, true);
    }
  }

  return list;
}

void ForeignKeyColumnVisibilityMap::copyItemFkToList(const Mdt::ItemEditor::ForeignKeyColumnVisibilityMapItem & item, ColumnList & list)
{
  const auto pred = [&list](int column){
    return ( std::find(list.cbegin(), list.cend(), column) == list.cend() );
  };
  const auto & fk = item.foreignKey();
  std::copy_if( fk.cbegin(), fk.cend(), std::back_inserter(list), pred );
}

}} // namespace Mdt{ namespace ItemEditor{
