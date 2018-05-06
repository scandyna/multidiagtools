/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "TableCacheOperationMap.h"
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace Container{

void TableCacheOperationMap::clear()
{
  mMap.clear();
}

void TableCacheOperationMap::insertRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 0);

  int row = pos;
  for(int i = 0; i < count; ++i){
    mMap.emplace_back( TableCacheOperationIndex(row, TableCacheOperation::Insert) );
    ++row;
  }
}

TableCacheOperation TableCacheOperationMap::operationAtRow(int row) const
{
  Q_ASSERT(row >= 0);

  return findRow(row).operation();
}

RowList TableCacheOperationMap::getRowsToInsertIntoStorage() const
{
  RowList rowList;

  for(const auto & index : mMap){
    if(index.operation() == TableCacheOperation::Insert){
      rowList.append(index.row());
    }
  }

  return rowList;
}

void TableCacheOperationMap::commitChanges()
{
  for(auto & index : mMap){
    index.setOperation(TableCacheOperation::None);
  }
}

int TableCacheOperationMap::getFirstCommittedRow() const
{
  Q_ASSERT(!isEmpty());

  const auto pred = [](const TableCacheOperationIndex & a, const TableCacheOperationIndex & b){
    return a.row() < b.row();
  };
  const auto it = std::min_element(mMap.cbegin(), mMap.cend(), pred);
  Q_ASSERT(it != mMap.cend());

  return it->row();
}

int TableCacheOperationMap::getLastCommittedRow() const
{
  Q_ASSERT(!isEmpty());

  const auto pred = [](const TableCacheOperationIndex & a, const TableCacheOperationIndex & b){
    return a.row() < b.row();
  };
  const auto it = std::max_element(mMap.cbegin(), mMap.cend(), pred);
  Q_ASSERT(it != mMap.cend());

  return it->row();
}

TableCacheOperationIndex TableCacheOperationMap::findIndex(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  const auto pred = [row, column](const TableCacheOperationIndex & index){
    return (index.row() == row) && (index.column() == column);
  };
  const auto it = std::find_if( mMap.cbegin(), mMap.cend(), pred );
  if(it == mMap.cend()){
    return TableCacheOperationIndex();
  }

  return *it;
}

TableCacheOperationIndex TableCacheOperationMap::findRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto pred = [row](const TableCacheOperationIndex & index){
    return index.row() == row;
  };
  const auto it = std::find_if( mMap.cbegin(), mMap.cend(), pred );
  if(it == mMap.cend()){
    return TableCacheOperationIndex();
  }

  return *it;
}

}} // namespace Mdt{ namespace Container{
