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
#include "mdtSqlViewSchema.h"

void mdtSqlViewSchema::setName(const QString & name)
{
  pvName = name;
}

void mdtSqlViewSchema::setTableName(const QString & name, const QString & alias)
{
  pvTableName = name;
  pvTableAlias = alias;
}

void mdtSqlViewSchema::setSelectSuffix(mdtSqlViewSchema::SelectSuffix s)
{
  pvSelectSuffix = s;
}

void mdtSqlViewSchema::addSelectItem(const QString& item)
{
  pvSelectList.append(item);
}

void mdtSqlViewSchema::addJoinClause(const mdtSqlViewSchemaJoinClause& jc)
{
  pvJoinClauseList.append(jc);
}

void mdtSqlViewSchema::clear()
{
  pvName.clear();
  pvTableName.clear();
  pvTableAlias.clear();
  pvSelectSuffix = SelectSuffixNone;
  pvSelectList.clear();
  pvJoinClauseList.clear();
}

QString mdtSqlViewSchema::getSqlForDrop(const QSqlDriver* driver) const
{
  Q_ASSERT(driver != nullptr);

  if(pvName.isEmpty()){
    return "";
  }
  return "DROP VIEW IF EXISTS " + driver->escapeIdentifier(pvName, QSqlDriver::TableName);
}

QString mdtSqlViewSchema::getSqlForCreate(const QSqlDriver* driver) const
{
  Q_ASSERT(driver != nullptr);

  QString sql;

  // Check if all is complete
  if( (pvName.isEmpty()) || (pvSelectList.isEmpty()) || (pvTableName.isEmpty()) ){
    return sql;
  }
  // Build header
  sql = "CREATE VIEW " + driver->escapeIdentifier(pvName, QSqlDriver::TableName) + " AS\n";
  // Build SELECT statement
  sql += selectKeyWord() + "\n";
  int lastIndex = pvSelectList.size() - 1;
  Q_ASSERT(lastIndex >= 0);
  for(int i = 0; i < lastIndex; ++i){
    sql += " " + driver->escapeIdentifier(pvSelectList.at(i), QSqlDriver::FieldName) + ",\n";
  }
  sql += " " + driver->escapeIdentifier(pvSelectList.at(lastIndex), QSqlDriver::FieldName) + "\n";
  // Add FROM statement (without anay join)
  sql += "FROM " + driver->escapeIdentifier(pvTableName, QSqlDriver::TableName);
  if(!pvTableAlias.isEmpty()){
    sql += " " + driver->escapeIdentifier(pvTableAlias, QSqlDriver::TableName);
  }
  sql += "\n";
  // Add JOIN clauses
  for(const auto & jc : pvJoinClauseList){
    sql += jc.getSql(driver);
  }

  return sql;
}
