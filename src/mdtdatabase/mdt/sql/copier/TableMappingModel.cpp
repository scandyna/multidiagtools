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
#include "TableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"
#include <QColor>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

using mdt::sql::copier::TableMapping;

TableMappingModel::TableMappingModel(QObject* parent)
 : QAbstractTableModel(parent),
   pvEditableItemIcon(QIcon::fromTheme("document-edit"))
{
}

void TableMappingModel::setupItemTypeDelegate(mdtComboBoxItemDelegate * const delegate)
{
  Q_ASSERT(delegate != nullptr);

  delegate->clear();
  delegate->addItem(tr("Field"), TableMappingItem::FieldMappingType);
  delegate->addItem(tr("Fixed value"), TableMappingItem::FixedValueType);
  delegate->addItem(tr("Unique insert expression"), TableMappingItem::UniqueInsertExpressionType);
}

void TableMappingModel::resetFieldMapping()
{
  beginResetModel();
  mappingBase()->resetFieldMapping();
  endResetModel();
}

void TableMappingModel::generateFieldMappingByName()
{
  beginResetModel();
  mappingBase()->generateFieldMappingByName();
  endResetModel();
}

void TableMappingModel::insertItem(const TableMappingItem& item)
{
  beginResetModel();
  mappingBase()->insertItem(item);
  endResetModel();
}

int TableMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return mappingBase()->itemsCount();
}

int TableMappingModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 9;
}

QVariant TableMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex_t>(section)){
    case ItemTypeIndex:
      return tr("Mapping\ntype");
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
    case ItemMappinStateIndex:
      return tr("Mapping\nstate");
  }

  return section;
}

QVariant TableMappingModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  const int row = index.row();
  const int column = index.column();
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < mappingBase()->itemsCount());

  if( (role == Qt::DisplayRole) || (role == Qt::EditRole) ){
    // Text only attributes
    switch(column){
      case ItemTypeIndex:
        return mapItemTypeData(row, role);
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
        return destinationFieldNamesText(row);
      case DestinationFieldTypeIndex:
        return destinationFieldTypesText(row);
      case ItemMappinStateIndex:
        return mapItemMappingStateText(mappingBase()->itemMappingState(row));
    }
  }else if(role == Qt::DecorationRole){
    // Decoration attributes
    switch(column){
      case SourceKeyTypeIndex:
        return QVariant();  // For future implementation
      case SourceFieldNameIndex:
      case SourceFixedValueIndex:
        return editFlagDecorationData(index);
      case DestinationKeyTypeIndex:
        return QVariant();  // For future implementation
      case ItemMappinStateIndex:
        return mapItemMappingStateDecoration(mappingBase()->itemMappingState(row));
      default:
        return QVariant();
    }
  }

  return QVariant();
}

bool TableMappingModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if(!index.isValid()){
    return false;
  }
  const int row = index.row();
  Q_ASSERT( (row >= 0) && (row < mappingBase()->itemsCount()) );
  const int column = index.column();

  /*if( (role == Qt::EditRole) && (column == SourceTypeIndex) ){
    mappingBase()->setSourceType(row, static_cast<mdtSqlCopierFieldMapping::SourceType>(value.toInt()));
  }else */ if( (role == Qt::DisplayRole) && (column == SourceFieldNameIndex) ){
    mappingBase()->setSourceFieldAtItem(row, value.toString());
  }else if( (role == Qt::EditRole) && (column == SourceFixedValueIndex) ){
    if( (value.type() == QVariant::String) && (value.toString().trimmed().isEmpty()) ){
      mappingBase()->setSourceFixedValueAtItem(row, QVariant());
    }else{
      mappingBase()->setSourceFixedValueAtItem(row, value);
    }
  }else{
    return false;
  }
  // Signal that we updated data
  emit dataChanged(index, index);

  return true;
}

