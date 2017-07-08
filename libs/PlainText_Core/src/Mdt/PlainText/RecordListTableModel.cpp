/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "RecordListTableModel.h"
#include "Record.h"
#include <algorithm>

namespace Mdt{ namespace PlainText{

RecordListTableModel::RecordListTableModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

int RecordListTableModel::rowCount(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return 0;
  }
  return mRecordList.rowCount();
}

int RecordListTableModel::columnCount(const QModelIndex & parent) const
{
  if(parent.isValid()){
    return 0;
  }
  return mColumnCount;
}

QVariant RecordListTableModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  Q_ASSERT(index.row() >= 0);
  Q_ASSERT(index.row() < mRecordList.rowCount());
  Q_ASSERT(index.column() >= 0);
  if(index.column() >= mRecordList.columnCount(index.row())){
    return QVariant();
  }
  return mRecordList.data(index.row(), index.column());
}

void RecordListTableModel::setRecordList(const RecordList & recordList)
{
  beginResetModel();
  mRecordList = recordList;
  updateColumnCount();
  endResetModel();
}

void RecordListTableModel::updateColumnCount()
{
  const auto cmp = [](const Record & a, const Record & b){
    return (a.columnCount() < b.columnCount());
  };
  const auto it = std::max_element(mRecordList.cbegin(), mRecordList.cend(), cmp);
  if(it == mRecordList.cend()){
    mColumnCount = 0;
  }else{
    mColumnCount = it->columnCount();
  }
}

}} // namespace Mdt{ namespace PlainText{
