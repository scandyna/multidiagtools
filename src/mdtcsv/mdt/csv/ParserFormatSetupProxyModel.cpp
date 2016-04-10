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
#include "ParserFormatSetupProxyModel.h"
#include "FieldType.h"
#include <QItemSelection>
#include <QBrush>
#include <QColor>

// #include <QDebug>

namespace mdt{ namespace csv{

ParserFormatSetupProxyModel::ParserFormatSetupProxyModel(QObject *parent)
 : QAbstractProxyModel(parent)
{
}

void ParserFormatSetupProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
  Q_ASSERT(newSourceModel != nullptr);

  beginResetModel();

  if(sourceModel() != nullptr){
    disconnect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &ParserFormatSetupProxyModel::onSourceModelAboutToBeReset);
    disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &ParserFormatSetupProxyModel::onSourceModelReset);
    disconnect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this, &ParserFormatSetupProxyModel::onSourceColumnsAboutToBeInserted);
    disconnect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this, &ParserFormatSetupProxyModel::onSourceColumnsAboutToBeRemoved);
    disconnect(sourceModel(), &QAbstractItemModel::columnsInserted, this, &ParserFormatSetupProxyModel::onSourceColumnsInserted);
    disconnect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, &ParserFormatSetupProxyModel::onSourceColumnsRemoved);
    disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &ParserFormatSetupProxyModel::onSourceDataChanged);
    disconnect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &ParserFormatSetupProxyModel::onSourceHeaderDataChanged);
    disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, &ParserFormatSetupProxyModel::onSourceRowsAboutToBeInserted);
    disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &ParserFormatSetupProxyModel::onSourceRowsAboutToBeRemoved);
    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &ParserFormatSetupProxyModel::onSourceRowsInserted);
    disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &ParserFormatSetupProxyModel::onSourceRowsRemoved);
  }

  QAbstractProxyModel::setSourceModel(newSourceModel);

  if(sourceModel() != nullptr){
    connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &ParserFormatSetupProxyModel::onSourceModelAboutToBeReset);
    connect(sourceModel(), &QAbstractItemModel::modelReset, this, &ParserFormatSetupProxyModel::onSourceModelReset);
    connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this, &ParserFormatSetupProxyModel::onSourceColumnsAboutToBeInserted);
    connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this, &ParserFormatSetupProxyModel::onSourceColumnsAboutToBeRemoved);
    connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, &ParserFormatSetupProxyModel::onSourceColumnsInserted);
    connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, &ParserFormatSetupProxyModel::onSourceColumnsRemoved);
    connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &ParserFormatSetupProxyModel::onSourceDataChanged);
    connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &ParserFormatSetupProxyModel::onSourceHeaderDataChanged);
    connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, &ParserFormatSetupProxyModel::onSourceRowsAboutToBeInserted);
    connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &ParserFormatSetupProxyModel::onSourceRowsAboutToBeRemoved);
    connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &ParserFormatSetupProxyModel::onSourceRowsInserted);
    connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &ParserFormatSetupProxyModel::onSourceRowsRemoved);
  }

  endResetModel();
}

QModelIndex ParserFormatSetupProxyModel::index(int row, int column, const QModelIndex & parent) const
{
  if( parent.isValid() || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
  return createIndex(row, column);
}

QModelIndex ParserFormatSetupProxyModel::parent(const QModelIndex & /*index*/) const
{
  // Currently, we not support hierarchical models
  return QModelIndex();
}


int ParserFormatSetupProxyModel::rowCount(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return 0;
  }
  if(sourceModel() == nullptr){
    return 1;
  }
  return sourceModel()->rowCount(parent) + 1;
}

int ParserFormatSetupProxyModel::columnCount(const QModelIndex & parent) const
{
  if( parent.isValid() || (sourceModel() == nullptr) ){
    return 0;
  }
  return sourceModel()->columnCount(parent);
}

QVariant ParserFormatSetupProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( (section == 0) && (orientation == Qt::Vertical) && (role == Qt::DisplayRole) ){
    return tr("Format");
  }
  if(sourceModel() == nullptr){
    return QVariant();
  }
  if(orientation == Qt::Vertical){
    return sourceModel()->headerData(section - 1, orientation, role);
  }
  return sourceModel()->headerData(section, orientation, role);
}

QVariant ParserFormatSetupProxyModel::data(const QModelIndex & proxyIndex, int role) const
{
  if(!proxyIndex.isValid()){
    return QVariant();
  }
  if(proxyIndex.row() == 0){
    return fieldFormatData(proxyIndex.column(), role);
  }
  if(sourceModel() == nullptr){
    return QVariant();
  }
  const auto idx = index(proxyIndex.row() - 1, proxyIndex.column());
  if(!idx.isValid()){
    return QVariant();
  }
  if(role == Qt::DisplayRole){
    auto value = sourceModel()->data(mapToSource(idx), role);
    if(!pvRecordFormatter.formatValue(idx.column(), value)){
      /// \todo Error handling
    }
    return value;
  }
  return sourceModel()->data(mapToSource(idx), role);
}

