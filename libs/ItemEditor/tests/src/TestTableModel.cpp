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
#include "TestTableModel.h"
#include <QChar>

// #include <QDebug>

TestTableModel::TestTableModel(TestTableModelRoleStorage s, QObject* parent)
 : QAbstractTableModel(parent),
   pvStorage(s),
   pvColumnCount(0)
{
}

int TestTableModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return pvColumnCount;
}

int TestTableModel::rowCount(const QModelIndex& parent) const
{
   if(parent.isValid()){
     return 0;
   }
   return pvData.size();
}

QVariant TestTableModel::data(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < (int)pvData.size());
  Q_ASSERT(index.column() >= 0);
  Q_ASSERT(index.column() < pvColumnCount);

  switch(role){
    case Qt::DisplayRole:
      return pvData[index.row()][index.column()].displayRoleData();
    case Qt::EditRole:
      return pvData[index.row()][index.column()].editRoleData();
    default:
      break;
  }
  return QVariant();
}

void TestTableModel::setItemEnabled(const QModelIndex& index, bool enable)
{
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < (int)pvData.size());
  Q_ASSERT(index.column() >= 0);
  Q_ASSERT(index.column() < pvColumnCount);

  pvData[index.row()][index.column()].setEnabled(enable);
}

void TestTableModel::setItemEditable(const QModelIndex& index, bool editable)
{
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < (int)pvData.size());
  Q_ASSERT(index.column() >= 0);
  Q_ASSERT(index.column() < pvColumnCount);

  pvData[index.row()][index.column()].setEditable(editable);
}

Qt::ItemFlags TestTableModel::flags(const QModelIndex& index) const
{
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < (int)pvData.size());
  Q_ASSERT(index.column() >= 0);
  Q_ASSERT(index.column() < pvColumnCount);

  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  const TestTableModelItemData item = pvData[index.row()][index.column()];

  if(!item.isEnabled()){
    f &= Qt::ItemFlags(~Qt::ItemIsEnabled);
  }
  if(item.isEditable()){
    f |= Qt::ItemIsEditable;
  }

  return f;
}

bool TestTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < (int)pvData.size());
  Q_ASSERT(index.column() >= 0);
  Q_ASSERT(index.column() < pvColumnCount);

  switch(role){
    case Qt::DisplayRole:
      pvData[index.row()][index.column()].setDisplayRoleData(value);
      break;
    case Qt::EditRole:
      pvData[index.row()][index.column()].setEditRoleData(value);
      break;
    default:
      return false;
  }
  emit dataChanged(index, index);
  return true;
}

void TestTableModel::populate(int rows, int columns)
{
  beginResetModel();
  pvColumnCount = columns;
  pvData.clear();
  pvData.reserve(rows);
  for(int i = 0; i < rows; ++i){
    pvData.push_back(getRowData(i));
  }
  endResetModel();
}

void TestTableModel::clear()
{
  beginResetModel();
  pvData.clear();
  pvColumnCount = 0;
  endResetModel();
}

std::vector<TestTableModelItemData> TestTableModel::getRowData(int currentRow)
{
  std::vector<TestTableModelItemData> data;

  data.reserve(pvColumnCount);
  for(int i = 0; i < pvColumnCount; ++i){
    data.emplace_back(pvStorage);
    data[i].setDisplayRoleData( QString("%1%2").arg(currentRow).arg( QChar('A'+i) ) );
  }

  return data;
}
