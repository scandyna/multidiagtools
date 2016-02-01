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
 : QSortFilterProxyModel(parent),
   pvFieldSelectionMode(MultiSelection)
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

void FieldSelectionProxyModel::setFieldSelectionMode(FieldSelectionMode mode)
{
  pvFieldSelectionMode = mode;
  clearFieldIndexSelection();
}

void FieldSelectionProxyModel::setFieldIndexSelected(int fieldIndex, bool select)
{
  Q_ASSERT(sourceModel() != nullptr);

  QModelIndex sourceIndex = sourceModel()->index(fieldIndex, 0);
  QModelIndex proxyIndex = mapFromSource(sourceIndex);
  Q_ASSERT(proxyIndex.isValid());
  Q_ASSERT(proxyIndex.row() < pvSelectionColumnList.size());

  setSelectionColumnAt(proxyIndex.row(), select);
  signalSelectionColumnDataChanged(proxyIndex);
}

void FieldSelectionProxyModel::clearFieldIndexSelection()
{
  // We simply reset the model, witch will call resetSelectionList()
  beginResetModel();
  endResetModel();
}

FieldIndexList FieldSelectionProxyModel::getSelectedFieldIndexes() const
{
  Q_ASSERT(sourceModel() != nullptr);

  FieldIndexList selectedFields;

  for(int proxyRow = 0; proxyRow < pvSelectionColumnList.size(); ++proxyRow){
    if(pvSelectionColumnList.testBit(proxyRow)){
      QModelIndex proxyIndex = index(proxyRow, 0);
      QModelIndex sourceIndex = mapToSource(proxyIndex);
      Q_ASSERT(sourceIndex.isValid());
      selectedFields.append(sourceIndex.row());
    }
  }
  if(selectedFields.count() > 1){
    std::sort(selectedFields.begin(), selectedFields.end());
  }

  return selectedFields;
}

// int FieldSelectionProxyModel::columnCount(const QModelIndex& parent) const
// {
//   if(parent.isValid()){
//     return QSortFilterProxyModel::columnCount(parent);
//   }
//   return QSortFilterProxyModel::columnCount(parent) + 1;
// }

// QModelIndex FieldSelectionProxyModel::mapToSource(const QModelIndex &proxyIndex) const
// {
//   Q_ASSERT(sourceModel() != nullptr);
// 
//   qDebug() << "mapToSource() - proxyIndex: " << proxyIndex;
//   
//   if(!proxyIndex.isValid()){
//     return QModelIndex();
//   }
//   if(proxyIndex.column() == 0){
//     return QModelIndex();
//   }
//   auto idx = sourceModel()->index(proxyIndex.row(), proxyIndex.column()-1, proxyIndex.parent());
//   
//   return QSortFilterProxyModel::mapToSource(idx);
// }

// QModelIndex FieldSelectionProxyModel::index(int row, int column, const QModelIndex &parent) const
// {
//   if(parent.isValid()){
//     return QModelIndex();
//   }
//   return createIndex(row, column);
// //   if( (column == 0) && (!parent.isValid()) ){
// //     return createIndex(row, column);
// //   }
//   ///return QSortFilterProxyModel::index(row, column, parent);
// }

// QVariant FieldSelectionProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
// {
//   Q_ASSERT(section >= 0);
// 
//   if(orientation != Qt::Horizontal){
//     return QSortFilterProxyModel::headerData(section, orientation, role);
//   }
//   if(section == 0){
//     switch(role){
//       case Qt::DisplayRole:
//         return tr("Sel");
//       default:
//         return QVariant();
//     }
//   }
//   return QSortFilterProxyModel::headerData(section-1, orientation, role);
// }

QVariant FieldSelectionProxyModel::data(const QModelIndex & originalIndex, int role) const
{
  if(!originalIndex.isValid()){
    return QVariant();
  }
  if( (originalIndex.column() == 0) && (role == Qt::CheckStateRole) ){
    Q_ASSERT(originalIndex.row() >= 0);
    Q_ASSERT(originalIndex.row() < pvSelectionColumnList.size());
    if(pvSelectionColumnList.testBit(originalIndex.row())){
      return Qt::Checked;
    }else{
      return Qt::Unchecked;
    }
    ///return selectionColumnData(originalIndex.row(), role);
  }
  qDebug() << "data():";
  ///auto idx = index(originalIndex.row(), originalIndex.column()-1, originalIndex.parent());
  ///auto idx = mapToSource(originalIndex);
  qDebug() << " originalIndex: " << originalIndex;
  ///qDebug() << " idx:           " << idx;

  ///return QSortFilterProxyModel::data(idx, role);
  return QSortFilterProxyModel::data(originalIndex, role);
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
      signalSelectionColumnDataChanged(originalIndex);
      return true;
    }
    return false;
  }
  auto idx = index(originalIndex.row(), originalIndex.column()-1, originalIndex.parent());

  return QSortFilterProxyModel::setData(idx, value, role);
}

// void FieldSelectionProxyModel::sort(int column, Qt::SortOrder sortOrder)
// {
//   if(column > 0){
//     QSortFilterProxyModel::sort(column-1, sortOrder);
//   }
// }

QVariant FieldSelectionProxyModel::selectionColumnData(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvSelectionColumnList.size());

  switch(role){
    case Qt::CheckStateRole:
      if(pvSelectionColumnList.testBit(row)){
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
  Q_ASSERT(row < pvSelectionColumnList.size());

  if(role == Qt::CheckStateRole){
    setSelectionColumnAt(row, value.toBool());
    return true;
  }
  return false;
}

void FieldSelectionProxyModel::setSelectionColumnAt(int row, bool state)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvSelectionColumnList.size());

  if(pvFieldSelectionMode == SingleSelection){
    pvSelectionColumnList.fill(false);
  }
  pvSelectionColumnList.setBit(row, state);
}

void FieldSelectionProxyModel::signalSelectionColumnDataChanged(const QModelIndex & updatedIndex)
{
  Q_ASSERT(updatedIndex.column() == 0);

  if(pvFieldSelectionMode == MultiSelection){
    emit dataChanged(updatedIndex, updatedIndex);
  }else{
    for(int row = 0; row < rowCount(); ++row){
      auto idx = index(row, 0);
      emit dataChanged(idx, idx);
    }
  }
}

void FieldSelectionProxyModel::resetSelectionList()
{
  pvSelectionColumnList.fill(false, rowCount());
}

bool FieldSelectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex & /* source_parent*/) const
{
  return ( std::find(pvFieldIndexesToHide.cbegin(), pvFieldIndexesToHide.cend(), source_row) == pvFieldIndexesToHide.cend() );
}

}} // namespace mdt{ namespace sql{
