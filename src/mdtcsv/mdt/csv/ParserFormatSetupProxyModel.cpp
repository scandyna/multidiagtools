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
#include <QItemSelection>

#include <QDebug>

namespace mdt{ namespace csv{

ParserFormatSetupProxyModel::ParserFormatSetupProxyModel(QObject *parent)
 : QAbstractProxyModel(parent)
{
}

QModelIndex ParserFormatSetupProxyModel::index(int row, int column, const QModelIndex & parent) const
{
  if( parent.isValid() || (sourceModel() == nullptr) ){
    return QModelIndex();
  }
//   if(row == 0){
//     return createIndex(row, column);
//   }
//   const auto sourceIndex = sourceModel()->index(row, column);



  ///return mapFromSource(sourceIndex);
  ///return createIndex(row, column, sourceIndex.internalPointer());
  qDebug() << "PM::index - row: " << row << ", col: " << column;
  qDebug() << "PM::index: return " << createIndex(row, column);
  
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
  qDebug() << "PM rowCount - model: " << sourceModel() << " - rows: " << sourceModel()->rowCount();
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
  if(sourceModel() == nullptr){
    return QVariant();
  }
  const auto idx = index(proxyIndex.row() - 1, proxyIndex.column());
  if(!idx.isValid()){
    return QVariant();
  }
  return sourceModel()->data(mapToSource(idx), role);
}

Qt::ItemFlags ParserFormatSetupProxyModel::flags(const QModelIndex & proxyIndex) const
{
  if(proxyIndex.row() == 0){
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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

QItemSelection ParserFormatSetupProxyModel::mapSelectionFromSource(const QItemSelection & sourceSelection) const
{
  qDebug() << "PM::mapSelectionFromSource ...";
  return QAbstractProxyModel::mapSelectionFromSource(sourceSelection);
}

QItemSelection ParserFormatSetupProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
  qDebug() << "PM::mapSelectionToSource ...";
  return QAbstractProxyModel::mapSelectionToSource(proxySelection);
}

}} // namespace mdt{ namespace csv{