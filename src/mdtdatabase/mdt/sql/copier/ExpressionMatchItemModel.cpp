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
#include "ExpressionMatchItemModel.h"
#include "TableMapping.h"

namespace mdt{ namespace sql{ namespace copier{

ExpressionMatchItemModel::ExpressionMatchItemModel(const std::shared_ptr<const TableMapping> & tm, QObject* parent)
 : QAbstractTableModel(parent),
   pvTableMapping(tm)
{
  Q_ASSERT(pvTableMapping);
}

void ExpressionMatchItemModel::setExpressionMatchItemList(const std::vector<ExpressionMatchItem> & itemList)
{
  beginResetModel();
  pvItems = itemList;
  endResetModel();
}

void ExpressionMatchItemModel::clearMatchItemList()
{
  beginResetModel();
  pvItems.clear();
  endResetModel();
}

int ExpressionMatchItemModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvItems.size();
}

int ExpressionMatchItemModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return 4;
}

QVariant ExpressionMatchItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation != Qt::Horizontal){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  if(role != Qt::DisplayRole){
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch(static_cast<ColumnIndex>(section)){
    case OperatorWithPreviousIndex:
      return tr("OP");
    case DestinationFieldNameIndex:
      return destinationFieldHeaderText();
    case MatchOperatorIndex:
      return tr("OP");
    case SourceValueFieldNameIndex:
      return sourceFieldHeaderText();
  }

  return section;
}

QVariant ExpressionMatchItemModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  const int row = index.row();
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvItems.size());
  const auto column = static_cast<ColumnIndex>(index.column());

  if( (role == Qt::DisplayRole) || (role == Qt::EditRole) ){
    // Get match item
    auto item = pvItems[row];
    // Return requested data
    switch(column){
      case OperatorWithPreviousIndex:
        return operatorWithPreviousText(item);
      case DestinationFieldNameIndex:
        return destinationFieldName(item);
      case MatchOperatorIndex:
        return "=";
      case SourceValueFieldNameIndex:
        return sourceValueFieldName(item);
    }
  }

  return QVariant();
}

bool ExpressionMatchItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if(!index.isValid()){
    return false;
  }
  if(role != Qt::DisplayRole){
    return false;
  }
  const int row = index.row();
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < (int)pvItems.size());
  const auto column = static_cast<ColumnIndex>(index.column());

  auto & item = pvItems[row];
  switch(column){
    case DestinationFieldNameIndex:
      setDestinationField(item, value.toString());
      break;
    case SourceValueFieldNameIndex:
      setSourceValueField(item, value.toString());
      break;
    default:
      return false;
  }
  // Update and signal
  ///pvExpression.setMatchItemAt(row, item);
  emit dataChanged(index, index);

  return true;
}

Qt::ItemFlags ExpressionMatchItemModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return QAbstractTableModel::flags(index);
  }
  const auto column = static_cast<ColumnIndex>(index.column());
  switch(column){
    case DestinationFieldNameIndex:
    case SourceValueFieldNameIndex:
      return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    default:
      return QAbstractTableModel::flags(index);
  }
}

bool ExpressionMatchItemModel::insertRows(int row, int count, const QModelIndex & parent)
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return false;
  }
  // We only support inserting 1 item at end
  if(count != 1){
    return false;
  }
  if(row != ((int)pvItems.size()-1) ){
    return false;
  }
  // Insert a item
  beginInsertRows(parent, row, row+count-1);
  if(pvItems.empty()){
    pvItems.emplace_back(-1, -1);
  }else{
    pvItems.emplace_back(mdtSqlWhereOperator::And, -1, -1);
  }
  endInsertRows();

  return true;
}

bool ExpressionMatchItemModel::removeRows(int row, int count, const QModelIndex& parent)
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return false;
  }
  Q_ASSERT((row + count) <= (int)pvItems.size());
  // Remove items
  beginRemoveRows(parent, row, row+count-1);
  auto first = pvItems.cbegin() + row;
  auto last = first + count;
  pvItems.erase(first, last);
  endRemoveRows();

  return true;
}

QString ExpressionMatchItemModel::destinationTableName() const
{
  return pvTableMapping->destinationTableName();
}

QString ExpressionMatchItemModel::sourceTableName() const
{
  return pvTableMapping->sourceTableName();
}

int ExpressionMatchItemModel::destinationFieldCount() const
{
  return pvTableMapping->destinationTableFieldCount();
}

int ExpressionMatchItemModel::sourceFieldCount() const
{
  return pvTableMapping->sourceTableFieldCount();
}

QString ExpressionMatchItemModel::fetchDestinationFieldName(int fieldIndex) const
{
  return pvTableMapping->destinationTableFieldNameAt(fieldIndex);
}

int ExpressionMatchItemModel::fetchDestinationFieldIndexOf(const QString & fieldName) const
{
  return pvTableMapping->destinationTableFieldIndexOf(fieldName);
}

QString ExpressionMatchItemModel::fetchSourceValueFieldName(int fieldIndex) const
{
  return pvTableMapping->sourceTableFieldNameAt(fieldIndex);
}

int ExpressionMatchItemModel::fetchSourceValueFieldIndexOf(const QString & fieldName) const
{
  return pvTableMapping->sourceTableFieldIndexOf(fieldName);
}

QString ExpressionMatchItemModel::destinationFieldHeaderText() const
{
  QString text;
  auto tableName = destinationTableName();

  if(tableName.isEmpty()){
    text = tr("Destination\nfield");
  }else{
    text = tr("Field in\n") + tableName;
  }

  return text;
}

QString ExpressionMatchItemModel::sourceFieldHeaderText() const
{
  QString text;
  auto tableName = sourceTableName();

  if(tableName.isEmpty()){
    text = tr("Source\nfield");
  }else{
    text = tr("Field in\n") + tableName;
  }

  return text;
}

QVariant ExpressionMatchItemModel::operatorWithPreviousText(const ExpressionMatchItem & item) const
{
  if(item.operatorWithPrevious == mdtSqlWhereOperator::Null){
    return QVariant();
  }
  return item.operatorWithPrevious.toString();
}

void ExpressionMatchItemModel::setDestinationField(ExpressionMatchItem & item, const QString & fieldName)
{
  item.destinationFieldIndex = fetchDestinationFieldIndexOf(fieldName);
}

QVariant ExpressionMatchItemModel::destinationFieldName(const ExpressionMatchItem & item) const
{
  const int fieldIndex = item.destinationFieldIndex;
  Q_ASSERT(fieldIndex < destinationFieldCount());

  if(fieldIndex < 0){
    return QVariant();
  }
  return fetchDestinationFieldName(fieldIndex);
}

void ExpressionMatchItemModel::setSourceValueField(ExpressionMatchItem & item, const QString & fieldName)
{
  item.sourceValueFieldIndex = fetchSourceValueFieldIndexOf(fieldName);
}

QVariant ExpressionMatchItemModel::sourceValueFieldName(const ExpressionMatchItem & item) const
{
  const int fieldIndex = item.sourceValueFieldIndex;
  Q_ASSERT(fieldIndex < sourceFieldCount());

  if(fieldIndex < 0){
    return QVariant();
  }
  return fetchSourceValueFieldName(fieldIndex);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
