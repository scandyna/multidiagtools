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
#include <QSqlDriver>
#include <QLatin1String>

//#include <QDebug>

using namespace mdtSqlViewSchema;

void SelectField::clear()
{
  pvName.clear();
  pvAlias.clear();
  pvTable.clear();
}

QString SelectField::getSql(const QSqlDriver * const driver) const
{
  Q_ASSERT(driver != nullptr);

  QString sql;

  if(pvName.isEmpty()){
    return sql;
  }
  if(!pvTable.isEmpty()){
    sql = driver->escapeIdentifier(pvTable, QSqlDriver::TableName) + ".";
  }
  if(pvName == "*"){
    sql += "*";
  }else{
    sql += driver->escapeIdentifier(pvName, QSqlDriver::FieldName);
  }
  if(!pvAlias.isEmpty()){
    sql += " AS " + driver->escapeIdentifier(pvAlias, QSqlDriver::FieldName);
  }

  return sql;
}


void Table::clear()
{
  pvTableName.clear();
  pvAlias.clear();
}

// QString Table::getSql(const QSqlDriver * const driver) const
// {
//   Q_ASSERT(driver != nullptr);
// 
//   
// }


void JoinKey::clear()
{
  pvMainTableField.clear();
  pvConstraintOperator = On;
  pvTableToJoinField.clear();
  pvFieldOperator = Equal;
}

QString JoinKey::constraintOperatorStr() const
{
  switch(pvConstraintOperator){
    case On:
      return "ON";
    case And:
      return "AND";
    case Or:
      return "OR";
  }
  return QString();
}

QString JoinKey::fieldOperatorStr() const
{
  switch(pvFieldOperator){
    case Equal:
      return "=";
    case MtfLessThanTdjf:
      return "<";
    case MtfGreaterThanTdjf:
      return ">";
  }
  return QString();
}

void JoinClause::clear()
{
  pvOperator = Join;
  pvMainTable.clear();
  pvTableToJoin.clear();
  pvKeyList.clear();
}

/// Try QT_USE_FAST_OPERATOR_PLUS

/// See also QLatin1Char

/// Or, fall back to +=

QString JoinClause::getSql(const QSqlDriver * const driver) const
{
  Q_ASSERT(driver != nullptr);

  QString sql;

  if(isNull()){
    return sql;
  }
  sql = " " + operatorStr() + " " + driver->escapeIdentifier(pvTableToJoin.tableName(), QSqlDriver::TableName);
  if(!pvTableToJoin.alias().isEmpty()){
    sql += " " + driver->escapeIdentifier(pvTableToJoin.alias(), QSqlDriver::TableName);
  }
  for(const auto & key : pvKeyList){
    sql += QLatin1String("\n  ") + key.constraintOperatorStr()
         + QLatin1String(" ") + driver->escapeIdentifier(pvMainTable.aliasOrTableName(), QSqlDriver::TableName)
         + QLatin1String(".") + driver->escapeIdentifier(key.mainTableField(), QSqlDriver::FieldName)
         + QLatin1String(" ") + key.fieldOperatorStr()
         + QLatin1String(" ") + driver->escapeIdentifier(pvTableToJoin.aliasOrTableName(), QSqlDriver::TableName)
         + QLatin1String(".") + driver->escapeIdentifier(key.tableToJoinField(), QSqlDriver::FieldName);
  }

  return sql;
}

QString JoinClause::operatorStr() const
{
  switch(pvOperator){ /// Try QStringLiteral
    case Join:
      return "JOIN";
    case LeftJoin:
      return "LEFT JOIN";
  }
  return QString();
}


void Schema::setName(const QString & name)
{
  pvName = name;
}

void Schema::setTable(const Table & table)
{
  pvTable = table;
}

void Schema::addSelectField(const Table & table, SelectField field)
{
  if(table.alias().isEmpty()){
    field.setTable(table.tableName());
  }else{
    field.setTable(table.alias());
  }
  pvSelectFieldList.append(field);
}

void Schema::setTableName(const QString & name, const QString & alias)
{
  pvTable.setTableName(name);
  pvTable.setAlias(alias);
}

void Schema::setSelectSuffix(Schema::SelectSuffix s)
{
  pvSelectSuffix = s;
}

void Schema::addJoinClause(const JoinClause& join)
{
  pvJoinClauseList.append(join);
}

// void Schema::addSelectItem(const QString& item)
// {
//   pvSelectList.append(item);
// }

// void Schema::addJoinClause(const mdtSqlViewSchemaJoinClause& jc)
// {
//   pvJoinClauseList.append(jc);
// }

void Schema::clear()
{
  pvSelectSuffix = SelectSuffixNone;
  pvName.clear();
  pvTable.clear();
  pvSelectFieldList.clear();
}

QString Schema::getSqlForDrop(const QSqlDriver* driver) const
{
  Q_ASSERT(driver != nullptr);

  if(pvName.isEmpty()){
    return "";
  }
  return "DROP VIEW IF EXISTS " + driver->escapeIdentifier(pvName, QSqlDriver::TableName);
}

QString Schema::getSqlForCreate(const QSqlDriver* driver) const
{
  Q_ASSERT(driver != nullptr);

  QString sql;

  // Check if all is complete
  if( (pvName.isEmpty()) || (pvSelectFieldList.isEmpty()) || (pvTable.tableName().isEmpty()) ){
    return sql;
  }
  // Build header
  sql = "CREATE VIEW " + driver->escapeIdentifier(pvName, QSqlDriver::TableName) + " AS\n";
  // Build SELECT statement
  sql += selectKeyWord() + "\n";
  int lastIndex = pvSelectFieldList.size() - 1;
  Q_ASSERT(lastIndex >= 0);
  for(int i = 0; i < lastIndex; ++i){
    sql += " " + driver->escapeIdentifier(pvSelectFieldList.at(i).getSql(driver), QSqlDriver::FieldName) + ",\n";
  }
  sql += " " + driver->escapeIdentifier(pvSelectFieldList.at(lastIndex).getSql(driver), QSqlDriver::FieldName) + "\n";
  // Add FROM statement (without anay join)
  sql += "FROM " + driver->escapeIdentifier(pvTable.tableName(), QSqlDriver::TableName);
  if(!pvTable.alias().isEmpty()){
    sql += " " + driver->escapeIdentifier(pvTable.alias(), QSqlDriver::TableName);
  }
  // Add JOIN clauses
  for(const auto & join : pvJoinClauseList){
    sql += "\n" + join.getSql(driver);
  }

  return sql;
}
