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
#include "DriverSQLite.h"
#include "FieldTypeName.h"
#include "Mdt/Sql/Error.h"
#include "Mdt/Algorithm.h"
#include <QStringBuilder>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <vector>

#include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

DriverSQLite::DriverSQLite(const QSqlDatabase& db)
 : DriverImplementationInterface(db)
{
  Q_ASSERT(qsqlDriver()->dbmsType() == QSqlDriver::SQLite);
}

QString DriverSQLite::getCollationDefinition(const Collation & collation) const
{
  if(collation.isNull()){
    return QString();
  }
  /*
   * SQLite has no locale support,
   * only case sensitivity for our case.
   * Note: in current version, we ignore RTRIM functionnality
   */
  if(collation.isCaseSensitive()){
    return QStringLiteral("COLLATE BINARY");
  }else{
    return QStringLiteral("COLLATE NOCASE");
  }
}

QString DriverSQLite::getFieldDefinition(const Field & field) const
{
  QString sql;

  // Field name and type
  sql = escapeFieldName(field.name()) % QStringLiteral(" ") % FieldTypeName::nameFromType( field.type() );
  // Length
  if(field.length() > 0){
    sql += QStringLiteral("(") % QString::number(field.length()) % QStringLiteral(")");
  }
  // Unique constraint
  if(field.isUnique()){
    sql += QStringLiteral(" UNIQUE");
  }
  // Null constraint
  if(field.isRequired()){
    sql += QStringLiteral(" NOT NULL");
  }
  // Default value
  if(field.defaultValue().isNull()){
    sql += QStringLiteral(" DEFAULT NULL");
  }else{
    sql += QStringLiteral(" DEFAULT ") + escapeFieldDefaultValue(field);
  }
  // Collation
  if(!field.collation().isNull()){
    sql += QStringLiteral(" ") % getCollationDefinition(field.collation());
  }

  return sql;
}

Expected< FieldList > DriverSQLite::getTableFieldListFromDatabase(const QString& tableName) const
{
  using Mdt::Sql::Schema::Field;

  Mdt::Expected<FieldList> ret;/// = DriverImplementationInterface::getTableFieldListFromDatabase(tableName);
  FieldList fieldList;

  QSqlQuery query(database());
  const QString sql = QStringLiteral("PRAGMA table_info(") % escapeTableName(tableName) % QStringLiteral(")");
  if(!query.exec(sql)){
    QString msg = tr("Fetching informations of table '%1' failed.").arg(tableName);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverSQLite");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverSQLite"));
    error.commit();
    setLastError(error);
    return ret;
  }
  while(query.next()){
    Field field;
    // Fetch field name
    field.setName(query.value("name").toString());
    // Fetch field type and length
    const QString fieldTypeString = query.value("type").toString();
    if(!fieldTypeString.isEmpty()){
      field.setType( fieldTypeFromString(fieldTypeString) );
      int length = fieldLengthFromString(fieldTypeString);
      if(length < -1){
        ret = lastError();
        return ret;
      }
      field.setLength(length);
    }
    // Fetch not null
    field.setRequired( query.value("notnull") == QVariant(1) );
    // Default value
    field.setDefaultValue( fieldDefaultValue(query.value("dflt_value")) );
    // Add to list
    fieldList.append(field);
  }
  ret = fieldList;

  return ret;
}

QString DriverSQLite::getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const
{
  QString sql;

  sql = escapeFieldName(pk.fieldName()) \
      % QStringLiteral(" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT");

  return sql;
}

QString DriverSQLite::getPrimaryKeyFieldDefinition(const SingleFieldPrimaryKey& pk) const
{
  QString sql;

  // Field name and type
  sql = escapeFieldName(pk.fieldName()) % QStringLiteral(" ") % FieldTypeName::nameFromType( pk.fieldType() );
  // Length
  if(pk.fieldLength() > 0){
    sql += QStringLiteral("(") % QString::number(pk.fieldLength()) % QStringLiteral(")");
  }
  // Primary key constraint
  sql += QStringLiteral(" NOT NULL PRIMARY KEY");
  // Collation
  if(!pk.collation().isNull()){
    sql += QStringLiteral(" ") % getCollationDefinition(pk.collation());
  }

  return sql;
}

