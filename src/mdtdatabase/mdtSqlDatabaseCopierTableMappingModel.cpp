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
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"

#include <QDebug>

mdtSqlDatabaseCopierTableMappingModel::mdtSqlDatabaseCopierTableMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

// bool mdtSqlDatabaseCopierTableMappingModel::setSourceDatabase(const QSqlDatabase& db)
// {
//   beginResetModel();
//   if(!pvMapping.setSourceDatabase(db)){
//     pvLastError = pvMapping.lastError();
//     return false;
//   }
//   endResetModel();
// 
//   return true;
// }

// bool mdtSqlDatabaseCopierTableMappingModel::setDestinationDatabase(const QSqlDatabase& db)
// {
//   beginResetModel();
//   if(!pvMapping.setDestinationDatabase(db)){
//     pvLastError = pvMapping.lastError();
//     return false;
//   }
//   endResetModel();
// 
//   return true;
// }

bool mdtSqlDatabaseCopierTableMappingModel::setSourceTable(const QString & tableName, const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setSourceTable(tableName, db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

bool mdtSqlDatabaseCopierTableMappingModel::setDestinationTable(const QString & tableName, const QSqlDatabase & db, mdtComboBoxItemDelegate* delegate)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setDestinationTable(tableName, db);
  if( (ok) && (delegate != nullptr) ){
    delegate->clear();
    delegate->addItem("");
    delegate->addItems(pvMapping.getDestinationFieldNameList());
  }
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

void mdtSqlDatabaseCopierTableMappingModel::resetFieldMapping()
{
  beginResetModel();
  pvMapping.resetFieldMapping();
  endResetModel();
}

void mdtSqlDatabaseCopierTableMappingModel::generateFieldMappingByName()
{
  beginResetModel();
  pvMapping.generateFieldMappingByName();
  endResetModel();
}

void mdtSqlDatabaseCopierTableMappingModel::setMapping(const mdtSqlDatabaseCopierTableMapping & m)
{
  beginResetModel();
  pvMapping = m;
  endResetModel();
}

int mdtSqlDatabaseCopierTableMappingModel::rowCount(const QModelIndex& parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvMapping.fieldCount();
}

int mdtSqlDatabaseCopierTableMappingModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 4;
}

QVariant mdtSqlDatabaseCopierTableMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex_t>(section)){
    case SourceNameIndex:
      return tr("Source\nfield name");
    case SourceTypeIndex:
      return tr("Source\nfield type");
    case DestinationNameIndex:
      return tr("Destination\nfield name");
    case DestinationTypeIndex:
      return tr("Destination\nfield type");
  }

  return section;
}

QVariant mdtSqlDatabaseCopierTableMappingModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvMapping.fieldCount()) );
  switch(index.column()){
    case SourceNameIndex:
      return pvMapping.sourceFieldName(row);
    case SourceTypeIndex:
      return pvMapping.sourceFieldTypeName(row);
    case DestinationNameIndex:
      return pvMapping.destinationFieldName(row);
    case DestinationTypeIndex:
      return pvMapping.destinationFieldTypeName(row);
  }

  return QVariant();
}

Qt::ItemFlags mdtSqlDatabaseCopierTableMappingModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return QAbstractTableModel::flags(index);
  }
  if(index.column() == DestinationNameIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

bool mdtSqlDatabaseCopierTableMappingModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if(role != Qt::EditRole){
    return false;
  }
  Q_ASSERT(index.column() == DestinationNameIndex); /// Currently, we only support selecting destination field name
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < pvMapping.fieldCount()) );
  pvMapping.setDestinationField(row, value.toString());
  // Signal that we updated data
  emit dataChanged(index, index);

  return true;
}
