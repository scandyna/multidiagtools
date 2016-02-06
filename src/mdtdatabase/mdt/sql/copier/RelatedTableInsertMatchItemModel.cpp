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
#include "RelatedTableInsertMatchItemModel.h"
#include "TableMapping.h"

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

RelatedTableInsertMatchItemModel::RelatedTableInsertMatchItemModel(const std::shared_ptr<const TableMapping> & tm, QObject* parent)
 : ExpressionMatchItemModel(tm, parent)
{
}

mdtExpected<bool> RelatedTableInsertMatchItemModel::setSourceRelatedTable(const QString & tableName)
{
  if(tableName == pvSourceRelatedTable.tableName()){
    return true;
  }
  pvSourceRelatedTable.clear();
  if(tableName.isEmpty()){
    return true;
  }
  if(tableName == tableMapping()->sourceTableName()){
    return true;
  }
  // Relates to another source table
  Q_ASSERT(tableMapping()->sourceDatabase().isValid());
  Q_ASSERT(tableMapping()->sourceDatabase().isOpen());
  if(!pvSourceRelatedTable.setupFromTable(tableName, tableMapping()->sourceDatabase())){
    return pvSourceRelatedTable.lastError();
  }
  return true;
}

mdtExpected< bool > RelatedTableInsertMatchItemModel::setDestinationRelatedTable(const QString &tableName)
{
  if(tableName == pvDestinationRelatedTable.tableName()){
    return true;
  }
  pvDestinationRelatedTable.clear();
  if(tableName.isEmpty()){
    return true;
  }
  Q_ASSERT(tableMapping()->destinationDatabase().isValid());
  Q_ASSERT(tableMapping()->destinationDatabase().isOpen());
  if(!pvDestinationRelatedTable.setupFromTable(tableName, tableMapping()->destinationDatabase())){
    return pvDestinationRelatedTable.lastError();
  }
  return true;
}

mdtExpected<bool> RelatedTableInsertMatchItemModel::setExpression(const RelatedTableInsertExpression & exp)
{
  auto ret = setSourceRelatedTable(exp.sourceRelatedTableName());
  if(!ret){
    return ret;
  }
  ret = setDestinationRelatedTable(exp.destinationRelatedTableName());
  if(!ret){
    return ret;
  }
  setExpressionMatchItemList(exp.matchItems());

  return true;
}

void RelatedTableInsertMatchItemModel::clear()
{
  clearMatchItemList();
  pvSourceRelatedTable.clear();
  pvDestinationRelatedTable.clear();
}

QStringList RelatedTableInsertMatchItemModel::getSourceRelatedTableFieldNameList() const
{
  if(!pvSourceRelatedTable.tableName().isEmpty()){
    return pvSourceRelatedTable.getFieldNameList();
  }
  return tableMapping()->getSourceTableFieldNameList();
}

QStringList RelatedTableInsertMatchItemModel::getDestinationRelatedTableFieldNameList() const
{
  return pvDestinationRelatedTable.getFieldNameList();
}

QString RelatedTableInsertMatchItemModel::destinationTableName() const
{
  return pvDestinationRelatedTable.tableName();
}

QString RelatedTableInsertMatchItemModel::sourceTableName() const
{
  if(!pvSourceRelatedTable.tableName().isEmpty()){
    return pvSourceRelatedTable.tableName();
  }
  return tableMapping()->sourceTableName();
}

int RelatedTableInsertMatchItemModel::sourceFieldCount() const
{
  if(!pvSourceRelatedTable.tableName().isEmpty()){
    return pvSourceRelatedTable.fieldCount();
  }
  return tableMapping()->sourceTableFieldCount();
}

int RelatedTableInsertMatchItemModel::destinationFieldCount() const
{
  return pvDestinationRelatedTable.fieldCount();
}

QString RelatedTableInsertMatchItemModel::fetchDestinationFieldName(int fieldIndex) const
{
  return pvDestinationRelatedTable.fieldName(fieldIndex);
}

int RelatedTableInsertMatchItemModel::fetchDestinationFieldIndexOf(const QString &fieldName) const
{
  return pvDestinationRelatedTable.fieldIndex(fieldName);
}

QString RelatedTableInsertMatchItemModel::fetchSourceValueFieldName(int fieldIndex) const
{
  if(!pvSourceRelatedTable.tableName().isEmpty()){
    return pvSourceRelatedTable.fieldName(fieldIndex);
  }
  return tableMapping()->sourceTableFieldNameAt(fieldIndex);
}

int RelatedTableInsertMatchItemModel::fetchSourceValueFieldIndexOf(const QString &fieldName) const
{
  if(!pvSourceRelatedTable.tableName().isEmpty()){
    return pvSourceRelatedTable.fieldIndex(fieldName);
  }
  return tableMapping()->sourceTableFieldIndexOf(fieldName);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
