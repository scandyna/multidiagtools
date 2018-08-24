/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "AbstractEditableCacheTableModel.h"
#include "Mdt/Container/TableCacheOperation.h"
#include <QLatin1String>

using Mdt::Container::TableCacheOperation;
using Mdt::Container::VariantRecord;

namespace Mdt{ namespace Entity{

AbstractEditableCacheTableModel::AbstractEditableCacheTableModel(QObject* parent)
 : ParentClass(parent)
{
}

Qt::ItemFlags AbstractEditableCacheTableModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  return ParentClass::flags(index) | Qt::ItemIsEditable;
}

void AbstractEditableCacheTableModel::setCache(AbstractEditableCache* cache)
{
  Q_ASSERT(cache != nullptr);

  beginResetModel();

  disconnectCommonSignalsAndSlots();
  disconnect(mOperationAtRowsChangedConnection);

  mCache = cache;

  connectCommonSignalsAndSlots(mCache, this);
  mOperationAtRowsChangedConnection =
    connect(mCache, &AbstractEditableCache::operationAtRowsChanged, this, &AbstractEditableCacheTableModel::onOperationAtRowsChanged);

  endResetModel();
}

int AbstractEditableCacheTableModel::rowCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  if(mCache.isNull()){
    return 0;
  }

  return mCache->rowCount();
}

int AbstractEditableCacheTableModel::columnCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  if(mCache.isNull()){
    return 0;
  }

  return mCache->columnCount();
}

QVariant AbstractEditableCacheTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( mCache.isNull()||(role != Qt::DisplayRole) ){
    return ParentClass::headerData(section, orientation, role);
  }
  Q_ASSERT(section >= 0);
  if(orientation == Qt::Horizontal){
    Q_ASSERT(section < columnCount());
    return mCache->horizontalHeaderName(section);
  }
  Q_ASSERT(orientation == Qt::Vertical);
  Q_ASSERT(section < rowCount());

  const auto operation = mCache->operationAtRow(section);
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

  return ParentClass::headerData(section, orientation, role);
}

QVariant AbstractEditableCacheTableModel::data(const QModelIndex& index, int role) const
{
  if(mCache.isNull()){
    return QVariant();
  }
  if(!index.isValid()){
    return QVariant();
  }
  Q_ASSERT(index.row() < rowCount());
  Q_ASSERT(index.column() < columnCount());

  if( (role == Qt::DisplayRole)||(role == Qt::EditRole) ){
    return mCache->data(index.row(), index.column());
  }

  return QVariant();
}

bool AbstractEditableCacheTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(role != Qt::EditRole){
    return false;
  }
  if(!index.isValid()){
    return false;
  }
  Q_ASSERT(index.row() < rowCount());
  Q_ASSERT(index.column() < columnCount());
  Q_ASSERT(mCache != nullptr);

  mCache->setData(index.row(), index.column(), value);

  return true;
}

bool AbstractEditableCacheTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if(parent.isValid()){
    return false;
  }
  if(mCache.isNull()){
    return false;
  }

  mCache->insertRecords(row, count, VariantRecord(columnCount()));

  return true;
}

void AbstractEditableCacheTableModel::onOperationAtRowsChanged(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow >= 0);
  Q_ASSERT(firstRow < rowCount());
  Q_ASSERT(lastRow >= 0);
  Q_ASSERT(lastRow < rowCount());

  emit headerDataChanged(Qt::Vertical, firstRow, lastRow);
}

}} // namespace Mdt{ namespace Entity{
