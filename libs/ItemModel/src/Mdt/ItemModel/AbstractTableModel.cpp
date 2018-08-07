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

// #include <QDebug>

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

QVariant AbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( (orientation == Qt::Horizontal)&&(role == Qt::DisplayRole) ){
    Q_ASSERT(section >= 0);
    Q_ASSERT(section < columnCount());
    return horizontalHeaderDisplayRoleData(section);
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant AbstractTableModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  Q_ASSERT(index.row() < rowCountImpl());
  Q_ASSERT(index.column() < columnCountImpl());

  if(role == Qt::DisplayRole){
    return displayRoleData(index.row(), index.column());
  }
  if(role == Qt::EditRole){
    return editRoleData(index.row(), index.column());
  }

  return QVariant();
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
//   emitDataChanged(index);

  return true;
}

bool AbstractTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if(parent.isValid()){
    return false;
  }
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= rowCountImpl());
  Q_ASSERT(count >= 0);

  beginInsertRows(parent, row, row+count-1);
  const bool ok = insertRowsImpl(row, count);
  endInsertRows();

  return ok;
}

bool AbstractTableModel::prependRow()
{
  return insertRows(0, 1);
}

bool AbstractTableModel::appendRow()
{
  return insertRows(rowCountImpl(), 1);
}

bool AbstractTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if(parent.isValid()){
    return false;
  }
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);
  Q_ASSERT( (row+count) <= rowCountImpl() );

  beginRemoveRows(parent, row, row+count-1);
  const bool ok = removeRowsImpl(row, count);
  endRemoveRows();

  return ok;
}

bool AbstractTableModel::removeFirstRow()
{
  return removeRows(0, 1);
}

bool AbstractTableModel::removeLastRow()
{
  return removeRows(rowCount()-1, 1);
}

QVariant AbstractTableModel::horizontalHeaderDisplayRoleData(int column) const
{
  return QAbstractTableModel::headerData(column, Qt::Horizontal, Qt::DisplayRole);
}

bool AbstractTableModel::setEditRoleData(int, int, const QVariant&)
{
  return false;
}

bool AbstractTableModel::insertRowsImpl(int, int)
{
  return false;
}

bool AbstractTableModel::removeRowsImpl(int, int)
{
  return false;
}

QVariant AbstractTableModel::editRoleData(int row, int column) const
{
  return displayRoleData(row, column);
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

void AbstractTableModel::emitDataAtRowsChanged(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow >= 0);
  Q_ASSERT(firstRow < rowCountImpl());
  Q_ASSERT(lastRow >= 0);
  Q_ASSERT(lastRow < rowCountImpl());

  const auto topLeft = index(firstRow, 0);
  const auto bottomRight = index(lastRow, columnCount()-1);

  emit dataChanged(topLeft, bottomRight);
}

void AbstractTableModel::emitVerticalHeaderDataChanged(const IndexRange::RowRange& rowRange)
{
  Q_ASSERT(!rowRange.isNull());

  emit headerDataChanged(Qt::Vertical, rowRange.firstRow(), rowRange.lastRow());
}

void AbstractTableModel::emitVerticalHeaderDataChanged(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  emit headerDataChanged(Qt::Vertical, row, row);
}

void AbstractTableModel::emitVerticalHeaderDataChanged(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow >= 0);
  Q_ASSERT(firstRow < rowCount());
  Q_ASSERT(lastRow >= 0);
  Q_ASSERT(lastRow < rowCount());

  emit headerDataChanged(Qt::Vertical, firstRow, lastRow);
}

}} // namespace Mdt{ namespace ItemModel{
