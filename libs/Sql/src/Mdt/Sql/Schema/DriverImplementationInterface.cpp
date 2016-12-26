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
#include "ForeignKeyActionSqlTransform.h"
#include "Mdt/Sql/Error.h"
#include "../InsertQuery.h"
#include "ViewSqlTransform.h"
#include <QStringList>
#include <QStringBuilder>
#include <QLatin1String>
#include <QObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

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

FieldType DriverImplementationInterface::fieldTypeFromString(const QString & fieldTypeString) const
{
  Q_ASSERT(!fieldTypeString.isEmpty());

  const QStringList parts = fieldTypeString.split('(', QString::SkipEmptyParts);
  Q_ASSERT(!parts.isEmpty());
  const QString tn = parts.at(0).trimmed().toUpper();

  if(tn == QLatin1String("BOOL") || tn == QLatin1String("BOOLEAN")){
    return FieldType::Boolean;
  }else if(tn == QLatin1String("INT") || tn == QLatin1String("INTEGER")){
    return FieldType::Integer;
  }else if(tn == QLatin1String("FLOAT")){
    return FieldType::Float;
  }else if(tn == QLatin1String("DOUBLE")){
    return FieldType::Double;
  }else if(tn == QLatin1String("VARCHAR")){
    return FieldType::Varchar;
  }else if(tn == QLatin1String("DATE")){
    return FieldType::Date;
  }else if(tn == QLatin1String("TIME")){
    return FieldType::Time;
  }else if(tn == QLatin1String("DATETIME")){
    return FieldType::DateTime;
  }
  return FieldType::UnknownType;
}

int DriverImplementationInterface::fieldLengthFromString(const QString & fieldTypeString) const
{
  Q_ASSERT(!fieldTypeString.isEmpty());

  const QStringList parts = fieldTypeString.split('(', QString::SkipEmptyParts);
  Q_ASSERT(!parts.isEmpty());

  if(parts.size() < 2){
    return -1;
  }
  const QString sizeStr = parts.at(1).trimmed().remove(')');
  bool ok;
  int length = sizeStr.toInt(&ok);
  if(!ok){
    QString msg = tr("Parsing length in '%1' failed.").arg(fieldTypeString);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.commit();
    setLastError(error);
    return -2;
  }

  return length;
}

