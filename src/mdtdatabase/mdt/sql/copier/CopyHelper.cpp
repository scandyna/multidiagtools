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
#include "CopyHelper.h"
#include "../FieldIndexList.h"
#include "mdtSqlTableSchema.h"
#include <QStringBuilder>
#include <algorithm>

namespace mdt{ namespace sql{ namespace copier{

QString CopyHelper::getSourceTableSelectSql(const TableMapping *const tm, const QSqlDatabase & sourceDb)
{
  Q_ASSERT(tm != nullptr);
  Q_ASSERT(sourceDb.isOpen());

  QString sql;
  auto *driver = sourceDb.driver();
  Q_ASSERT(driver != nullptr);

  // Get fields in source table
  auto fieldNameList = tm->getSourceTableFieldNameList();
  for(auto & fn : fieldNameList){
    fn = driver->escapeIdentifier(fn, QSqlDriver::FieldName);
  }
  // Build SQL
  sql = QStringLiteral("SELECT ") \
      % fieldNameList.join(',') \
      % QStringLiteral(" FROM ") \
      % driver->escapeIdentifier(tm->sourceTableName(), QSqlDriver::TableName);

  return sql;
}

// QStringList CopyHelper::getExpressionDestinationFieldNameList(const TableMapping *const tm, const RelatedTableInsertExpression & exp)
// {
//   Q_ASSERT(tm != nullptr);
//   Q_ASSERT(tm->destinationDatabase().isOpen());
// 
//   using mdt::sql::FieldIndexList;
// 
//   QStringList fieldNameList;
//   const auto dfiList = exp.destinationFieldIndexList();
// 
//   fieldNameList.reserve(dfiList.count());
//   for(auto dfi : dfiList){
//     fieldNameList.append(tm->destinationTableFieldNameAt(dfi));
//   }
// 
//   return fieldNameList;
// }

CopyHelperFieldNameLists CopyHelper::getDestinationFieldNameLists(const TableMapping *const tm)
{
  Q_ASSERT(tm != nullptr);

  using mdt::sql::FieldIndexList;

  CopyHelperFieldNameLists lists;

  for(int itemIndex = 0; itemIndex < tm->itemsCount(); ++itemIndex){
    auto tmi = tm->itemAt(itemIndex);
    /// \todo Check mapping state ?
    if(!tmi.isNull()){
      switch(tmi.type()){
        case TableMappingItem::RelatedTableInsertExpressionType:
          lists.rtiExpFieldNameList.append(tm->destinationFieldNameListAtItem(itemIndex));
          break;
        case TableMappingItem::FieldMappingType:
          lists.fieldMapFieldNameList.append(tm->destinationFieldNameListAtItem(itemIndex));
          break;
        case TableMappingItem::FixedValueType:
          lists.fixedValueFieldNameList.append(tm->destinationFieldNameListAtItem(itemIndex));
          break;
      }
    }
  }

  return lists;
}

QString CopyHelper::getDestinationInsertSql(const mdt::sql::copier::TableMapping *const tm, const QSqlDatabase & destinationDb)
{
  Q_ASSERT(tm != nullptr);
  Q_ASSERT(destinationDb.isOpen());

  QString sql;
  auto *driver = destinationDb.driver();
  Q_ASSERT(driver != nullptr);
  QStringList allDestinationFieldNameList;
  QString valuesPlaceHolderString;

  // Get destination field names
  auto destinationFieldNameLists = getDestinationFieldNameLists(tm);
  allDestinationFieldNameList.reserve(destinationFieldNameLists.rtiExpFieldNameList.size() + destinationFieldNameLists.fieldMapFieldNameList.size() + destinationFieldNameLists.fixedValueFieldNameList.size());
  for(const auto & fn : destinationFieldNameLists.rtiExpFieldNameList){
    allDestinationFieldNameList.append(driver->escapeIdentifier(fn, QSqlDriver::FieldName));
  }
  for(const auto & fn : destinationFieldNameLists.fieldMapFieldNameList){
    allDestinationFieldNameList.append(driver->escapeIdentifier(fn, QSqlDriver::FieldName));
  }
  for(const auto & fn : destinationFieldNameLists.fixedValueFieldNameList){
    allDestinationFieldNameList.append(driver->escapeIdentifier(fn, QSqlDriver::FieldName));
  }
  // Build value place holder string
  const int lastIndex = allDestinationFieldNameList.size() - 1;
  if(lastIndex < 0){
    return sql;
  }
  for(int i = 0; i < lastIndex; ++i){
    valuesPlaceHolderString += QStringLiteral("?,");
  }
  valuesPlaceHolderString += QStringLiteral("?");
  // Build SQL
  sql = QStringLiteral("INSERT INTO ") \
      % driver->escapeIdentifier(tm->destinationTableName(), QSqlDriver::TableName) \
      % QStringLiteral(" (") \
      % allDestinationFieldNameList.join(',') \
      % QStringLiteral(") VALUES (") \
      % valuesPlaceHolderString \
      % QStringLiteral(")");

  return sql;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
