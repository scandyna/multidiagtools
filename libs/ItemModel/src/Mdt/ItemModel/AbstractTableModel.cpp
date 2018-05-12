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
#include "AbstractTableModel.h"

namespace Mdt{ namespace ItemModel{

AbstractTableModel::AbstractTableModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

int AbstractTableModel::rowCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  return rowCountImpl();
}

int AbstractTableModel::columnCount(const QModelIndex& parent) const
{
  if(parent.isValid()){
    return 0;
  }
  return columnCountImpl();
}

Qt::ItemFlags AbstractTableModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return Qt::NoItemFlags;
  }
  if(mIsModelEditable){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

QVariant AbstractTableModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  Q_ASSERT(index.row() < rowCountImpl());
  Q_ASSERT(index.column() < columnCountImpl());

  return displayRoleData(index.row(), index.column());
}

bool AbstractTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if(role != Qt::EditRole){
    return false;
  }
  Q_ASSERT(index.row() < rowCountImpl());
  Q_ASSERT(index.column() < columnCountImpl());

  if(!setEditRoleData(index.row(), index.column(), value)){
    return false;
  }
  emitDataChanged(index);

  return true;
}

bool AbstractTableModel::setEditRoleData(int, int, const QVariant&)
{
  return false;
}

void AbstractTableModel::setModelEditable(bool editable)
{
  mIsModelEditable = editable;
}

void AbstractTableModel::emitDataChanged(const QModelIndex& index)
{
  Q_ASSERT(index.isValid());
  Q_ASSERT(index.row() < rowCountImpl());
  Q_ASSERT(index.column() < columnCountImpl());

  emit dataChanged(index, index);
}

}} // namespace Mdt{ namespace ItemModel{
