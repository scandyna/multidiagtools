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
#include "FieldSelectionProxyModel.h"
#include <algorithm>

#include <QDebug>

namespace mdt{ namespace sql{

FieldSelectionProxyModel::FieldSelectionProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
  connect(this, &FieldSelectionProxyModel::modelReset, this, &FieldSelectionProxyModel::resetSelectionList);
}

void FieldSelectionProxyModel::setFieldIndexesToHide(const FieldIndexList& indexes)
{
  beginResetModel();
  pvFieldIndexesToHide = indexes;
  endResetModel();
}

void FieldSelectionProxyModel::showAllFieldIndexes()
{
  beginResetModel();
  pvFieldIndexesToHide.clear();
  endResetModel();
}

int FieldSelectionProxyModel::columnCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return QSortFilterProxyModel::columnCount(parent);
  }
  return QSortFilterProxyModel::columnCount(parent) + 1;
}

QVariant FieldSelectionProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_ASSERT(section >= 0);

  if(orientation != Qt::Horizontal){
    return QSortFilterProxyModel::headerData(section, orientation, role);
  }
  if(section == 0){
    switch(role){
      case Qt::DisplayRole:
        return tr("Sel");
      default:
        return QVariant();
    }
  }
  return QSortFilterProxyModel::headerData(section-1, orientation, role);
}

QVariant FieldSelectionProxyModel::data(const QModelIndex & originalIndex, int role) const
{
  if(!originalIndex.isValid()){
    return QVariant();
  }
  if(originalIndex.column() == 0){
    return selectionColumnData(originalIndex.row(), role);
  }
  auto idx = index(originalIndex.row(), originalIndex.column()-1, originalIndex.parent());

  return QSortFilterProxyModel::data(idx, role);
}

Qt::ItemFlags FieldSelectionProxyModel::flags(const QModelIndex & index) const
{
  if( (index.isValid()) && (index.column() == 0) ){
    return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QSortFilterProxyModel::flags(index);
}

bool FieldSelectionProxyModel::setData(const QModelIndex & originalIndex, const QVariant & value, int role)
{
  if(!originalIndex.isValid()){
    return false;
  }
  if(originalIndex.column() == 0){
    if( setSelectionColumnData(originalIndex.row(), value, role) ){
      emit dataChanged(originalIndex, originalIndex);
      return true;
    }
    return false;
  }
  auto idx = index(originalIndex.row(), originalIndex.column()-1, originalIndex.parent());

  return QSortFilterProxyModel::setData(idx, value, role);
}

void FieldSelectionProxyModel::sort(int column, Qt::SortOrder sortOrder)
{
  if(column > 0){
    QSortFilterProxyModel::sort(column-1, sortOrder);
  }
}

QVariant FieldSelectionProxyModel::selectionColumnData(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvSelectionColumnList.size());

  switch(role){
    case Qt::CheckStateRole:
      if(pvSelectionColumnList[row]){
        return Qt::Checked;
      }
      return Qt::Unchecked;
    default:
      return QVariant();
  }
}

bool FieldSelectionProxyModel::setSelectionColumnData(int row, const QVariant & value, int role)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvSelectionColumnList.size());

  qDebug() << "setSelectionColumnData() - row: " << row << ", value: " << value << " , role: " << role;

  if(role == Qt::CheckStateRole){
    pvSelectionColumnList[row] = value.toBool();
    return true;
  }
  return false;
}

void FieldSelectionProxyModel::resetSelectionList()
{
  qDebug() << "-- Reset, rows: " << rowCount();
  pvSelectionColumnList.assign(rowCount(), false);
}

// QModelIndex FieldSelectionProxyModel::mapToSource(const QModelIndex & proxyIndex) const
// {
//   if( (!proxyIndex.isValid()) || (proxyIndex.parent().isValid()) ){
//     return QSortFilterProxyModel::mapToSource(proxyIndex);
//   }
//   qDebug() << "mapToSource(): proxyIndex: " << proxyIndex;
//   auto idx = index(proxyIndex.row(), proxyIndex.column()-1, proxyIndex.parent());
//   qDebug() << " mapToSource(): idx: " << idx;
//   return QSortFilterProxyModel::mapToSource(idx);
// }

// QModelIndex FieldSelectionProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
// {
//   if( (!sourceIndex.isValid()) || (sourceIndex.parent().isValid()) ){
//     return QSortFilterProxyModel::mapFromSource(sourceIndex);
//   }
//   qDebug() << "mapFromSource(): sourceIndex: " << sourceIndex;
//   auto idx = index(sourceIndex.row(), sourceIndex.column()+1, sourceIndex.parent());
//   qDebug() << " mapFromSource(): idx: " << idx;
//   return QSortFilterProxyModel::mapFromSource(idx);
// }

// QModelIndex FieldSelectionProxyModel::index(int row, int column, const QModelIndex& parent) const
// {
//   return QSortFilterProxyModel::index(row, column+1, parent);
// }


bool FieldSelectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex & /* source_parent*/) const
{
  return ( std::find(pvFieldIndexesToHide.cbegin(), pvFieldIndexesToHide.cend(), source_row) == pvFieldIndexesToHide.cend() );
}

}} // namespace mdt{ namespace sql{
