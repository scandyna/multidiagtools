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
#include "mdtCsvParserModel.h"

#include <QDebug>

mdtCsvParserModel::mdtCsvParserModel(QObject *parent)
 : QAbstractTableModel(parent)
{
}

int mdtCsvParserModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvBuffer.recordCount();
}

int mdtCsvParserModel::columnCount(const QModelIndex & /*parent*/) const
{
  return pvHeader.count();
}

QVariant mdtCsvParserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(pvHeader.columnDataList.isEmpty()){
    return QVariant();
  }
  Q_ASSERT(section >= 0);
  Q_ASSERT(section < pvHeader.count());

  return pvHeader.columnDataList.at(section);
}

QVariant mdtCsvParserModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if(role != Qt::DisplayRole){
    return QVariant();
  }
  const int row = index.row();
  const int column = index.column();
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvBuffer.recordCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < pvBuffer.recordList.at(row).count());

  return pvBuffer.recordList.at(row).columnDataList.at(column);
}

void mdtCsvParserModel::setHeader(const mdtCsvRecord & header)
{
  pvHeader = header;
}

void mdtCsvParserModel::addRecord(const mdtCsvRecord & record)
{
  qDebug() << "REC: " << record.columnDataList;

  Q_ASSERT(record.count() == pvHeader.count());

  beginInsertRows(QModelIndex(), pvBuffer.recordCount(), pvBuffer.recordCount());
  pvBuffer.addRecord(record);
  endInsertRows();
}
