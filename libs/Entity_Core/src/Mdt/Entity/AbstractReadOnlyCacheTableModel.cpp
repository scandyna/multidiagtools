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
#include "AbstractReadOnlyCacheTableModel.h"

namespace Mdt{ namespace Entity{

AbstractReadOnlyCacheTableModel::AbstractReadOnlyCacheTableModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void AbstractReadOnlyCacheTableModel::setCache(AbstractReadOnlyCache* cache)
{
  Q_ASSERT(cache != nullptr);

  beginResetModel();

  disconnect(mCacheAboutToBeResetConnection);
  disconnect(mCacheResetConnection);
  disconnect(mDataChangedConnection);

  mCache = cache;

  mCacheAboutToBeResetConnection =
    connect(mCache, &AbstractReadOnlyCache::cacheAboutToBeReset, this, &AbstractReadOnlyCacheTableModel::beginResetModel);
  mCacheResetConnection =
    connect(mCache, &AbstractReadOnlyCache::cacheReset, this, &AbstractReadOnlyCacheTableModel::endResetModel);
  mDataChangedConnection =
    connect(mCache, &AbstractReadOnlyCache::dataAtRowsChanged, this, &AbstractReadOnlyCacheTableModel::onDataAtRowsChanged);

  endResetModel();
}

int AbstractReadOnlyCacheTableModel::rowCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  if(mCache.isNull()){
    return 0;
  }

  return mCache->rowCount();
}

int AbstractReadOnlyCacheTableModel::columnCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  if(mCache.isNull()){
    return 0;
  }

  return mCache->columnCount();
}

QVariant AbstractReadOnlyCacheTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( (!mCache.isNull())&&(orientation == Qt::Horizontal)&&(role == Qt::DisplayRole) ){
    Q_ASSERT(section >= 0);
    Q_ASSERT(section < columnCount());
    return mCache->horizontalHeaderName(section);
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant AbstractReadOnlyCacheTableModel::data(const QModelIndex& index, int role) const
{
  if(mCache.isNull()){
    return QVariant();
  }
  if(!index.isValid()){
    return QVariant();
  }
  Q_ASSERT(index.row() < rowCount());
  Q_ASSERT(index.column() < columnCount());

  if(role == Qt::DisplayRole){
    return mCache->data(index.row(), index.column());
  }

  return QVariant();
}

// QVariant AbstractReadOnlyCacheTableModel::horizontalHeaderDisplayRoleData(int column) const
// {
//   return QAbstractTableModel::headerData(column, Qt::Horizontal, Qt::DisplayRole);
// }

void AbstractReadOnlyCacheTableModel::onDataAtRowsChanged(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow < rowCount());
  Q_ASSERT(lastRow < rowCount());

  const auto topLeft = index(firstRow, 0);
  const auto bottomRight = index(lastRow, columnCount()-1);
  Q_ASSERT(topLeft.isValid());
  Q_ASSERT(bottomRight.isValid());

  emit dataChanged(topLeft, bottomRight);
}

}} // namespace Mdt{ namespace Entity{
