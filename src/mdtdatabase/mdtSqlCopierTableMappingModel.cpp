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
#include "mdtSqlCopierTableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"
#include <QColor>

//#include <QDebug>

mdtSqlCopierTableMappingModel::mdtSqlCopierTableMappingModel(QObject* parent)
 : QAbstractTableModel(parent)
{
}

void mdtSqlCopierTableMappingModel::setupSourceTypeDelegate(mdtComboBoxItemDelegate * const delegate)
{
  Q_ASSERT(delegate != nullptr);

  using mdt::sql::copier::SourceField;

  delegate->clear();
  delegate->addItem(tr("Field"), SourceField::SourceFieldIndexType);
  delegate->addItem(tr("Fixed value"), SourceField::SourceFixedValueType);
  delegate->addItem(tr("Expression"), SourceField::SourceFieldExpressionType);
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
  return 9;
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
    case SourceTypeIndex:
      return tr("Source\ntype");
    case SourceKeyTypeIndex:
      return tr("Source\nKey");
    case SourceFieldNameIndex:
      return tr("Source\nfield name");
    case SourceFieldTypeIndex:
      return tr("Source\nfield type");
    case SourceFixedValueIndex:
      return tr("Source\nfixed value");
    case DestinationKeyTypeIndex:
      return tr("Destination\nKey");
    case DestinationFieldNameIndex:
      return tr("Destination\nfield name");
    case DestinationFieldTypeIndex:
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

  if( (role == Qt::DisplayRole) || (role == Qt::EditRole) ){
    // Text only attributes
    switch(column){
      case SourceTypeIndex:
        return sourceTypeData(row, role);
      case SourceKeyTypeIndex:
        return sourceFieldKeyTypeText(row);
      case SourceFieldNameIndex:
        return sourceFieldNameText(row);
      case SourceFieldTypeIndex:
        return sourceFieldTypeNameText(row);
      case SourceFixedValueIndex:
        return sourceFixedValue(row);
      case DestinationKeyTypeIndex:
        return destinationFieldKeyTypeText(row);
      case DestinationFieldNameIndex:
        return mappingBase().destinationFieldName(row);
      case DestinationFieldTypeIndex:
        return mappingBase().destinationFieldTypeName(row);
      case FieldMappinStateIndex:
        return fieldMappingStateText(mappingBase().fieldMappingState(row));
    }
  }else if(role == Qt::DecorationRole){
    // Decoration attributes
    switch(column){
      case SourceKeyTypeIndex:
        return QVariant();  // For future implementation
      case DestinationKeyTypeIndex:
        return QVariant();  // For future implementation
      case FieldMappinStateIndex:
        return fieldMappingStateDecoration(mappingBase().fieldMappingState(row));
      default:
        return QVariant();
    }
  }

  return QVariant();
}

bool mdtSqlCopierTableMappingModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if(!index.isValid()){
    return false;
  }
  const int row = index.row();
  Q_ASSERT( (row >= 0) && (row < mappingBase().fieldCount()) );
  const int column = index.column();

  /*if( (role == Qt::EditRole) && (column == SourceTypeIndex) ){
    mappingBase().setSourceType(row, static_cast<mdtSqlCopierFieldMapping::SourceType>(value.toInt()));
  }else */ if( (role == Qt::DisplayRole) && (column == SourceFieldNameIndex) ){
    mappingBase().setSourceField(row, value.toString());
  }else if( (role == Qt::EditRole) && (column == SourceFixedValueIndex) ){
    if( (value.type() == QVariant::String) && (value.toString().trimmed().isEmpty()) ){
      mappingBase().setSourceFixedValue(row, QVariant());
    }else{
      mappingBase().setSourceFixedValue(row, value);
    }
  }else{
    return false;
  }
  // Signal that we updated data
  emit dataChanged(index, index);

  return true;
}

Qt::ItemFlags mdtSqlCopierTableMappingModel::flags(const QModelIndex & index) const
{
  if(!index.isValid()){
    return QAbstractTableModel::flags(index);
  }
  const int column = index.column();
  if(column == SourceTypeIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  if(column == SourceFieldNameIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  if(column == SourceFixedValueIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

QVariant mdtSqlCopierTableMappingModel::sourceTypeData(int row, int role) const
{
  using mdt::sql::copier::SourceField;

  auto type = mappingBase().sourceType(row);
  // For edit role, we return source type
  if(role == Qt::EditRole){
    return type;
  }
  Q_ASSERT(role == Qt::DisplayRole);
  // For display role, we return source type text
  switch(type){
    case SourceField::SourceFieldIndexType:
      return tr("Field");
    case SourceField::SourceFixedValueType:
      return tr("Fixed value");
    case SourceField::SourceFieldExpressionType:
      return tr("Expression");
  }
  return QVariant();
}

QVariant mdtSqlCopierTableMappingModel::sourceFieldKeyTypeText(int row) const
{
  using mdt::sql::copier::SourceField;

  if(mappingBase().sourceType(row) != SourceField::SourceFieldIndexType){
    return QVariant();
  }
  switch(mappingBase().sourceFieldKeyType(row)){
    case mdtSqlCopierTableMapping::NotAKey:
      return QVariant();
    case mdtSqlCopierTableMapping::PrimaryKey:
      return "PK";
  }
  return QVariant();
}

QVariant mdtSqlCopierTableMappingModel::sourceFieldNameText(int row) const
{
  using mdt::sql::copier::SourceField;

  if(mappingBase().sourceType(row) != SourceField::SourceFieldIndexType){
    return QVariant();
  }
  return mappingBase().sourceFieldName(row);
}

QVariant mdtSqlCopierTableMappingModel::sourceFieldTypeNameText(int row) const
{
  using mdt::sql::copier::SourceField;

  if(mappingBase().sourceType(row) != SourceField::SourceFieldIndexType){
    return QVariant();
  }
  return mappingBase().sourceFieldTypeName(row);
}

QVariant mdtSqlCopierTableMappingModel::sourceFixedValue(int row) const
{
  using mdt::sql::copier::SourceField;

  if(mappingBase().sourceType(row) != SourceField::SourceFixedValueType){
    return QVariant();
  }
  return mappingBase().sourceFixedValue(row);
}

QVariant mdtSqlCopierTableMappingModel::destinationFieldKeyTypeText(int row) const
{
  switch(mappingBase().destinationFieldKeyType(row)){
    case mdtSqlCopierTableMapping::NotAKey:
      return QVariant();
    case mdtSqlCopierTableMapping::PrimaryKey:
      return "PK";
  }
  return QVariant();
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