bool ParserFormatSetupProxyModel::setData(const QModelIndex & proxyIndex, const QVariant & value, int role)
{
  if(!proxyIndex.isValid()){
    return false;
  }
  if(proxyIndex.row() == 0){
    if(role != Qt::EditRole){
      return false;
    }
    setFieldType(proxyIndex.column(), static_cast<QMetaType::Type>(value.toInt()));
    return true;
  }
  if(sourceModel() == nullptr){
    return false;
  }
  const auto idx = index(proxyIndex.row() - 1, proxyIndex.column());
  if(!idx.isValid()){
    return false;
  }

  return sourceModel()->setData(idx, value, role);
}

Qt::ItemFlags ParserFormatSetupProxyModel::flags(const QModelIndex & proxyIndex) const
{
  if(proxyIndex.row() == 0){
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
  }
  if(sourceModel() == nullptr){
    return Qt::NoItemFlags;
  }
  const auto idx = index(proxyIndex.row() - 1, proxyIndex.column());

  return sourceModel()->flags(mapToSource(idx));
}

QModelIndex ParserFormatSetupProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
{
  if( (!sourceIndex.isValid()) || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
  Q_ASSERT(sourceIndex.model() == sourceModel());

  return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex ParserFormatSetupProxyModel::mapToSource(const QModelIndex & proxyIndex) const
{
  if( (!proxyIndex.isValid()) || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
  Q_ASSERT(proxyIndex.model() == this);

  return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

void ParserFormatSetupProxyModel::onSourceModelAboutToBeReset()
{
  beginResetModel();
}

void ParserFormatSetupProxyModel::onSourceModelReset()
{
  pvRecordFormatter.setFieldCount(columnCount(), QMetaType::QString);
  endResetModel();
}

void ParserFormatSetupProxyModel::onSourceColumnsAboutToBeInserted(const QModelIndex & parent, int first, int last)
{
  Q_ASSERT( (!parent.isValid()) || (parent.model() == sourceModel()) );
  beginInsertColumns(mapFromSource(parent), first, last);
}

void ParserFormatSetupProxyModel::onSourceColumnsAboutToBeRemoved(const QModelIndex & parent, int first, int last)
{
  Q_ASSERT( (!parent.isValid()) || (parent.model() == sourceModel()) );
  beginRemoveColumns(mapFromSource(parent), first, last);
}

void ParserFormatSetupProxyModel::onSourceColumnsInserted(const QModelIndex & /*parent*/, int /*first*/, int /*last*/)
{
  endInsertColumns();
}

void ParserFormatSetupProxyModel::onSourceColumnsRemoved(const QModelIndex & /*parent*/, int /*first*/, int /*last*/)
{
  endRemoveColumns();
}

void ParserFormatSetupProxyModel::onSourceDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles)
{
  Q_ASSERT( (!topLeft.isValid()) || (topLeft.model() == sourceModel()) );
  Q_ASSERT( (!bottomRight.isValid()) || (bottomRight.model() == sourceModel()) );
  dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}

void ParserFormatSetupProxyModel::onSourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
  headerDataChanged(orientation, first, last);
}

void ParserFormatSetupProxyModel::onSourceRowsAboutToBeInserted(const QModelIndex & parent, int start, int end)
{
  Q_ASSERT( (!parent.isValid()) || (parent.model() == sourceModel()) );
  beginInsertRows(mapFromSource(parent), start, end);
}

void ParserFormatSetupProxyModel::onSourceRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
  Q_ASSERT( (!parent.isValid()) || (parent.model() == sourceModel()) );
  beginRemoveRows(mapFromSource(parent), start, end);
}

void ParserFormatSetupProxyModel::onSourceRowsInserted(const QModelIndex & /*parent*/, int /*start*/, int /*end*/)
{
  endInsertRows();
}

void ParserFormatSetupProxyModel::onSourceRowsRemoved(const QModelIndex & /*parent*/, int /*start*/, int /*end*/)
{
  endRemoveRows();
}

QVariant ParserFormatSetupProxyModel::fieldFormatData(int column, int role) const
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < pvRecordFormatter.fieldCount());

  switch(role){
    case Qt::DisplayRole:
      return FieldType::nameFromType(pvRecordFormatter.fieldType(column));
    case Qt::BackgroundRole:
      return QBrush(Qt::lightGray);
    default:
      return QVariant();
  }
}

void ParserFormatSetupProxyModel::setFieldType(int column, QMetaType::Type type)
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < pvRecordFormatter.fieldCount());

  pvRecordFormatter.setFieldType(column, type);
  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    auto idx = index(row, column);
    emit dataChanged(idx, idx);
  }
}

}} // namespace mdt{ namespace csv{