Qt::ItemFlags TableMappingModel::flags(const QModelIndex & index) const
{
  if(!index.isValid()){
    return QAbstractTableModel::flags(index);
  }
  const int column = index.column();
  const int row = index.row();
  Q_ASSERT(row < mappingBase()->itemsCount());

  if(column == ItemTypeIndex){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  if( (column == SourceFieldNameIndex) && (mappingBase()->itemAt(row).destinationFieldIndexList().count() == 1) ){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  if( (column == SourceFixedValueIndex) && (mappingBase()->itemAt(row).destinationFieldIndexList().count() == 1) ){
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
  return QAbstractTableModel::flags(index);
}

QVariant TableMappingModel::mapItemTypeData(int row, int role) const
{
  auto type = mappingBase()->itemType(row);
  // For edit role, we return map item type
  if(role == Qt::EditRole){
    return type;
  }
  Q_ASSERT(role == Qt::DisplayRole);
  // For display role, we return map item type text
  switch(type){
    case TableMappingItem::FieldMappingType:
      return tr("Field");
    case TableMappingItem::FixedValueType:
      return tr("Fixed value");
    case TableMappingItem::UniqueInsertExpressionType:
      return tr("Unique insert expression");
  }
  return QVariant();
}

QVariant TableMappingModel::sourceFieldKeyTypeText(int row) const
{
  if(mappingBase()->itemType(row) != TableMappingItem::FieldMappingType){
    return QVariant();
  }
  QString ktName;
  QStringList ktNameList;
  auto fieldKeyTypeList = mappingBase()->sourceFieldKeyTypeListAtItem(row);

  for(const auto & kt : fieldKeyTypeList){
    ktName = keyTypeName(kt);
    if(!ktName.isEmpty()){
      ktNameList.append(ktName);
    }
  }

  return ktNameList.join(QStringLiteral(", "));
}

QVariant TableMappingModel::sourceFieldNameText(int row) const
{
  if(mappingBase()->itemType(row) != TableMappingItem::FieldMappingType){
    return QVariant();
  }
  return mappingBase()->sourceFieldNameListAtItem(row).join(", ");
}

QVariant TableMappingModel::sourceFieldTypeNameText(int row) const
{
  if(mappingBase()->itemType(row) != TableMappingItem::FieldMappingType){
    return QVariant();
  }
  return mappingBase()->sourceFieldTypeNameListAtItem(row).join(", ");
}

QVariant TableMappingModel::sourceFixedValue(int row) const
{
  if(mappingBase()->itemType(row) != TableMappingItem::FixedValueType){
    return QVariant();
  }
  return mappingBase()->sourceFixedValueAtItem(row);
}

QVariant TableMappingModel::destinationFieldKeyTypeText(int row) const
{
  QString ktName;
  QStringList ktNameList;
  auto fieldKeyTypeList = mappingBase()->destinationFieldKeyTypeListAtItem(row);

  for(const auto & kt : fieldKeyTypeList){
    ktName = keyTypeName(kt);
    if(!ktName.isEmpty()){
      ktNameList.append(ktName);
    }
  }

  return ktNameList.join(QStringLiteral(", "));
}

QVariant TableMappingModel::destinationFieldNamesText(int row) const
{
  return mappingBase()->destinationFieldNameListAtItem(row).join(QStringLiteral(", "));
}

QVariant TableMappingModel::destinationFieldTypesText(int row) const
{
  return mappingBase()->destinationFieldTypeNameListAtItem(row).join(QStringLiteral(", "));
}

QVariant TableMappingModel::mapItemMappingStateData(int row, int role) const
{
  switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
      return mapItemMappingStateText(mappingBase()->itemMappingState(row));
    case Qt::DecorationRole:
      return mapItemMappingStateDecoration(mappingBase()->itemMappingState(row));
    default:
      return QVariant();
  }
}

QVariant TableMappingModel::mapItemMappingStateText(mdt::sql::copier::TableMappingItemState state) const
{
  switch(state){
    case TableMappingItemState::MappingNotSet:
    case TableMappingItemState::MappingPartial:
      return QVariant();
    case TableMappingItemState::MappingComplete:
      return tr("Ok");
    case TableMappingItemState::MappingError:
      return tr("Error");
  }
  return QVariant();
}

QVariant TableMappingModel::mapItemMappingStateDecoration(mdt::sql::copier::TableMappingItemState state) const
{
  switch(state){
    case TableMappingItemState::MappingNotSet:
      return QVariant();
    case TableMappingItemState::MappingPartial:
      return QColor(Qt::yellow);
    case TableMappingItemState::MappingComplete:
      return QColor(Qt::green);
    case TableMappingItemState::MappingError:
      return QColor(Qt::red);
  }
  return QVariant();
}

QString TableMappingModel::keyTypeName(TableMapping::FieldKeyType type) const
{
  switch(type){
    case TableMapping::NotAKey:
      return QString();
    case TableMapping::PrimaryKey:
      return QStringLiteral("PK");
  }
  return QString();
}

QVariant TableMappingModel::editFlagDecorationData(const QModelIndex & index) const
{
  if(flags(index) & Qt::ItemIsEditable){
    return pvEditableItemIcon;
  }
  return QVariant();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
