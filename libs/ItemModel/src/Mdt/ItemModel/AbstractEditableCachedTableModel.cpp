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
#include "AbstractEditableCachedTableModel.h"
#include "Mdt/Container/RowList.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QLatin1String>

using Mdt::IndexRange::RowRange;
using Mdt::Container::RowList;
using Mdt::Container::TableCacheOperation;
using Mdt::Container::TableCacheTask;
using Mdt::Container::TableCacheRowTask;
using Mdt::Container::TableCacheRowTaskList;
using Mdt::Container::VariantRecord;

namespace Mdt{ namespace ItemModel{

AbstractEditableCachedTableModel::AbstractEditableCachedTableModel(QObject *parent)
 : BaseClass(parent)
{
}

Qt::ItemFlags AbstractEditableCachedTableModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  Q_ASSERT(index.row() < rowCount());
  if( isTaskPendingForRow(index.row()) ){
    return Qt::NoItemFlags;
  }
  if( mOperationMap.isRowRemoved(index.row()) ){
    return Qt::NoItemFlags;
  }
  return BaseClass::flags(index) | Qt::ItemIsEditable;
}

bool AbstractEditableCachedTableModel::setData(const QModelIndex& index, const QVariant & value, int role)
{
  if(role != Qt::EditRole){
    return false;
  }
  if(!index.isValid()){
    return false;
  }
  Q_ASSERT(index.row() < rowCount());
  Q_ASSERT(index.column() < columnCount());

  setDataToCache(index.row(), index.column(), value);
  mOperationMap.setOperationAtRow(index.row(), TableCacheOperation::Update);

  emit dataChanged(index, index, {Qt::DisplayRole});
  emit headerDataChanged(Qt::Vertical, index.row(), index.row());

  return true;
}

bool AbstractEditableCachedTableModel::insertRows(int row, int count, const QModelIndex & parent)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= rowCount());
  Q_ASSERT(count >= 1);
  Q_ASSERT( (rowCount()+count) <= cachedRowCountLimit() );

  if(parent.isValid()){
    return false;
  }
  /// \todo Should be a method: canAddRows(int count)
  if( (rowCount()+count) > cachedRowCountLimit() ){
    return false;
  }

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);

  beginInsertRows(rows);
  insertRecordsToCache(row, count, VariantRecord(columnCount()));
  mOperationMap.insertRecords(row, count);
  endInsertRows();

  return true;
}

bool AbstractEditableCachedTableModel::removeRows(int row, int count, const QModelIndex & parent)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);
  Q_ASSERT( (row+count) <= rowCount() );

  if(parent.isValid()){
    return false;
  }
  mOperationMap.removeRecords(row, count);

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  emit headerDataChanged(Qt::Vertical, rows.firstRow(), rows.lastRow());

  return true;
}

bool AbstractEditableCachedTableModel::submitChanges()
{
  if(!updateModifiedRowsInBackend()){
    return false;
  }
  if(!addNewRecordsToBackend()){
    return false;
  }
  if(!removeRemovedRowsFromBackend()){
    return false;
  }
//   removeRowsToDeleteFromCacheOnly();
//   if(!removeRowsToDeleteFromBackend()){
//     return false;
//   }
  return true;
}

void AbstractEditableCachedTableModel::fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= rowCount());
  Q_ASSERT(count >= 1);
  Q_ASSERT( (rowCount()+count) <= cachedRowCountLimit() );
  Q_ASSERT(record.columnCount() == columnCount());

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  beginInsertRows(rows);
  insertRecordsToCache(row, count, record);
  /*const RowList shiftedRows = */ mOperationMap.shiftRowsInMap(row, count);
  endInsertRows();

//   if(!shiftedRows.isEmpty()){
//     Q_ASSERT( (shiftedRows.smallestRow() - count) >= 0 );
// //     emit headerDataChanged(Qt::Vertical, shiftedRows.smallestRow()-count, shiftedRows.greatestRow());
//   }
}

QVariant AbstractEditableCachedTableModel::verticalHeaderDisplayRoleData(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  if(isTaskFailedForRow(row)){
    return QLatin1String("!");
  }

  const TableCacheOperation operation = mOperationMap.operationAtRow(row);
  switch(operation){
    case TableCacheOperation::Insert:
      return QLatin1String("*");
    case TableCacheOperation::Update:
      return QLatin1String("e");
    case TableCacheOperation::Delete:
    case TableCacheOperation::InsertDelete:
      return QLatin1String("x");
    case TableCacheOperation::None:
      break;
  }

  return BaseClass::verticalHeaderDisplayRoleData(row);
}

void AbstractEditableCachedTableModel::taskSucceededForRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  mOperationMap.removeOperationAtRow(row);
}

bool AbstractEditableCachedTableModel::addRecordsToBackend(const TableCacheRowTaskList & rowTasks)
{
  for(const auto & rowTask : rowTasks){
    if(!addRecordToBackend(rowTask)){
      return false;
    }
  }
  return true;
}

bool AbstractEditableCachedTableModel::updateRecordsInBackend(const TableCacheRowTaskList & rowTasks)
{
  for(const auto & rowTask : rowTasks){
    if(!updateRecordInBackend(rowTask)){
      return false;
    }
  }
  return true;
}

bool AbstractEditableCachedTableModel::removeRecordsFromBackend(const TableCacheRowTaskList & rowTasks)
{
  for(const auto & rowTask : rowTasks){
    if(!removeRecordFromBackend(rowTask)){
      return false;
    }
  }
  return true;
}

bool AbstractEditableCachedTableModel::addNewRecordsToBackend()
{
  const RowList rows = mOperationMap.getRowsToInsertIntoStorage();
  const TableCacheRowTaskList rowTasks = beginRowTasks(rows);

  return addRecordsToBackend(rowTasks);
}

bool AbstractEditableCachedTableModel::updateModifiedRowsInBackend()
{
  const RowList rows = mOperationMap.getRowsToUpdateInStorage();
  const TableCacheRowTaskList rowTasks = beginRowTasks(rows);

  return updateRecordsInBackend(rowTasks);
}

bool AbstractEditableCachedTableModel::removeRemovedRowsFromBackend()
{
  const RowList rows = mOperationMap.getRowsToDeleteInStorage();
  const TableCacheRowTaskList rowTasks = beginRowTasks(rows);

  return removeRecordsFromBackend(rowTasks);
}

// Mdt::Container::TableCacheOperation AbstractEditableCachedTableModel::operationAtRow(int row) const
// {
//   Q_ASSERT(row >= 0);
//   Q_ASSERT(row < rowCount());
// 
//   return mOperationMap.operationAtRow(row);
// }

}} // namespace Mdt{ namespace ItemModel{
