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
// #include "StlContainer.h"
#include <algorithm>
#include <iterator>
#include <functional>

// #include <QDebug>

namespace Mdt{ namespace Container{

void TableCacheOperationMap::clear()
{
  mMap.clear();
  mCommittedRows.clear();
}

void TableCacheOperationMap::shiftRowsForInsert(int row, int count)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);

  for(auto & index : mMap){
    if(index.row() >= row){
      index.shiftRow(count);
    }
  }
}

void TableCacheOperationMap::shiftRowsForRemove(int row, int count)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);

  const int firstRow = row + count;
  for(auto & index : mMap){
    if(index.row() >= firstRow){
      index.shiftRow(-count);
    }
  }
}

void TableCacheOperationMap::insertRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 0);

//   shiftRowsInMap(pos, count);
  shiftRowsForInsert(pos, count);
  updateOrAddOperationsForRows(pos, count, TableCacheOperation::Insert);
}

void TableCacheOperationMap::setRecordUpdated(int row)
{
  Q_ASSERT(row >= 0);

  setOperationAtRow(row, TableCacheOperation::Update);
}

void TableCacheOperationMap::removeRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 1);

  updateOrAddOperationsForRows(pos, count, TableCacheOperation::Delete);
}

void TableCacheOperationMap::cancelRemoveRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 1);

  Mdt::IndexRange::RowRange rowRange;
  rowRange.setFirstRow(pos);
  rowRange.setRowCount(count);

  const auto indexToRemovePredicate = [rowRange](const TableCacheOperationIndex & index){
    if( !rowRange.containsRow(index.row()) ){
      return false;
    }
    if(index.operation() == TableCacheOperation::Delete){
      return true;
    }
//     if(index.operation() == TableCacheOperation::InsertDelete){
//       return true;
//     }
    return false;
  };
  mMap.erase( std::remove_if(mMap.begin(), mMap.end(), indexToRemovePredicate), mMap.end() );

  for(auto & index : mMap){
    if( rowRange.containsRow(index.row()) ){
      if(index.operation() == TableCacheOperation::UpdateDelete){
        index.setOperation(TableCacheOperation::Update);
      }
      if(index.operation() == TableCacheOperation::InsertDelete){
        index.setOperation(TableCacheOperation::Insert);
      }
    }
  }
}

bool TableCacheOperationMap::isRowRemoved(int row) const noexcept
{
  Q_ASSERT(row >= 0);

  const auto it = findRowConstIterator(row);
  if(it == mMap.cend()){
    return false;
  }
  switch(it->operation()){
    case TableCacheOperation::Delete:
    case TableCacheOperation::InsertDelete:
    case TableCacheOperation::UpdateDelete:
      return true;
    default:
      break;
  }

  return false;
}

void TableCacheOperationMap::removeOperationAtRow(int row)
{
  Q_ASSERT(row >= 0);

  const auto it = findRowIterator(row);
  if(it != mMap.cend()){
    mMap.erase(it);
  }
}

RowList TableCacheOperationMap::shiftRowsInMap(int row, int count)
{
  Q_ASSERT(row >= 0);

  RowList rowList;

  for(auto & index : mMap){
    if(index.row() >= row){
      const int shiftedRow = index.row() + count;
      Q_ASSERT(shiftedRow >= 0);
      index.setRow(shiftedRow);
      rowList.append(shiftedRow);
    }
  }

  return rowList;
}

void TableCacheOperationMap::setOperationAtRow(int row, TableCacheOperation operation)
{
  Q_ASSERT(row >= 0);

  const auto it = findRowIterator(row);
  if(it == mMap.cend()){
    mMap.push_back( TableCacheOperationIndex(row, operation) );
  }else{
    it->setOperation( operationFromExisting(it->operation(), operation) );
  }
}

TableCacheOperation TableCacheOperationMap::operationAtRow(int row) const
{
  Q_ASSERT(row >= 0);

  return findRow(row).operation();
}

RowList TableCacheOperationMap::getRowsToInsertIntoStorage() const
{
  return getRowsForOperation(TableCacheOperation::Insert);
}

RowList TableCacheOperationMap::getRowsToUpdateInStorage() const
{
  return getRowsForOperation(TableCacheOperation::Update);
}

RowList TableCacheOperationMap::getRowsForUpdatedRecords() const
{
  return getRowsForOperation(TableCacheOperation::Insert, TableCacheOperation::Update);
}

RowList TableCacheOperationMap::getRowsToDeleteInStorage() const
{
  auto rowList = getRowsForOperation(TableCacheOperation::Delete);
  std::sort(rowList.begin(), rowList.end(), std::greater<>());

  return rowList;
}

RowList TableCacheOperationMap::getRowsToDeleteInCacheOnly() const
{
  auto rowList = getRowsForOperation(TableCacheOperation::InsertDelete);
  std::sort(rowList.begin(), rowList.end(), std::greater<>());

  return rowList;
}

RowList TableCacheOperationMap::getRowsToDeleteInCache() const
{
  auto rowList = getRowsForOperation(TableCacheOperation::Delete, TableCacheOperation::InsertDelete);
  std::sort(rowList.begin(), rowList.end(), std::greater<>());

  return rowList;
}

void TableCacheOperationMap::commitChanges()
{
  setCommittedRows();
  mMap.clear();
}