Mdt::Expected<PrimaryKeyContainer> DriverSQLite::getTablePrimaryKeyFromDatabase(const QString & tableName) const
{
  Mdt::Expected<PrimaryKeyContainer> ret;
  PrimaryKeyContainer pk;
  QSqlQuery query(database());
  std::vector<QSqlRecord> pkFieldList;

  const QString sql = QStringLiteral("PRAGMA table_info(") % escapeTableName(tableName) % QStringLiteral(")");
  if(!query.exec(sql)){
    QString msg = tr("Fetching primary key for table '%1' failed.").arg(tableName);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverSQLite");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverSQLite"));
    error.commit();
    setLastError(error);
    return ret;
  }
  // Collect fields that are part of primary key
  while(query.next()){
    if(query.value("pk").toInt() > 0){
      pkFieldList.push_back(query.record());
    }
  }
  // Deduce and build primary key
  if(pkFieldList.size() == 1){
    const QString fieldTypeName = pkFieldList[0].value("type").toString().trimmed().toUpper();
    const QString fieldName = pkFieldList[0].value("name").toString();
    /*
     * If field is of type INTEGER (not INT !),
     * we habe a INTEGER PRIMARY KEY which is a auto increment primary key
     * Note: type can be INTEGRE or INTEGER AUTO INCREMENT, which is the same.
     */
    if(fieldTypeName.startsWith(QLatin1String("INTEGER"))){
      AutoIncrementPrimaryKey k;
      k.setFieldName(fieldName);
      pk.setPrimaryKey(k);
    }else{
      SingleFieldPrimaryKey k;
      k.setFieldName(fieldName);
      k.setFieldType( fieldTypeFromString(fieldTypeName) );
      int length = fieldLengthFromString(fieldTypeName);
      if(length < -1){
        ret = lastError();
        return ret;
      }
      k.setFieldLength(length);
      pk.setPrimaryKey(k);
    }
  }else{
    // We have a multi column primary key
    PrimaryKey k;
    for(unsigned int i = 0; i < pkFieldList.size(); ++i){
      k.addFieldName( pkFieldList[i].value("name").toString() );
    }
    pk.setPrimaryKey(k);
  }
  ret = pk;

  return ret;
}

Mdt::Expected<ForeignKeyList> DriverSQLite::getTableForeignKeyListFromDatabase(const QString & tableName) const
{
  Mdt::Expected<ForeignKeyList> ret;
  QMap<int, ForeignKey> fkMap;
  QSqlQuery query(database());

  const QString sql = QStringLiteral("PRAGMA foreign_key_list(") % escapeTableName(tableName) % QStringLiteral(")");
  if(!query.exec(sql)){
    QString msg = tr("Fetching foreign keys for table '%1' failed.").arg(tableName);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverSQLite");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverSQLite"));
    error.commit();
    setLastError(error);
    return ret;
  }
  while(query.next()){
    int fkId = query.value("id").toInt();
    ParentTableFieldName parentField( query.value("to").toString() );
    ChildTableFieldName childField( query.value("from").toString() );
    // Check if a FK must be created or just updated
    if(fkMap.contains(fkId)){
      fkMap[fkId].addKeyFields(parentField, childField);
    }else{
      ForeignKey fk;
      fk.setParentTableName( query.value("table").toString() );
      fk.setChildTableName( tableName );
      fk.setOnDeleteAction( ForeignKey::actionFromString( query.value("on_delete").toString() ) );
      fk.setOnUpdateAction( ForeignKey::actionFromString( query.value("on_update").toString() ) );
      fk.addKeyFields(parentField, childField);
      fkMap.insert(fkId, fk);
    }
  }
  ForeignKeyList fkList;
  for(const auto & fk : fkMap){
    fkList.append(fk);
  }
  ret = fkList;

  return ret;
}

Mdt::Expected<IndexList> DriverSQLite::getTableIndexListFromDatabase(const QString& tableName) const
{
  using Mdt::Sql::Schema::Index;

  Mdt::Expected<IndexList> ret;
  IndexList indexList;
  QSqlQuery query(database());

  const QString sql = QStringLiteral("PRAGMA index_list(") % escapeTableName(tableName) % QStringLiteral(")");
  if(!query.exec(sql)){
    QString msg = tr("Fetching list of indexes for table '%1' failed.").arg(tableName);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverSQLite");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverSQLite"));
    error.commit();
    setLastError(error);
    return ret;
  }
  while(query.next()){
    if(query.value("origin") == QVariant("c")){
      Index index;
      index.setTableName(tableName);
      index.setName( query.value("name").toString() );
      if(!addColumnsToIndex(index)){
        ret = lastError();
        return ret;
      }
      indexList.append(index);
    }
  }
  ret = indexList;

  return ret;
}

bool DriverSQLite::addColumnsToIndex(Index & index) const
{
  QSqlQuery query(database());

  const QString sql = QStringLiteral("PRAGMA index_info(") % escapeTableName(index.name()) % QStringLiteral(")");
  if(!query.exec(sql)){
    QString msg = tr("Fetching information for index '%1' failed.").arg(index.name());
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverSQLite");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverSQLite"));
    error.commit();
    setLastError(error);
    return false;
  }
  while(query.next()){
    index.addFieldName(query.value("name").toString());
  }

  return true;
}

QVariant DriverSQLite::fieldDefaultValue(const QVariant & v) const
{
  QVariant dv;

  if(v.isNull()){
    return dv;
  }
  QString ds = v.toString().trimmed();
  // Check about NULL flag
  if(ds == QLatin1String("NULL")){
    return dv;
  }
  dv = Mdt::Algorithm::removeFirstLastCharIf(ds, '"');

  return dv;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
