/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ShiftProxyModel.h"
#include <QModelIndex>

#include <QDebug>

namespace mdt{

ShiftProxyModel::ShiftProxyModel(QObject *parent)
 : QAbstractProxyModel(parent),
   pvColumnOffset(0),
   pvRowOffset(0)
{
}

void ShiftProxyModel::setRowOffset(int offset)
{
  pvRowOffset = offset;
}

QModelIndex ShiftProxyModel::index(int row, int column, const QModelIndex & parent) const
{
  Q_ASSERT(sourceModel() != nullptr);

  // Currently, we not support hierarchical models
  if(parent.isValid()){
    return QModelIndex();
  }
  const auto sourceIndex = sourceModel()->index(row, column);

  qDebug() << "PM::index - row: " << row << ", col: " << column;
  
  ///return mapFromSource(sourceIndex);
  return createIndex(row, column, sourceIndex.internalPointer());
}

QModelIndex ShiftProxyModel::parent(const QModelIndex & /*child*/) const
{
  // Currently, we not support hierarchical models
  return QModelIndex();
}

int ShiftProxyModel::rowCount(const QModelIndex & parent) const
{
  if(sourceModel() == nullptr){
    return 0;
  }
  return sourceModel()->rowCount(parent) + pvRowOffset;
}

int ShiftProxyModel::columnCount(const QModelIndex & parent) const
{
  if(sourceModel() == nullptr){
    return 0;
  }
  return sourceModel()->columnCount(parent);
}

QModelIndex ShiftProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
{
  if( (!sourceIndex.isValid()) || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
  Q_ASSERT(sourceIndex.model() == sourceModel());
  
  qDebug() << "PM::mapFromSource - sourceIndex: " << sourceIndex;
  qDebug() << "PM::mapFromSource - return: " << createIndex(sourceIndex.row() + pvRowOffset, sourceIndex.column() + pvColumnOffset, sourceIndex.internalPointer());

  return createIndex(sourceIndex.row() + pvRowOffset, sourceIndex.column() + pvColumnOffset, sourceIndex.internalPointer());
}

QModelIndex ShiftProxyModel::mapToSource(const QModelIndex & proxyIndex) const
{
  if( (!proxyIndex.isValid()) || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
  Q_ASSERT(proxyIndex.model() == this);

  qDebug() << "PM::mapToSource - proxyIndex: " << proxyIndex;
  qDebug() << "PM::mapToSource - return: " << sourceModel()->index(proxyIndex.row() - pvRowOffset, proxyIndex.column() - pvColumnOffset);
  
  return sourceModel()->index(proxyIndex.row() - pvRowOffset, proxyIndex.column() - pvColumnOffset);
}

} // namespace mdt{
