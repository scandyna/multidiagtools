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
#include "mdtSqlDatabaseCopierMappingModel.h"
#include <QColor>

//#include <QDebug>

using mdt::sql::copier::DatabaseCopierTableMapping;

mdtSqlDatabaseCopierMappingModel::mdtSqlDatabaseCopierMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

bool mdtSqlDatabaseCopierMappingModel::setSourceDatabase(const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setSourceDatabase(db);
  pvRowCopyProgress.assign(pvMapping.tableMappingCount(), 0);
  pvRowCopyStatus.assign(pvMapping.tableMappingCount(), CopyStatusUnknown);
  pvRowCopyStatusText.assign(pvMapping.tableMappingCount(), QString());
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
  pvRowCopyProgress.assign(pvMapping.tableMappingCount(), 0);
  pvRowCopyStatus.assign(pvMapping.tableMappingCount(), CopyStatusUnknown);
  pvRowCopyStatusText.assign(pvMapping.tableMappingCount(), QString());
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

bool mdtSqlDatabaseCopierMappingModel::resetTableMapping()
{
  bool ok;

  beginResetModel();
  ok = pvMapping.resetTableMapping();
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

std::shared_ptr<DatabaseCopierTableMapping> mdtSqlDatabaseCopierMappingModel::tableMapping(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvMapping.tableMappingCount());

  return pvMapping.tableMapping(row);
}

void mdtSqlDatabaseCopierMappingModel::setTableMapping(int row, const std::shared_ptr<DatabaseCopierTableMapping> & tm)
{
  Q_ASSERT(tm);
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvMapping.tableMappingCount());

  pvMapping.setTableMapping(row, tm);
  auto firstIdx = index(row, SourceTableNameIndex);
  auto lastIdx = index(row, TableMappingStateIndex);
  emit dataChanged(firstIdx, lastIdx);
}

void mdtSqlDatabaseCopierMappingModel::setTableCopyProgress(int row, int progress)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyProgress.size());

  pvRowCopyProgress[row] = progress;
  auto idx = index(row, TableCopyProgressIndex);
  emit dataChanged(idx, idx);
}

void mdtSqlDatabaseCopierMappingModel::setTableCopyStatus(int row, int status)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyStatusText.size());

  pvRowCopyStatus[row] = static_cast<CopyStatus>(status);
  pvRowCopyStatusText[row] = QString();
  auto idx = index(row, TableCopyStatusIndex);
  emit dataChanged(idx, idx);
}

void mdtSqlDatabaseCopierMappingModel::setTableCopyError(int row, mdtError error)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyStatusText.size());

  pvRowCopyStatus[row] = CopyStatusError;
  pvRowCopyStatusText[row] = error.text();
  auto idx = index(row, TableCopyStatusIndex);
  emit dataChanged(idx, idx);
}

void mdtSqlDatabaseCopierMappingModel::clearCopyStatusAndProgress()
{
  beginResetModel();
  pvRowCopyProgress.assign(pvMapping.tableMappingCount(), 0);
  pvRowCopyStatus.assign(pvMapping.tableMappingCount(), CopyStatusUnknown);
  pvRowCopyStatusText.assign(pvMapping.tableMappingCount(), QString());
  endResetModel();
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
  return 5;
}

QVariant mdtSqlDatabaseCopierMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex>(section)){
    case SourceTableNameIndex:
      return tr("Source table");
    case DestinationTableNameIndex:
      return tr("Destination table");
    case TableMappingStateIndex:
      return tr("Mapping state");
    case TableCopyProgressIndex:
      return tr("Copy progress");
    case TableCopyStatusIndex:
      return tr("Copy status");
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
  auto column = static_cast<ColumnIndex>(index.column());
  switch(column){
    case SourceTableNameIndex:
      return sourceTableData(row, role);
    case DestinationTableNameIndex:
      return destinationTableData(row, role);
    case TableMappingStateIndex:
      return tableMappingStateData(row, role);
    case TableCopyProgressIndex:
      return tableCopyProgress(row, role);
    case TableCopyStatusIndex:
      return tableCopyStatusData(row, role);
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

QString mdtSqlDatabaseCopierMappingModel::tableMappingStateText(DatabaseCopierTableMapping::MappingState state) const
{
  switch(state){
    case DatabaseCopierTableMapping::MappingNotSet:
      return "";
    case DatabaseCopierTableMapping::MappingComplete:
      return tr("Ok");
    case DatabaseCopierTableMapping::MappingPartial:
      return tr("Partial");
    case DatabaseCopierTableMapping::MappingError:
      return tr("Error");
  }
  return "";
}

QVariant mdtSqlDatabaseCopierMappingModel::tableMappingStateDecoration(DatabaseCopierTableMapping::MappingState state) const
{
  switch(state){
    case DatabaseCopierTableMapping::MappingNotSet:
      return QVariant();
    case DatabaseCopierTableMapping::MappingComplete:
      return QColor(Qt::green);
    case DatabaseCopierTableMapping::MappingPartial:
      return QColor(Qt::yellow);
    case DatabaseCopierTableMapping::MappingError:
      return QColor(Qt::red);
  }
  return QVariant();
}

QVariant mdtSqlDatabaseCopierMappingModel::tableCopyProgress(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyProgress.size());

  if(role != Qt::DisplayRole){
    return QVariant();
  }

  return pvRowCopyProgress[row];
}

QVariant mdtSqlDatabaseCopierMappingModel::tableCopyStatusData(int row, int role) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvRowCopyStatus.size());
  Q_ASSERT(row < (int)pvRowCopyStatusText.size());

  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return tableCopyStatusText(pvRowCopyStatus[row]);
    case Qt::ToolTipRole:
      return pvRowCopyStatusText[row];
    case Qt::DecorationRole:
      return tableCopyStatusDecoration(pvRowCopyStatus[row]);
    default:
      return QVariant();
  }
}

QString mdtSqlDatabaseCopierMappingModel::tableCopyStatusText(CopyStatus status) const
{
  switch(status){
    case CopyStatusUnknown:
      return "";
    case CopyStatusOk:
      return tr("Ok");
    case CopyStatusError:
      return tr("Error");
  }
  return "";
}

QVariant mdtSqlDatabaseCopierMappingModel::tableCopyStatusDecoration(CopyStatus status) const
{
  switch(status){
    case CopyStatusUnknown:
      return QVariant();
    case CopyStatusOk:
      return QColor(Qt::green);
    case CopyStatusError:
      return QColor(Qt::red);
  }
  return QVariant();
}
