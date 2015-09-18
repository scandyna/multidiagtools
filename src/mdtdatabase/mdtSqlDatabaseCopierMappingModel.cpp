/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlDatabaseCopierMappingModel.h"
#include <QColor>

#include <QDebug>

mdtSqlDatabaseCopierMappingModel::mdtSqlDatabaseCopierMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

bool mdtSqlDatabaseCopierMappingModel::setSourceDatabase(const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setSourceDatabase(db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

bool mdtSqlDatabaseCopierMappingModel::setDestinationDatabase(const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setDestinationDatabase(db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

bool mdtSqlDatabaseCopierMappingModel::generateTableMappingByName()
{
  bool ok;

  beginResetModel();
  ok = pvMapping.generateTableMappingByName();
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

int mdtSqlDatabaseCopierMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvMapping.tableMappingCount();
}

int mdtSqlDatabaseCopierMappingModel::columnCount(const QModelIndex &) const
{
  return 3;
}

QVariant mdtSqlDatabaseCopierMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex_t>(section)){
    case SourceTableNameIndex:
      return tr("Source table");
    case DestinationTableNameIndex:
      return tr("Destination table");
    case TableMappingStateIndex:
      return tr("Mapping state");
  }

  return section;
}

QVariant mdtSqlDatabaseCopierMappingModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
//   if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
//     return QVariant();
//   }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvMapping.tableMappingCount()) );
  switch(index.column()){
    case SourceTableNameIndex:
      return sourceTableData(row, role);
    case DestinationTableNameIndex:
      return destinationTableData(row, role);
    case TableMappingStateIndex:
      return tableMappingStateData(row, role);
  }

  return QVariant();
}

QVariant mdtSqlDatabaseCopierMappingModel::sourceTableData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      // Return text informations
      return pvMapping.sourceTableName(row);
    default:
      return QVariant();
  }
}

QVariant mdtSqlDatabaseCopierMappingModel::destinationTableData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      // Return text informations
      return pvMapping.destinationTableName(row);
    default:
      return QVariant();
  }
}

QVariant mdtSqlDatabaseCopierMappingModel::tableMappingStateData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return tableMappingStateText(pvMapping.tableMappingState(row));
    case Qt::DecorationRole:
      return tableMappingStateDecoration(pvMapping.tableMappingState(row));
    default:
      return QVariant();
  }
}

QString mdtSqlDatabaseCopierMappingModel::tableMappingStateText(mdtSqlDatabaseCopierTableMapping::MappingState state) const
{
  switch(state){
    case mdtSqlDatabaseCopierTableMapping::MappingNotSet:
      return "";
    case mdtSqlDatabaseCopierTableMapping::MappingComplete:
      return tr("Ok");
    case mdtSqlDatabaseCopierTableMapping::MappingPartial:
      return tr("Partial");
    case mdtSqlDatabaseCopierTableMapping::MappingError:
      return tr("Error");
  }
  return "";
}

QVariant mdtSqlDatabaseCopierMappingModel::tableMappingStateDecoration(mdtSqlDatabaseCopierTableMapping::MappingState state) const
{
  switch(state){
    case mdtSqlDatabaseCopierTableMapping::MappingNotSet:
      return QVariant();
    case mdtSqlDatabaseCopierTableMapping::MappingComplete:
      return QColor(Qt::green);
    case mdtSqlDatabaseCopierTableMapping::MappingPartial:
      return QColor(Qt::yellow);
    case mdtSqlDatabaseCopierTableMapping::MappingError:
      return QColor(Qt::red);
  }
  return QVariant();
}
