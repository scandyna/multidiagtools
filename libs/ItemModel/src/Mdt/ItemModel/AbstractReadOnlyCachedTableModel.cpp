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
#include "AbstractReadOnlyCachedTableModel.h"
#include "Mdt/Container/StlContainer.h"

using Mdt::IndexRange::RowRange;
using Mdt::Container::VariantRecord;
using Mdt::Container::RowList;
using Mdt::Container::TableCacheRowTask;
using Mdt::Container::TableCacheRowTaskList;
using Mdt::Container::TableCacheTask;


namespace Mdt{ namespace ItemModel{

AbstractReadOnlyCachedTableModel::AbstractReadOnlyCachedTableModel(QObject* parent)
 : BaseClass(parent)
{
}

void AbstractReadOnlyCachedTableModel::setCachedRowCountLimit(int limit)
{
  Q_ASSERT(limit > 0);

  mCachedRowCountLimit = limit;
}

Qt::ItemFlags AbstractReadOnlyCachedTableModel::flags(const QModelIndex & index) const
{
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  Q_ASSERT(index.row() < rowCount());
  if( isTaskPendingForRow(index.row()) ){
    return Qt::NoItemFlags;
  }
  return BaseClass::flags(index);
}

int AbstractReadOnlyCachedTableModel::rowCount(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return 0;
  }
  return mCache.size();
}

QVariant AbstractReadOnlyCachedTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole){
    Q_ASSERT(section >= 0);
    if(orientation == Qt::Horizontal){
      Q_ASSERT(section < columnCount());
      return horizontalHeaderDisplayRoleData(section);
    }
    if(orientation == Qt::Vertical){
      Q_ASSERT(section < rowCount());
      return verticalHeaderDisplayRoleData(section);
    }
  }

  return BaseClass::headerData(section, orientation, role);
}

QVariant AbstractReadOnlyCachedTableModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  Q_ASSERT(index.row() < rowCount());
  Q_ASSERT(index.column() < columnCount());

  if( (role == Qt::DisplayRole)||(role == Qt::EditRole) ){
    return mCache[index.row()].value(index.column());
  }

  return QVariant();
}

bool AbstractReadOnlyCachedTableModel::fetchAll()
{
  /*
   * We signal cache was reset while clearing the cache,
   * then, fromBackendAppendRecord() will signal new incomming rows.
   * This has the adventage to also work with async fetching.
   */
  beginResetModel();
  mCache.clear();
  endResetModel();
  
  /// \todo Have also to clear transactions + operations

  return fetchRecords( cachedRowCountLimit() );
}

bool AbstractReadOnlyCachedTableModel::fetchRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= rowCount());

  const TableCacheRowTask rowTask = beginRowTask(row);
  Q_ASSERT(!rowTask.isNull());
  Q_ASSERT(rowTask.row() == row);

  return fetchRecordFromBackend(rowTask);
}

void AbstractReadOnlyCachedTableModel::fromBackendAppendRecord(const VariantRecord & record)
{
  Q_ASSERT(rowCount() < cachedRowCountLimit());
  Q_ASSERT(record.columnCount() == columnCount());

  fromBackendInsertRecords(rowCount(), 1, record);
}

void AbstractReadOnlyCachedTableModel::fromBackendInsertRecords(int row, int count, const VariantRecord & record)
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
  insertRecordsToCache(rows, record);
  endInsertRows();
}

void AbstractReadOnlyCachedTableModel::insertRecordsToCache(const RowRange & rowRange, const VariantRecord & record)
{
  Q_ASSERT(rowRange.firstRow() >= 0);
  Q_ASSERT(!rowRange.isEmpty());
  Q_ASSERT( (rowRange.rowCount() + rowCount()) <= cachedRowCountLimit() );
  Q_ASSERT(record.columnCount() == columnCount());

  mTaskMap.shiftRowsForInsert(rowRange.firstRow(), rowRange.rowCount());
  insertToContainer(mCache, rowRange.firstRow(), rowRange.rowCount(), record);
}

void AbstractReadOnlyCachedTableModel::fromBackendRemoveRows(int row, int count)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);
  Q_ASSERT( (row+count) <= rowCount() );

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  beginRemoveRows(rows);
  removeRecordsFromCache(rows);
  endRemoveRows();
}

