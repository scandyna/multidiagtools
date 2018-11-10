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
using Mdt::Container::TableCacheTransaction;
using Mdt::Container::TableCacheRowTransaction;
using Mdt::Container::TableCacheRowTransactionList;
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
  if(mOperationMap.isTransactionPendingForRow(index.row())){
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

bool AbstractEditableCachedTableModel::submitChanges()
{
//   if(!updateModifiedRowsInBackend()){
//     return false;
//   }
  if(!addNewRecordsToBackend()){
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
  const RowList shiftedRows = mOperationMap.shiftRowsInMap(row, count);
  endInsertRows();

  if(!shiftedRows.isEmpty()){
    Q_ASSERT( (shiftedRows.smallestRow() - count) >= 0 );
    emit headerDataChanged(Qt::Vertical, shiftedRows.smallestRow()-count, shiftedRows.greatestRow());
  }
}

QVariant AbstractEditableCachedTableModel::verticalHeaderDisplayRoleData(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  if(mOperationMap.isTransactionFailedForRow(row)){
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

void AbstractEditableCachedTableModel::transactionSucceeded(const TableCacheTransaction & transaction, const VariantRecord record)
{
  Q_ASSERT(!transaction.isNull());
  Q_ASSERT(record.columnCount() == columnCount());

  const int row = mOperationMap.getRowForTransaction(transaction);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  mOperationMap.setTransatctionDoneForRow(row);
  fromBackendSetRecord(row, record);

  emit headerDataChanged(Qt::Vertical, row, row);
}

void AbstractEditableCachedTableModel::transactionFailed(const TableCacheTransaction & transaction, const Mdt::Error & error)
{
  Q_ASSERT(!transaction.isNull());

  const int row = mOperationMap.getRowForTransaction(transaction);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  mOperationMap.setTransatctionFailedForRow(row);

  emit headerDataChanged(Qt::Vertical, row, row);
}

bool AbstractEditableCachedTableModel::addRecordsToBackend(const TableCacheRowTransactionList & rowTransactions)
{
  for(const auto & rowTransaction : rowTransactions){
    if(!addRecordToBackend(rowTransaction)){
      return false;
    }
  }
  return true;
}

bool AbstractEditableCachedTableModel::updateRecordsInBackend(const TableCacheRowTransactionList & rowTransactions)
{
  for(const auto & rowTransaction : rowTransactions){
    if(!updateRecordInBackend(rowTransaction)){
      return false;
    }
  }
  return true;
}

bool AbstractEditableCachedTableModel::addNewRecordsToBackend()
{
  const TableCacheRowTransactionList rowTransactions = mOperationMap.getRowsToAddToBackend();

  mOperationMap.setTransactionsPending(rowTransactions);

  return addRecordsToBackend(rowTransactions);
}

bool AbstractEditableCachedTableModel::updateModifiedRowsInBackend()
{
}

// Mdt::Container::TableCacheOperation AbstractEditableCachedTableModel::operationAtRow(int row) const
// {
//   Q_ASSERT(row >= 0);
//   Q_ASSERT(row < rowCount());
// 
//   return mOperationMap.operationAtRow(row);
// }

}} // namespace Mdt{ namespace ItemModel{
