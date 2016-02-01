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

//#include <QDebug>

namespace mdt{ namespace sql{

FieldSelectionProxyModel::FieldSelectionProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent),
   pvFieldSelectionMode(FieldSelectionMode::MultiSelection)
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
  Q_ASSERT(proxyIndex.row() < pvSelectedFieldProxyStateList.size());

  setFieldAtProxyRowSelected(proxyIndex.row(), select);
  signalFieldSelectionChanged(proxyIndex);
}

void FieldSelectionProxyModel::clearFieldIndexSelection()
{
  // We simply reset the model, witch will call resetSelectionList()
  beginResetModel();
  endResetModel();
}

FieldIndexList FieldSelectionProxyModel::getSelectedFieldIndexList() const
{
  Q_ASSERT(sourceModel() != nullptr);

  FieldIndexList selectedFields;

  for(int proxyRow = 0; proxyRow < pvSelectedFieldProxyStateList.size(); ++proxyRow){
    if(pvSelectedFieldProxyStateList.testBit(proxyRow)){
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

QStringList FieldSelectionProxyModel::getSelectedFieldNameList() const
{
  Q_ASSERT(sourceModel() != nullptr);

  QStringList fieldNameList;
  auto fieldIndexList = getSelectedFieldIndexList();

  for(int fieldIndex : fieldIndexList){
    auto sourceIndex = sourceModel()->index(fieldIndex, 0);
    Q_ASSERT(sourceIndex.isValid());
    fieldNameList.append(sourceModel()->data(sourceIndex).toString());
  }

  return fieldNameList;
}

QVariant FieldSelectionProxyModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (index.column() == 0) && (role == Qt::CheckStateRole) ){
    Q_ASSERT(index.row() >= 0);
    Q_ASSERT(index.row() < pvSelectedFieldProxyStateList.size());
    if(pvSelectedFieldProxyStateList.testBit(index.row())){
      return Qt::Checked;
    }else{
      return Qt::Unchecked;
    }
  }
  return QSortFilterProxyModel::data(index, role);
}

Qt::ItemFlags FieldSelectionProxyModel::flags(const QModelIndex & index) const
{
  if( (index.isValid()) && (index.column() == 0) ){
    return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QSortFilterProxyModel::flags(index);
}

bool FieldSelectionProxyModel::setData(const QModelIndex & index, const QVariant &value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if( (index.column() == 0) && (role == Qt::CheckStateRole) ){
    setFieldAtProxyRowSelected(index.row(), value.toBool());
    signalFieldSelectionChanged(index);
    return true;
  }
  return QSortFilterProxyModel::setData(index, value, role);
}

void FieldSelectionProxyModel::setFieldAtProxyRowSelected(int row, bool state)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvSelectedFieldProxyStateList.size());

  if(pvFieldSelectionMode == FieldSelectionMode::SingleSelection){
    pvSelectedFieldProxyStateList.fill(false);
  }
  pvSelectedFieldProxyStateList.setBit(row, state);
}

void FieldSelectionProxyModel::signalFieldSelectionChanged(const QModelIndex & updatedProxyIndex)
{
  Q_ASSERT(updatedProxyIndex.column() == 0);

  if(pvFieldSelectionMode == FieldSelectionMode::MultiSelection){
    emit dataChanged(updatedProxyIndex, updatedProxyIndex);
  }else{
    for(int row = 0; row < rowCount(); ++row){
      auto idx = index(row, 0);
      emit dataChanged(idx, idx);
    }
  }
}

void FieldSelectionProxyModel::resetSelectionList()
{
  pvSelectedFieldProxyStateList.fill(false, rowCount());
}

bool FieldSelectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex & /* source_parent*/) const
{
  return ( std::find(pvFieldIndexesToHide.cbegin(), pvFieldIndexesToHide.cend(), source_row) == pvFieldIndexesToHide.cend() );
}

}} // namespace mdt{ namespace sql{