Mdt::Expected<FieldList> DriverImplementationInterface::getTableFieldListFromDatabase(const QString & tableName) const
{
  Mdt::Expected<FieldList> ret;
  FieldList fieldList;

  auto record = database().record(tableName);
  for(int i = 0; i < record.count(); ++i){
    Field field;
    QSqlField qtField = record.field(i);
    field.setName( qtField.name() );
    field.setType( fieldTypeFromQVariantType(qtField.type()) );
    field.setRequired( (qtField.requiredStatus() == QSqlField::Required) );
    field.setLength( qtField.length() );
    field.setDefaultValue( qtField.defaultValue() );
    fieldList.append(field);
  }
  ret = fieldList;

  return ret;
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
      % QStringLiteral("   ON DELETE ") % ForeignKeyActionSqlTransform::getSql(fk.onDeleteAction()) % QStringLiteral("\n") \
      % QStringLiteral("   ON UPDATE ") % ForeignKeyActionSqlTransform::getSql(fk.onUpdateAction());

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

bool DriverImplementationInterface::createTable(const Table & table)
{
  QSqlQuery query(mDatabase);
  const QString tableName = table.tableName();
  const auto indexList = table.indexList();
  const auto fkList = table.foreignKeyList();
  QString sql;

  // Create table
  sql = getSqlToCreateTable(table);
  if(!query.exec(sql)){
    QString msg = tr("Creating table '%1' failed.").arg(tableName);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }
  // Create indexes
  for(const auto & index : indexList){
    sql = getSqlToCreateIndex(index);
    if(!query.exec(sql)){
      QString msg = tr("Creating index '%1' for table '%2' failed.").arg(index.name()).arg(tableName);
      auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
      error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
      error.commit();
      setLastError(error);
      return false;
    }
  }
  // Create indexes for FK
  for(const auto & fk : fkList){
    if(fk.isIndexed()){
      auto index = fk.getChildTableIndex();
      sql = getSqlToCreateIndex(index);
      if(!query.exec(sql)){
        QString msg = tr("Creating index '%1' for table '%2' failed.").arg(index.name()).arg(tableName);
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
        error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
        error.commit();
        setLastError(error);
        return false;
      }
    }
  }

  return true;
}

bool DriverImplementationInterface::dropTable(const Table& table)
{
  /*
   * Note: we assume that DBMS will automatically drop indexes related to table.
   *       SQLite tells it explicitly in the documentation.
   */
  QSqlQuery query(mDatabase);
  QString sql;

  sql = getSqlToDropTable(table);
  if(!query.exec(sql)){
    QString msg = tr("Removing table '%1' failed.").arg(table.tableName());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

bool DriverImplementationInterface::createView(const View & view)
{
  QSqlQuery query(mDatabase);
  QString sql;

  sql = ViewSqlTransform::getSqlToCreateView(view, mDatabase);
  if(!query.exec(sql)){
    QString msg = tr("Creating view '%1' failed.").arg(view.name());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

bool DriverImplementationInterface::dropView(const View & view)
{
  QSqlQuery query(mDatabase);
  QString sql;

  sql = ViewSqlTransform::getSqlToDropView(view, mDatabase);
  if(!query.exec(sql)){
    QString msg = tr("Removing view '%1' failed.").arg(view.name());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

QString DriverImplementationInterface::getSqlToCreateTrigger(const Trigger& trigger) const
{
  QString sql;

  if(trigger.isTemporary()){
    sql = QStringLiteral("CREATE TEMPORARY TRIGGER ");
  }else{
    sql = QStringLiteral("CREATE TRIGGER ");
  }
  sql += escapeTableName(trigger.name()) % QStringLiteral(" ") \
       % triggerEventKeyWord(trigger.event()) % QStringLiteral(" ON ") % escapeTableName(trigger.tableName()) \
       % QStringLiteral("\nFOR EACH ROW\nBEGIN\n") \
       % trigger.script() \
       % QStringLiteral("\nEND;");

  return sql;
}

bool DriverImplementationInterface::createTrigger(const Trigger& trigger)
{
  QSqlQuery query(mDatabase);
  QString sql;

  sql = getSqlToCreateTrigger(trigger);
  if(!query.exec(sql)){
    QString msg = tr("Creating trigger '%1' failed.").arg(trigger.name());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

bool DriverImplementationInterface::dropTrigger(const Trigger& trigger)
{
  QSqlQuery query(mDatabase);
  QString sql;

  sql = getSqlToDropTrigger(trigger);
  if(!query.exec(sql)){
    QString msg = tr("Removing trigger '%1' failed.").arg(trigger.name());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverImplementationInterface");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverImplementationInterface"));
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

QString DriverImplementationInterface::getSqlToDropTrigger(const Trigger& trigger) const
{
  QString sql;

  sql = QStringLiteral("DROP TRIGGER IF EXISTS ") % escapeTableName(trigger.name()) % QStringLiteral(";");

  return sql;
}

bool DriverImplementationInterface::populateTable(const TablePopulation & tp)
{
  InsertQuery query(database());
  const int rowCount = tp.rowCount();
  const int columnCount = tp.fieldcount();

  for(int row = 0; row < rowCount; ++row){
    query.setTableName(tp.tableName());
    for(int col = 0; col < columnCount; ++col){
      query.addValue(FieldName(tp.fieldName(col)), tp.data(row, col));
    }
    if(!query.exec()){
      setLastError(query.lastError());
      return false;
    }
    query.clear();
  }

  return true;
}

bool DriverImplementationInterface::createSchema(const Schema & schema)
{
  const auto tableList = schema.tableList();
  const auto viewList = schema.viewList();
  const auto triggerList = schema.triggerList();
  const auto tablePopulationList = schema.tablePopulationList();

  /// \todo use transactions !

  // Create tables
  for(const auto & table : tableList){
    if(!createTable(table)){
      return false;
    }
  }
  // Create views
  for(const auto & view : viewList){
    if(!createView(view)){
      return false;
    }
  }
  // Create triggers
  for(const auto & trigger : triggerList){
    if(!createTrigger(trigger)){
      return false;
    }
  }
  // Populate tables
  for(const auto & tp : tablePopulationList){
    if(!populateTable(tp)){
      return false;
    }
  }

  return true;
}

bool DriverImplementationInterface::dropSchema(const Schema & schema)
{
  /*
   * Note: currently, we get back tables, views, etc.. from schema.
   * In the future, we could implement functions that can drop things by name..
   */
  const auto tableList = schema.tableList();
  const auto viewList = schema.viewList();

  /// \todo use transactions !

  // Drop views
  for(const auto & view : viewList){
    if(!dropView(view)){
      return false;
    }
  }
  // Drop triggers ??
  
  // Drop tables
  /// \todo see if some DBMS do not automatically drop data.
  for(const auto & table : tableList){
    if(!dropTable(table)){
      return false;
    }
  }

  return true;
}

QString DriverImplementationInterface::escapeFieldName(const QString & fieldName) const
{
  return qsqlDriver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString DriverImplementationInterface::escapeTableName(const QString & tableName) const
{
  return qsqlDriver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString DriverImplementationInterface::escapeFieldDefaultValue(const Field & field) const
{
  if(field.type() == FieldType::Varchar){
    return escapeFieldName(field.defaultValue().toString());
  }
  return field.defaultValue().toString();
}

QString DriverImplementationInterface::triggerEventKeyWord(Trigger::Event event) const
{
  switch(event){
    case Trigger::Unknown:
      return QStringLiteral("");
    case Trigger::AfterInsert:
      return QStringLiteral("AFTER INSERT");
  }
  return QString();
}

QString DriverImplementationInterface::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
