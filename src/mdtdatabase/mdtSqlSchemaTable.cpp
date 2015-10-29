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
#include "mdtSqlSchemaTable.h"
#include "mdtSqlFieldType.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSqlField>
#include <QSqlDriver>

#include <QDebug>

mdtSqlSchemaTable::mdtSqlSchemaTable()
 : pvIsTemporary(false)
{
}

mdtSqlSchemaTable::~mdtSqlSchemaTable()
{
}

void mdtSqlSchemaTable::clear()
{
  pvIsTemporary = false;
  pvDriverType.clear();
  pvDatabaseName.clear();
  pvTableName.clear();
  pvCollation.clear();
  pvStorageEngineName.clear();
  pvFields.clear();
  pvPrimaryKey.clear();
  pvIndexes.clear();
  pvForeignKeys.clear();
}

bool mdtSqlSchemaTable::setDriverName(const QString & name) 
{
  if(!pvDriverType.setType(name)){
    pvLastError.setError("Driver name '" + name + "' is not supported.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  return true;
}

void mdtSqlSchemaTable::setDatabaseName(const QString & name) 
{
  pvDatabaseName = name;
}

void mdtSqlSchemaTable::setTableName(const QString & name, const QString &charset, Qt::CaseSensitivity cs) 
{
  pvTableName = name;
  pvCollation.clear();
  pvCollation.setCharset(charset);
  pvCollation.setCaseSensitive(cs == Qt::CaseSensitive);
}

QString mdtSqlSchemaTable::tableName() const
{
  return pvTableName;
}

void mdtSqlSchemaTable::setTemporary(bool temporary)
{
  pvIsTemporary = temporary;
}

void mdtSqlSchemaTable::setStorageEngineName(const QString & name) 
{
  pvStorageEngineName = name;
}

void mdtSqlSchemaTable::addField(const mdtSqlField & field, bool isPartOfPrimaryKey)
{
  pvFields.append(field);
  if(isPartOfPrimaryKey){
    pvPrimaryKey.addField(field.name());
  }
}

void mdtSqlSchemaTable::addField(const QSqlField & field, bool isPartOfPrimaryKey) 
{
//   pvFields.append(field);
//   if(isPartOfPrimaryKey){
//     pvPrimaryKey.append(field);
//   }
}

QStringList mdtSqlSchemaTable::getFieldNameList() const
{
  QStringList names;

  for(const auto & field : pvFields){
    names.append(field.name());
  }

  return names;
}

mdtSqlField mdtSqlSchemaTable::field(const QString& fieldName) const
{
  for(const auto & field : pvFields){
    if(field.name() == fieldName){
      return field;
    }
  }
  return mdtSqlField();
}

// QSqlField mdtSqlSchemaTable::field(const QString & fieldName) const
// {
//   int i;
// 
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       return pvFields.at(i);
//     }
//   }
// 
//   return QSqlField();
// }

// /*! \todo
//  *  see fieldTypeNameMySql() and fieldTypeNameSqlite() and define all properly.
//  */
// QString mdtSqlSchemaTable::fieldTypeName(int index, mdtSqlDriverType::Type driverType) const
// {
//   Q_ASSERT(index >= 0);
//   Q_ASSERT(index < pvFields.size());
// 
// //   if(pvDriverType.type() == mdtSqlDriverType::Unknown){
// //     return QString();
// //   }
//   ///return mdtSqlFieldType::nameFromType(pvFields.at(index).type(), pvDriverType.type());
//   
//   if(driverType == mdtSqlDriverType::Unknown){
//     return QString();
//   }
//   return mdtSqlFieldType::nameFromType(pvFields.at(index).type(), driverType);
// }

void mdtSqlSchemaTable::addIndex(const mdtSqlIndex & index)
{
  pvIndexes.append(index);
}

void mdtSqlSchemaTable::addForeignKey(const mdtSqlForeignKey & fk)
{
  Q_ASSERT(!fk.parentTableName().isEmpty());
  Q_ASSERT(childFieldsExistsInTable(fk));
  pvForeignKeys.append(fk);
}

void mdtSqlSchemaTable::setForeignKeyAt(int index, const mdtSqlForeignKey& fk)
{
  Q_ASSERT(!fk.parentTableName().isEmpty());
  Q_ASSERT(childFieldsExistsInTable(fk));
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFields.size());
  pvForeignKeys[index] = fk;
}

mdtSqlForeignKey mdtSqlSchemaTable::foreignKeyReferencing(const QString & parentTableName) const
{
  mdtSqlForeignKey fk;

  for(const auto & _fk : pvForeignKeys){
    if(_fk.parentTableName() == parentTableName){
      fk = _fk;
      fk.setChildTableName(pvTableName);
      return fk;
    }
  }

  return fk;
}

void mdtSqlSchemaTable::addIndex(const QString & name, bool unique) 
{
//   if(pvIndexes.contains(name)){
//     return;
//   }
//   pvIndexes.insert(name, QSqlIndex("", name));
//   pvIndexeAtIsUnique.insert(name, unique);
}

bool mdtSqlSchemaTable::addFieldToIndex(const QString & indexName, const QString & fieldName) 
{
//   QSqlField field;
//   int i;
// 
//   if(!pvIndexes.contains(indexName)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to index '" + indexName\
//                           + "' : index not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       field = pvFields.at(i);
//       break;
//     }
//   }
//   if(field.name().isEmpty()){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to index '" + indexName\
//                           + "' : field not found in table.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   pvIndexes[indexName].append(field);
// 
//   return true;
}

void mdtSqlSchemaTable::addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate) 
{
//   mdtSqlSchemaTableForeignKeyInfo info;
// 
//   if(pvForeignKeys.contains(name)){
//     return;
//   }
//   info.referingTableName = referingTableName;
//   info.actionOnDelete = actionOnDelete;
//   info.actionOnUpdate = actionOnUpdate;
//   pvForeignKeys.insert(name, info);
}

