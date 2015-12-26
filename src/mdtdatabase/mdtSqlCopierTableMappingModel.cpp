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
#include "mdtSqlCopierTableMappingModel.h"
#include <QColor>

mdtSqlCopierTableMappingModel::mdtSqlCopierTableMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void mdtSqlCopierTableMappingModel::resetFieldMapping()
{
  beginResetModel();
  mappingBase().resetFieldMapping();
  endResetModel();
}

void mdtSqlCopierTableMappingModel::generateFieldMappingByName()
{
  beginResetModel();
  mappingBase().generateFieldMappingByName();
  endResetModel();
}

int mdtSqlCopierTableMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return mappingBase().fieldCount();
}

int mdtSqlCopierTableMappingModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 5;
}

QVariant mdtSqlCopierTableMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    case FieldMappinStateIndex:
      return tr("Field mapping\nstate");
  }

  return section;
}

QVariant mdtSqlCopierTableMappingModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  const int row = index.row();
  const int column = index.column();
  Q_ASSERT( (row >= 0) && (row < mappingBase().fieldCount()) );

  if( (role == Qt::DecorationRole) && (column == FieldMappinStateIndex) ){
    return fieldMappingStateDecoration(mappingBase().fieldMappingState(row));
  }
  if( (role != Qt::DisplayRole) && (role != Qt::EditRole) ){
    return QVariant();
  }
  switch(column){
    case SourceNameIndex:
      return mappingBase().sourceFieldName(row);
    case SourceTypeIndex:
      return mappingBase().sourceFieldTypeName(row);
    case DestinationNameIndex:
      return mappingBase().destinationFieldName(row);
    case DestinationTypeIndex:
      return mappingBase().destinationFieldTypeName(row);
    case FieldMappinStateIndex:
      return fieldMappingStateText(mappingBase().fieldMappingState(row));
  }

  return QVariant();
}

bool mdtSqlCopierTableMappingModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if(role != Qt::EditRole){
    return false;
  }
  Q_ASSERT(index.column() == SourceNameIndex); /// Currently, we only support selecting source field name
  int row = index.row();
  Q_ASSERT( (row >= 0) && (row < mappingBase().fieldCount()) );
  mappingBase().setSourceField(row, value.toString());
  // Signal that we updated data
  emit dataChanged(index, index);

  return true;
}

Qt::ItemFlags mdtSqlCopierTableMappingModel::flags(const QModelIndex & index) const
{
  if(!index.isValid()){
    return QAbstractTableModel::flags(index);
  }
  if(index.column() == SourceNameIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

QVariant mdtSqlCopierTableMappingModel::fieldMappingStateData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return fieldMappingStateText(mappingBase().fieldMappingState(row));
    case Qt::DecorationRole:
      return fieldMappingStateDecoration(mappingBase().fieldMappingState(row));
    default:
      return QVariant();
  }
}

QVariant mdtSqlCopierTableMappingModel::fieldMappingStateText(mdtSqlCopierFieldMapping::MappingState state) const
{
  switch(state){
    case mdtSqlCopierFieldMapping::MappingNotSet:
    case mdtSqlCopierFieldMapping::MappingPartial:
      return QString();
    case mdtSqlCopierFieldMapping::MappingComplete:
      return tr("Ok");
    case mdtSqlCopierFieldMapping::MappingError:
      return tr("Error");
  }
  return QVariant();
}

QVariant mdtSqlCopierTableMappingModel::fieldMappingStateDecoration(mdtSqlCopierFieldMapping::MappingState state) const
{
  switch(state){
    case mdtSqlCopierFieldMapping::MappingNotSet:
      return QVariant();
    case mdtSqlCopierFieldMapping::MappingPartial:
      return QColor(Qt::yellow);
    case mdtSqlCopierFieldMapping::MappingComplete:
      return QColor(Qt::green);
    case mdtSqlCopierFieldMapping::MappingError:
      return QColor(Qt::red);
  }
  return QString();
}
