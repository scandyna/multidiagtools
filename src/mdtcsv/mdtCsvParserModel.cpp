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
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(pvHeader.columnDataList.isEmpty()){
    return QVariant();
  }
  
  if(section < 0){
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

  if(column >= pvBuffer.recordList.at(row).count()){
    return QVariant();
  }

  return pvBuffer.recordList.at(row).columnDataList.at(column);
}

bool mdtCsvParserModel::reformatColumnData(int index, QMetaType::Type type)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < columnCount());

  qDebug() << "Model: reformat column " << index << " , type: " << type << " ...";

  return true;
}

void mdtCsvParserModel::setHeader(const mdtCsvRecord & header)
{
  pvHeader = header;
}

void mdtCsvParserModel::addRecord(const mdtCsvRecord & record)
{
  if(record.count() > pvHeader.count()){
    addColumn();
  }
  beginInsertRows(QModelIndex(), pvBuffer.recordCount(), pvBuffer.recordCount());
  pvBuffer.addRecord(record);
  endInsertRows();
}

void mdtCsvParserModel::clearCache()
{
  pvHeader.clear();
  pvBuffer.clear();
}

void mdtCsvParserModel::addColumn()
{
  beginInsertColumns(QModelIndex(), pvHeader.count(), pvHeader.count());
  pvHeader.columnDataList.append("");
  endInsertColumns();
}