bool mdtSqlSchemaTable::addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & referingFieldName) 
{
//   QSqlField field, referingField;
//   int i;
// 
//   if(!pvForeignKeys.contains(foreignKeyName)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to foreign key '" + foreignKeyName\
//                           + "' : foreign key not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       field = pvFields.at(i);
//     }
//   }
//   if(field.name().isEmpty()){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to foreign key '" + foreignKeyName\
//                           + "' : field not found in table.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   referingField.setName(referingFieldName);
//   pvForeignKeys[foreignKeyName].fields.append(field);
//   pvForeignKeys[foreignKeyName].referingFields.append(referingField);
// 
//   return true;
}

bool mdtSqlSchemaTable::updateForeignKeyReferingTable(const QString & name, const QString & referingTableName)
{
//   if(!pvForeignKeys.contains(name)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot update refering table for foreign key '" + name\
//                           + "' : foreign key not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   pvForeignKeys[name].referingTableName = referingTableName;
// 
//   return true;
}

bool mdtSqlSchemaTable::setupFromTable(const QString & name, QSqlDatabase db)
{
  Q_ASSERT(db.isOpen());

  ///QSqlRecord record;
  ///int i;

  clear();
  // Check that requested table exists
  if(!db.tables(QSql::AllTables).contains(name)){
    pvLastError.setError("Table '" + name + "' not found.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Get record that contains field informations
  /**
  record = db.record(name);
  if(record.isEmpty()){
    pvLastError.setError("Table '" + name + "' not found.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  */
  // Set table name
  pvTableName = name;
//   if(!setDriverName(db.driverName())){
//     return false;
//   }
  // Add fields
  if(!setupFieldsFromDatabase(db)){
    return false;
  }
  // Set primary key
  pvPrimaryKey.setupFromQSqlIndex(db.primaryIndex(name));
  // Add other indexes
  if(!setupIndexesFromDatabase(db)){
    return false;
  }
  // Add foreign keys
  if(!setupForeignKeysFromDatabase(db)){
    return false;
  }

  return true;
}

QStringList mdtSqlSchemaTable::getSqlForCreateTable(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QStringList sqlList;
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  bool pkInFieldDefinition;
  int lastFieldIndex = pvFields.size() - 1;

  // Check if we have requiered info
  if(pvTableName.isEmpty()){
    return sqlList;
  }
  if(pvFields.isEmpty()){
    return sqlList;
  }
  // Create table beginning part
  if(isTemporary()){
    sql = "CREATE TEMPORARY TABLE ";
  }else{
    sql = "CREATE TABLE ";
  }
  if(!pvDatabaseName.isEmpty()){
    sql += driver->escapeIdentifier(pvDatabaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + " (\n";
  // Add fields
  Q_ASSERT(lastFieldIndex >= 0);
  for(int i = 0; i < lastFieldIndex; ++i){
    pkInFieldDefinition = includePrimaryKeyDefinitionInField(pvFields.at(i).name());
    sql += "  " + pvFields.at(i).getSql(db, pkInFieldDefinition) + ",\n";
  }
  pkInFieldDefinition = includePrimaryKeyDefinitionInField(pvFields.at(lastFieldIndex).name());
  sql += "  " + pvFields.at(lastFieldIndex).getSql(db, pkInFieldDefinition);
  // Add primary key constraint if needed
  if( (!pkInFieldDefinition) && (pvPrimaryKey.fieldCount() > 1) ){
    sql += ",\n  " + pvPrimaryKey.getSql(db);
  }
  // Add foreign key constraints
  for(const auto & fk : pvForeignKeys){
    sql += ",\n" + fk.getSqlForForeignKey(db);
  }
  // Add engine
  if(pvStorageEngineName.isEmpty()){
    sql += "\n);\n";
  }else{
    switch(mdtSqlDriverType::typeFromName(db.driverName())){
      case mdtSqlDriverType::Unknown:
      case mdtSqlDriverType::SQLite:
        sql += "\n);\n";
        break;
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::MariaDB:
        sql += "\n) ENGINE=" + pvStorageEngineName + ";\n";
        break;
    }
  }
  sqlList.append(sql);
  // Add indexes
  for(auto index : pvIndexes){
    index.setTableName(pvTableName);
    index.generateName();
    sql = index.getSqlForCreate(db, pvDatabaseName) + ";\n";
    sqlList.append(sql);
  }
  // Add indexes for foreign keys that request to create indexes
  for(auto fk : pvForeignKeys){
    if(fk.createParentIndex()){
      sql = fk.getSqlForCreateParentTableIndex(db, pvDatabaseName) + ";\n";
      sqlList.append(sql);
    }
    if(fk.createChildIndex()){
      fk.setChildTableName(pvTableName);
      sql = fk.getSqlForCreateChildTableIndex(db, pvDatabaseName) + ";\n";
      sqlList.append(sql);
    }
  }

  return sqlList;
}

QStringList mdtSqlSchemaTable::getSqlForDropTable(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QStringList sqlList;
  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  // Check if we have requiered info
  if(pvTableName.isEmpty()){
    return sqlList;
  }
  // Build SQL
  sql = "DROP TABLE IF EXISTS ";
  if(!pvDatabaseName.isEmpty()){
    sql += driver->escapeIdentifier(pvDatabaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + ";\n";
  sqlList.append(sql);
  // Add indexes
  for(auto index : pvIndexes){
    index.setTableName(pvTableName);
    index.generateName();
    sql = index.getSqlForDrop(db, pvDatabaseName) +  + ";\n";
    sqlList.append(sql);
  }
  // Add indexes for foreign keys that request to create indexes
  for(auto fk : pvForeignKeys){
    if(fk.createParentIndex()){
      sql = fk.getSqlForDropParentTableIndex(db, pvDatabaseName) + ";\n";
      sqlList.append(sql);
    }
    if(fk.createChildIndex()){
      fk.setChildTableName(pvTableName);
      sql = fk.getSqlForDropChildTableIndex(db, pvDatabaseName) + ";\n";
      sqlList.append(sql);
    }
  }

  return sqlList;
}

QString mdtSqlSchemaTable::sqlForCreateTable()
{
  QString sql;

//   // Check if we have requiered info
//   if(pvTableName.trimmed().isEmpty()){
//     return sql;
//   }
//   if(pvFields.isEmpty()){
//     return sql;
//   }
//   // Add the create table statement regarding driver
//   switch(pvDriverType.type()){
//     case mdtSqlDriverType::MariaDB:
//     case mdtSqlDriverType::MySQL:
//       sql += sqlForCreateTableMySql();
//       break;
//     case mdtSqlDriverType::SQLite:
//       sql += sqlForCreateTableSqlite();
//       break;
//     case mdtSqlDriverType::Unknown:
//       pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//       pvLastError.commit();
//       break;
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForDropTable() const
{
  QString sql;
//   QString delimiter;

//   if(pvTableName.isEmpty()){
//     return "";
//   }
//   switch(pvDriverType.type()){
//     case mdtSqlDriverType::MariaDB:
//     case mdtSqlDriverType::MySQL:
//       delimiter = "`";
//       break;
//     case mdtSqlDriverType::SQLite:
//       delimiter = "'";
//       break;
//     case mdtSqlDriverType::Unknown:
//       break;
//   }
//   if(pvDatabaseName.trimmed().isEmpty()){
//     sql = "DROP TABLE IF EXISTS " + delimiter + pvTableName + delimiter + ";\n";
//   }else{
//     sql = "DROP TABLE IF EXISTS " + delimiter + pvDatabaseName + delimiter + "." + delimiter + pvTableName + delimiter + ";\n";
//   }

  return sql;
}

bool mdtSqlSchemaTable::includePrimaryKeyDefinitionInField(const QString & fieldName) const
{
  auto pk = pvPrimaryKey;

  return ( (pk.fieldCount() == 1) && (pk.fieldName(0) == fieldName) );
}

bool mdtSqlSchemaTable::childFieldsExistsInTable(const mdtSqlForeignKey & fk)
{
  for(const auto & childTableFieldName : fk.childTableFields()){
    if(fieldIndex(childTableFieldName) >= 0){
      return true;
    }
  }
  return false;
}

mdtSqlSchemaTable::foreignKeyAction_t mdtSqlSchemaTable::foreignKeyActionFromName(const QString & name) const
{
//   if(name.toUpper() == "CASCADE"){
//     return Cascade;
//   }
//   if(name.toUpper() == "SET NULL"){
//     return SetNull;
//   }
//   if(name.toUpper() == "RESTRICT"){
//     return Restrict;
//   }
//   return NoAction;
}

bool mdtSqlSchemaTable::setupFieldsFromDatabase(const QSqlDatabase & db)
{
  mdtSqlDriverType::Type driverType = mdtSqlDriverType::typeFromName(db.driverName());

  switch(driverType){
    case mdtSqlDriverType::Unknown:
      return false;
    case mdtSqlDriverType::SQLite:
      return setupFieldsFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
      break;
  }
  QSqlRecord record = db.record(pvTableName);
  int i;
  Q_ASSERT(!record.isEmpty());
  for(i = 0; i < record.count(); ++i){
    mdtSqlField field;
    field.setupFromQSqlField(record.field(i), driverType);
    pvFields.append(field);
  }
  return true;
}

bool mdtSqlSchemaTable::setupFieldsFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlRecord dbRecord = db.record(pvTableName);
  ///QSqlRecord record;
  QSqlQuery query(db);
  QString sql;
  ///QSqlField field;
  QString fieldTypeName;

  Q_ASSERT(!dbRecord.isEmpty());

  // Get infos from database
  sql = "PRAGMA table_info(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get informations for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Add fields
  while(query.next()){
    mdtSqlField field;
    mdtSqlFieldType ft;
    auto tableInfoRecord = query.record();
    // Most of setup can be done via Qt's driver
    field.setupFromQSqlField(dbRecord.field(tableInfoRecord.value("name").toString()), mdtSqlDriverType::SQLite);
    // Try to map field type from name
    fieldTypeName = tableInfoRecord.value("type").toString().toUpper();
    if(!fieldTypeName.isEmpty()){
      ft = mdtSqlFieldType::typeFromName(fieldTypeName, mdtSqlDriverType::SQLite);
      if(!ft.isNull()){
        field.setType(ft.type());
        field.setLength(ft.length());
      }
    }
    // If field type is one that we want to handle, set it
//     if(fieldTypeName == "DATE"){
//       field.setType(mdtSqlFieldType::Date);
//     }else if(fieldTypeName == "TIME"){
//       field.setType(mdtSqlFieldType::Time);
//     }else if(fieldTypeName == "DATETIME"){
//       field.setType(mdtSqlFieldType::DateTime);
//     }
    // Add field
    pvFields.append(field);
  }

  return true;
}

bool mdtSqlSchemaTable::setupIndexesFromDatabase(const QSqlDatabase & db)
{
  switch(mdtSqlDriverType::typeFromName(db.driverName())){
    case mdtSqlDriverType::SQLite:
      return setupIndexesFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }
  pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
  pvLastError.commit();
  return false;
}

bool mdtSqlSchemaTable::setupIndexesFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlQuery query(db);
  QString sql;
  QSqlRecord record;
//   QString indexName;
//   QStringList indexList;
  

  // Get index list
  sql = "PRAGMA index_list(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get list of indexes for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Add indexes
  while(query.next()){
    mdtSqlIndex index;
    record = query.record();
    index.setName(record.value("name").toString());
    index.setUnique(record.value("unique").toInt() == 1);
    index.setTableName(pvTableName);
    pvIndexes.append(index);
//     indexName = record.value("name").toString();
//     addIndex(indexName, record.value("unique").toBool());
//     indexList.append(indexName);
  }
  // Add fields for each indexes
  for(auto & index : pvIndexes){
    sql = "PRAGMA index_info(" + index.name() + ")";
    if(!query.exec(sql)){
      QSqlError sqlError = query.lastError();
      pvLastError.setError("Cannot get index info for index '" + index.name() + "' in table '" + pvTableName + "'", mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
      pvLastError.commit();
      return false;
    }
    while(query.next()){
      record = query.record();
      index.addField(record.value("name").toString());
    }
  }
//   for(int i = 0; i < indexList.size(); ++i){
//     indexName = indexList.at(i);
//     sql = "PRAGMA index_info(" + indexName + ")";
//     if(!query.exec(sql)){
//       QSqlError sqlError = query.lastError();
//       pvLastError.setError("Cannot get index info for index '" + indexName + "' in table '" + pvTableName + "'", mdtError::Error);
//       pvLastError.setSystemError(sqlError.number(), sqlError.text());
//       MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//       pvLastError.commit();
//       return false;
//     }
//     while(query.next()){
//       record = query.record();
//       if(!addFieldToIndex(indexName, record.value("name").toString())){
//         return false;
//       }
//     }
//   }

  return true;
}

bool mdtSqlSchemaTable::setupForeignKeysFromDatabase(const QSqlDatabase & db)
{
  switch(mdtSqlDriverType::typeFromName(db.driverName())){
    case mdtSqlDriverType::SQLite:
      return setupForeignKeysFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }
  pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
  pvLastError.commit();
  return false;
}

bool mdtSqlSchemaTable::setupForeignKeysFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlQuery query(db);
  QString sql;
  QSqlRecord record;
  mdtSqlForeignKey fk;
  QMap<int, mdtSqlForeignKey> fkMap;
//   int lastFkId;
  ///QString fkName;

  // Get foreign key list
  sql = "PRAGMA foreign_key_list(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get list of foreign keys for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Build foreign keys
  while(query.next()){
    record = query.record();
    int fkId = record.value("id").toInt();
    // Check if a FK must be created or just updated
    if(fkMap.contains(fkId)){
      fkMap[fkId].addKeyFields(record.value("to").toString(), record.value("from").toString());
    }else{
      fk.clear();
      fk.setParentTableName(record.value("table").toString());
      fk.setChildTableName(pvTableName);
      fk.setOnDeleteAction(record.value("on_delete").toString());
      fk.setOnUpdateAction(record.value("on_update").toString());
      fk.addKeyFields(record.value("to").toString(), record.value("from").toString());
      fkMap.insert(fkId, fk);
    }
  }
  // Add foreign keys
  pvForeignKeys = fkMap.values();
  
//   // Add foreign keys
//   lastFkId = -1;
//   while(query.next()){
//     record = query.record();
//     
//     qDebug() << record;
//     
//     int fkId = record.value("id").toInt();
//     
//     qDebug() << "Id: " << fkId << ", last ID: " << lastFkId;
//     
//     // Setup FK common
//     if(fkId != lastFkId){
//       
//       qDebug() << " - base setup ...";
//       ///pvForeignKeys.append(fk);
//       
//       fk.clear();
//       fk.setParentTableName(record.value("table").toString());
//       fk.setChildTableName(pvTableName);
//       fk.setOnDeleteAction(record.value("on_delete").toString());
//       fk.setOnUpdateAction(record.value("on_update").toString());
//     }
//     // Add field
//     
//     qDebug() << " - add field ...";
//     
//     fk.addKeyFields(record.value("to").toString(), record.value("from").toString());
//     
// //     lastFkId = fkId;
//     // Add FK to list
//     if(fkId != lastFkId){
//       
//       qDebug() << " - add FK to list";
//       
//       pvForeignKeys.append(fk);
//       lastFkId = fkId;
//     }
//     
//     fkName = record.value("from").toString() + "_fk";
//     addForeignKey(fkName, record.value("table").toString(), foreignKeyActionFromName(record.value("on_delete").toString()), foreignKeyActionFromName(record.value("on_update").toString()) );
//     if(!addFieldToForeignKey(fkName, record.value("from").toString(), record.value("to").toString())){
//       return false;
//     }
//   }
  ///pvForeignKeys.append(fk);

  return true;
}
