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
  mCommittedRows.clear();
}

void TableCacheOperationMap::insertRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 0);

  updateOrAddOperationsForRows(pos, count, TableCacheOperation::Insert);
//   int row = pos;
//   for(int i = 0; i < count; ++i){
//     mMap.emplace_back( TableCacheOperationIndex(row, TableCacheOperation::Insert) );
//     ++row;
//   }
}

void TableCacheOperationMap::removeRecords(int pos, int count)
{
  Q_ASSERT(pos >= 0);
  Q_ASSERT(count >= 0);

  updateOrAddOperationsForRows(pos, count, TableCacheOperation::Delete);
//   int row = pos;
//   for(int i = 0; i < count; ++i){
//     auto operationIndex = operationIndexAtIfExists(i);
//     if(operationIndex.isNull()){
//       mMap.emplace_back( TableCacheOperationIndex(row, TableCacheOperation::Delete) );
//     }else{
//       operationIndex.setOperation( operationFromExisting(operationIndex.operation(), TableCacheOperation::Delete) );
//     }
//     ++row;
//   }
}

void TableCacheOperationMap::setOperationAtRow(int row, TableCacheOperation operation)
{
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

RowList TableCacheOperationMap::getRowsToDeleteInStorage() const
{
  return getRowsForOperation(TableCacheOperation::Delete);
}

void TableCacheOperationMap::commitChanges()
{
  setCommittedRows();
  mMap.clear();
//   for(auto & index : mMap){
//     index.setOperation(TableCacheOperation::None);
//   }
}

TableCacheOperation TableCacheOperationMap::operationFromExisting(TableCacheOperation existingOperation, TableCacheOperation operation)
{
  switch(existingOperation){
    case TableCacheOperation::None:
      return operation;
    case TableCacheOperation::Insert:
      switch(operation){
        ///case TableCacheOperation::None:
        case TableCacheOperation::Insert:
        case TableCacheOperation::Update:
          return TableCacheOperation::Insert;
        ///case TableCacheOperation::Delete:
      }
      break;
    case TableCacheOperation::Update:
      switch(operation){
        ///case TableCacheOperation::None:
        ///case TableCacheOperation::Insert:
        case TableCacheOperation::Update:
          return TableCacheOperation::Update;
        case TableCacheOperation::Delete:
          return TableCacheOperation::Delete;
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
  mCommittedRows.setFirstRow( getFirstCommittedRow() );
  mCommittedRows.setLastRow( getLastCommittedRow() );
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

}} // namespace Mdt{ namespace Container{