void AbstractReadOnlyCachedTableModel::removeRecordsFromCache(const RowRange & rowRange)
{
  Q_ASSERT(rowRange.firstRow() >= 0);
  Q_ASSERT(!rowRange.isEmpty());
  Q_ASSERT(rowRange.lastRow() <= rowCount());

  mTaskMap.shiftRowsForRemove(rowRange.firstRow(), rowRange.rowCount());
//   mTaskMap.shiftRows(rows.firstRow(), -rows.rowCount());
  removeFromContainer(mCache, rowRange.firstRow(), rowRange.rowCount());
}

QVariant AbstractReadOnlyCachedTableModel::horizontalHeaderDisplayRoleData(int column) const
{
  return BaseClass::headerData(column, Qt::Horizontal, Qt::DisplayRole);
}

QVariant AbstractReadOnlyCachedTableModel::verticalHeaderDisplayRoleData(int row) const
{
  return BaseClass::headerData(row, Qt::Vertical, Qt::DisplayRole);
}

void AbstractReadOnlyCachedTableModel::fromBackendSetData(int row, int column, const QVariant& data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  setDataToCache(row, column, data);

  const QModelIndex idx = index(row, column);
  emit dataChanged(idx, idx, {Qt::DisplayRole});
}

void AbstractReadOnlyCachedTableModel::setDataToCache(int row, int column, const QVariant& data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mCache[row].setValue(column, data);
}

void AbstractReadOnlyCachedTableModel::fromBackendSetRecord(int row, const VariantRecord & record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  mCache[row] = record;

  const QModelIndex leftIndex = index(row, 0);
  const QModelIndex rightIndex = index(row, columnCount()-1);
  emit dataChanged(leftIndex, rightIndex, {Qt::DisplayRole});
}

bool AbstractReadOnlyCachedTableModel::fetchRecordFromBackend(const TableCacheRowTask & rowTask)
{
  Q_ASSERT(rowTask.row() >= 0);
  Q_ASSERT(rowTask.row() < rowCount());

  return false;
}

TableCacheRowTask AbstractReadOnlyCachedTableModel::beginRowTask(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return mTaskMap.beginRowTask(row);
}

TableCacheRowTaskList AbstractReadOnlyCachedTableModel::beginRowTasks(const RowList & rows)
{
  return mTaskMap.beginRowTasks(rows);
}

void AbstractReadOnlyCachedTableModel::taskSucceeded(const TableCacheTask & task, const VariantRecord record)
{
  Q_ASSERT(!task.isNull());
  Q_ASSERT(record.columnCount() == columnCount());

  const int row = mTaskMap.getRowForTask(task);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  mTaskMap.setTaskDoneForRow(row);
  taskSucceededForRow(row);
  fromBackendSetRecord(row, record);

  emit headerDataChanged(Qt::Vertical, row, row);
}

void AbstractReadOnlyCachedTableModel::removeRecordTaskSucceeded(const TableCacheTask & task)
{
  Q_ASSERT(!task.isNull());

  const int row = mTaskMap.getRowForTask(task);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  mTaskMap.setTaskDoneForRow(row);
  taskSucceededForRow(row);
  fromBackendRemoveRows(row, 1);
}

void AbstractReadOnlyCachedTableModel::taskFailed(const TableCacheTask & task, const Mdt::Error & error)
{
  Q_ASSERT(!task.isNull());

  const int row = mTaskMap.getRowForTask(task);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  mTaskMap.setTaskFailedForRow(row);
  taskFailedForRow(row);

  emit headerDataChanged(Qt::Vertical, row, row);

  setLastError(error);
}

void AbstractReadOnlyCachedTableModel::beginInsertRows(const RowRange& rowRange)
{
  Q_ASSERT(rowRange.isValid());

  BaseClass::beginInsertRows(QModelIndex(), rowRange.firstRow(), rowRange.lastRow());
}

void AbstractReadOnlyCachedTableModel::beginRemoveRows(const Mdt::IndexRange::RowRange & rowRange)
{
  Q_ASSERT(rowRange.isValid());

  BaseClass::beginRemoveRows(QModelIndex(), rowRange.firstRow(), rowRange.lastRow());
}

void AbstractReadOnlyCachedTableModel::setLastError(const Mdt::Error & error)
{
  mLastError = error;
  emit errorOccured(error);
}

}} // namespace Mdt{ namespace ItemModel{