TableCacheOperation TableCacheOperationMap::removeOperationFromCurrent(TableCacheOperation currentOperation)
{
  switch(currentOperation){
    case TableCacheOperation::Insert:
      return TableCacheOperation::InsertDelete;
    case TableCacheOperation::Update:
      return TableCacheOperation::UpdateDelete;
  }

  return currentOperation;
}

TableCacheOperation TableCacheOperationMap::updateOperationFromCurrent(TableCacheOperation currentOperation)
{
  switch(currentOperation){
    case TableCacheOperation::None:
      return TableCacheOperation::Update;
    case TableCacheOperation::Update:
      return TableCacheOperation::Update;
    case TableCacheOperation::Insert:
      return TableCacheOperation::Insert;
    case TableCacheOperation::Delete:
      return TableCacheOperation::Update;
  }

  return currentOperation;
}

TableCacheOperation TableCacheOperationMap::operationFromExisting(TableCacheOperation existingOperation, TableCacheOperation operation) noexcept
{
  switch(operation){
    case TableCacheOperation::Delete:
      return removeOperationFromCurrent(existingOperation);
    case TableCacheOperation::Update:
      return updateOperationFromCurrent(existingOperation);
  }

  switch(existingOperation){
    case TableCacheOperation::None:
      return operation;
    case TableCacheOperation::Insert:
      switch(operation){
        ///case TableCacheOperation::None:
        case TableCacheOperation::Insert:
        case TableCacheOperation::Update:
          return TableCacheOperation::Insert;
        case TableCacheOperation::Delete:
          return TableCacheOperation::InsertDelete;
      }
      break;
    case TableCacheOperation::Update:
      switch(operation){
        ///case TableCacheOperation::None:
        ///case TableCacheOperation::Insert:
        case TableCacheOperation::Update:
          return TableCacheOperation::Update;
        case TableCacheOperation::Delete:
          return TableCacheOperation::UpdateDelete;
      }
      break;
    case TableCacheOperation::Delete:
      switch(operation){
        ///case TableCacheOperation::None:
        case TableCacheOperation::Insert:
          return TableCacheOperation::Delete;
        case TableCacheOperation::Update:
          return TableCacheOperation::Update;
        case TableCacheOperation::Delete:
          return TableCacheOperation::Delete;
      }
      break;
  }
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

template<typename Iterator>
Iterator findRowIteratorT(int row, Iterator first, Iterator last)
{
  Q_ASSERT(row >= 0);

  const auto pred = [row](const TableCacheOperationIndex & index){
    return index.row() == row;
  };

  return std::find_if( first, last, pred );
}

TableCacheOperationMap::iterator TableCacheOperationMap::findRowIterator(int row)
{
  Q_ASSERT(row >= 0);

  return findRowIteratorT( row, mMap.begin(), mMap.end() );
}

TableCacheOperationMap::const_iterator TableCacheOperationMap::findRowConstIterator(int row) const
{
  Q_ASSERT(row >= 0);

  return findRowIteratorT( row, mMap.cbegin(), mMap.cend() );
}

TableCacheOperationIndex TableCacheOperationMap::findRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto it = findRowConstIterator(row);
  if(it == mMap.cend()){
    return TableCacheOperationIndex();
  }

  return *it;
}

void TableCacheOperationMap::updateOrAddOperationsForRows(int row, int count, TableCacheOperation operation)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 0);

  for(int i = 0; i < count; ++i){
    setOperationAtRow(row, operation);
    ++row;
  }
}

TableCacheOperationIndex TableCacheOperationMap::operationIndexAtIfExists(int index) const
{
  Q_ASSERT(index >= 0);

  if(index < indexCount()){
    return mMap[index];
  }
  return TableCacheOperationIndex();
}

void TableCacheOperationMap::setCommittedRows()
{
  mCommittedRows.clear();
  if(isEmpty()){
    return;
  }

  RowList committedRows;
  for(const auto index : mMap){
    if(isCommittedRow(index)){
      Q_ASSERT(index.row() >= 0);
      committedRows.append(index.row());
    }
  }

  const auto firstLastIt = std::minmax_element(committedRows.cbegin(), committedRows.cend());
  if( (firstLastIt.first != committedRows.cend()) && (firstLastIt.second != committedRows.cend()) ){
    const auto firstIt = firstLastIt.first;
    const auto lastIt = firstLastIt.second;
    mCommittedRows.setFirstRow( *firstIt );
    mCommittedRows.setLastRow( *lastIt );
  }
}

bool TableCacheOperationMap::isCommittedRow(const TableCacheOperationIndex & index)
{
  return (index.operation() == TableCacheOperation::Insert) || (index.operation() == TableCacheOperation::Update);
//   return (index.operation() != TableCacheOperation::Delete) && (index.operation() != TableCacheOperation::InsertDelete);
}

RowList TableCacheOperationMap::getRowsForOperation(TableCacheOperation operation) const
{
  RowList rowList;

  for(const auto & index : mMap){
    if(index.operation() == operation){
      rowList.append(index.row());
    }
  }

  return rowList;
}

RowList TableCacheOperationMap::getRowsForOperation(TableCacheOperation operation1, TableCacheOperation operation2) const
{
  RowList rowList;

  for(const auto & index : mMap){
    if( (index.operation() == operation1) || (index.operation() == operation2) ){
      rowList.append(index.row());
    }
  }

  return rowList;
}

}} // namespace Mdt{ namespace Container{
