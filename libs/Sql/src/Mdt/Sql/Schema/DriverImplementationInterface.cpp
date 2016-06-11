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
#include "DriverImplementationInterface.h"
#include <QStringList>
#include <QStringBuilder>
#include <QObject>
#include <QSqlQuery>

namespace Mdt{ namespace Sql{ namespace Schema{

FieldTypeList DriverImplementationInterface::getAvailableFieldTypeList() const
{
  FieldTypeList list;

  list.append(FieldType::Boolean);
  list.append(FieldType::Integer);
  list.append(FieldType::Float);
  list.append(FieldType::Double);
  list.append(FieldType::Varchar);
  list.append(FieldType::Date);
  list.append(FieldType::Time);
  list.append(FieldType::DateTime);

  return list;
}

FieldType DriverImplementationInterface::fieldTypeFromQMetaType(QMetaType::Type qmt) const
{
  switch(qmt){
    case QMetaType::Bool:
      return FieldType::Boolean;
    case QMetaType::Int:
      return FieldType::Integer;
    case QMetaType::Float:
      return FieldType::Float;
    case QMetaType::Double:
      return FieldType::Double;
    case QMetaType::QString:
      return FieldType::Varchar;
    case QMetaType::QDate:
      return FieldType::Date;
    case QMetaType::QTime:
      return FieldType::Time;
    case QMetaType::QDateTime:
      return FieldType::DateTime;
    default:
      break;
  }
  return FieldType::UnknownType;
}

QMetaType::Type DriverImplementationInterface::fieldTypeToQMetaType(FieldType ft) const
{
  switch(ft){
    case FieldType::Boolean:
      return QMetaType::Bool;
    case FieldType::Integer:
      return QMetaType::Int;
    case FieldType::Float:
      return QMetaType::Float;
    case FieldType::Double:
      return QMetaType::Double;
    case FieldType::Varchar:
      return QMetaType::QString;
    case FieldType::Date:
      return QMetaType::QDate;
    case FieldType::Time:
      return QMetaType::QTime;
    case FieldType::DateTime:
      return QMetaType::QDateTime;
    case FieldType::UnknownType:
      return QMetaType::UnknownType;
  }
  return QMetaType::UnknownType;
}

QString DriverImplementationInterface::getPrimaryKeyDefinition(const PrimaryKey & pk) const
{
  QString sql;
  const auto rawFieldNameList = pk.fieldNameList();
  QStringList escapedFieldNameList;

  for(const auto & fieldName : rawFieldNameList){
    escapedFieldNameList.append( escapeFieldName(fieldName) );
  }
  sql = QStringLiteral("PRIMARY KEY (") % escapedFieldNameList.join(',') % QStringLiteral(")");

  return sql;
}

QString DriverImplementationInterface::getForeignKeyDefinition(const ForeignKey & fk) const
{
  using Mdt::Sql::Schema::ForeignKey;

  QString sql;
  QStringList parentTableFieldNameList = fk.parentTableFieldNameList();
  QStringList childTableFieldNameList = fk.childTableFieldNameList();

  // Escape field name lists
  for(auto & fieldName : parentTableFieldNameList){
    fieldName = escapeFieldName(fieldName);
  }
  for(auto & fieldName : childTableFieldNameList){
    fieldName = escapeFieldName(fieldName);
  }
  // Build SQL
  sql = QStringLiteral("  FOREIGN KEY (") \
      % childTableFieldNameList.join(',') \
      % QStringLiteral(")\n   REFERENCES ") \
      % escapeTableName(fk.parentTableName()) \
      % QStringLiteral(" (") \
      % parentTableFieldNameList.join(',') \
      % QStringLiteral(")\n") \
      % QStringLiteral("   ON DELETE ") % ForeignKey::actionString(fk.onDeleteAction()) % QStringLiteral("\n") \
      % QStringLiteral("   ON UPDATE ") % ForeignKey::actionString(fk.onUpdateAction());

  return sql;
}

QString DriverImplementationInterface::getSqlToCreateIndex(const Index & index) const
{
  QString sql;
  QStringList fieldNameList = index.fieldNameList();

  // Escape field names
  for(auto & fieldName : fieldNameList){
    fieldName = escapeFieldName(fieldName);
  }
  // Create SQL statement
  if(index.isUnique()){
    sql = QStringLiteral("CREATE UNIQUE INDEX ");
  }else{
    sql = QStringLiteral("CREATE INDEX ");
  }
  sql += escapeTableName(index.name()) \
       % QStringLiteral(" ON ") \
       % escapeTableName(index.tableName()) \
       % QStringLiteral(" (") \
       % fieldNameList.join(',') \
       % QStringLiteral(")");

  return sql;
}

QString DriverImplementationInterface::getSqlToDropIndex(const Index & index) const
{
  QString sql;

  sql = QStringLiteral("DROP INDEX IF EXISTS ") \
      % escapeTableName(index.name());

  return sql;
}

QString DriverImplementationInterface::getSqlToCreateTable(const Table & table) const
{
  Q_ASSERT(!table.isNull());
  Q_ASSERT(table.fieldCount() > 0);

  QString sql;
  int firstFieldIndex = 0;
  const int lastFieldIndex = table.fieldCount() - 1;
  const auto fkList = table.foreignKeyList();

  // Temporary table flag
  if(table.isTemporary()){
    sql = QStringLiteral("CREATE TEMPORARY TABLE ");
  }else{
    sql = QStringLiteral("CREATE TABLE ");
  }
  // Table name
  sql += escapeTableName(table.tableName()) % QStringLiteral(" (\n");
  /*
   * If primary key is a AutoIncrementPrimaryKey or a SingleFieldPrimaryKey
   * we add them as field (else it will be a table constraint)
   */
  switch(table.primaryKeyType()){
    case PrimaryKeyContainer::AutoIncrementPrimaryKeyType:
      sql += QStringLiteral("  ") % getPrimaryKeyFieldDefinition(table.autoIncrementPrimaryKey());
      firstFieldIndex = 1;
      break;
    case PrimaryKeyContainer::SingleFieldPrimaryKeyType:
      sql += QStringLiteral("  ") % getPrimaryKeyFieldDefinition(table.singleFieldPrimaryKey());
      firstFieldIndex = 1;
      break;
    case PrimaryKeyContainer::PrimaryKeyType:
      firstFieldIndex = 0;
      break;
  }
  if(firstFieldIndex == 1){
    if(firstFieldIndex <= lastFieldIndex){
      sql += QStringLiteral(",\n");
    }
  }
  // Add fields (other than AutoIncrementPrimaryKey or SingleFieldPrimaryKey)
  Q_ASSERT(lastFieldIndex >= 0);
  for(int i = firstFieldIndex; i < lastFieldIndex; ++i){
    sql += QStringLiteral("  ") % getFieldDefinition(table.field(i)) % QStringLiteral(",\n");
  }
  if(firstFieldIndex < table.fieldCount()){
    sql += QStringLiteral("  ") % getFieldDefinition(table.field(lastFieldIndex));
  }
  // Add primary key constraint if needed
  if(table.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType){
    const auto pk = table.primaryKey();
    if(pk.fieldCount() > 0){
      sql += QStringLiteral(",\n  ") % getPrimaryKeyDefinition(pk);
    }
  }
  // Add foreign key constraints
  for(const auto & fk : fkList){
    sql += QStringLiteral(",\n") % getForeignKeyDefinition(fk);
  }
  // Add engine

  sql += QStringLiteral("\n);\n");

  return sql;
}

QString DriverImplementationInterface::getSqlToDropTable(const Table& table) const
{
  QString sql;

  sql = QStringLiteral("DROP TABLE IF EXISTS ") % escapeTableName(table.tableName()) % QStringLiteral(";\n");

  return sql;
}

bool DriverImplementationInterface::createTable(const Table& table)
{
  // Create table
  
  // Create indexes
  
  // Create indexes for FK
  
  return false;
}

bool DriverImplementationInterface::dropTable(const Table& table)
{
  return false;
}

QString DriverImplementationInterface::escapeFieldName(const QString & fieldName) const
{
  return qsqlDriver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString DriverImplementationInterface::escapeTableName(const QString & tableName) const
{
  return qsqlDriver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString DriverImplementationInterface::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
