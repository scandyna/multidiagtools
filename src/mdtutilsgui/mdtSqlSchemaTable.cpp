/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtError.h"

#include <QDebug>

mdtSqlSchemaTable::mdtSqlSchemaTable()
{
}

mdtSqlSchemaTable::~mdtSqlSchemaTable()
{
}

void mdtSqlSchemaTable::clear()
{
  pvDriverName.clear();
  pvDatabaseName.clear();
  pvTableName.clear();
  pvCharset.clear();
  pvCaseSensitivity = Qt::CaseInsensitive;
  pvStorageEngineName.clear();
  pvFields.clear();
  pvPrimaryKey.clear();
  pvIndexes.clear();
  pvIndexeAtIsUnique.clear();
  pvForeignKeys.clear();
}

void mdtSqlSchemaTable::setDriverName(const QString & name) 
{
  pvDriverName = name;
}

void mdtSqlSchemaTable::setDatabaseName(const QString & name) 
{
  pvDatabaseName = name;
}

void mdtSqlSchemaTable::setTableName(const QString & name, const QString &charset, Qt::CaseSensitivity cs) 
{
  pvTableName = name;
  pvCharset = charset.toLower();
  pvCaseSensitivity = cs;
}

QString mdtSqlSchemaTable::tableName() const
{
  return pvTableName;
}

void mdtSqlSchemaTable::setStorageEngineName(const QString & name) 
{
  pvStorageEngineName = name;
}

void mdtSqlSchemaTable::addField(const QSqlField & field, bool isPartOfPrimaryKey) 
{
  pvFields.append(field);
  if(isPartOfPrimaryKey){
    pvPrimaryKey.append(field);
  }
}

int mdtSqlSchemaTable::fieldCount() const
{
  return pvFields.size();
}

void mdtSqlSchemaTable::addIndex(const QString & name, bool unique) 
{
  if(pvIndexes.contains(name)){
    return;
  }
  pvIndexes.insert(name, QSqlIndex("", name));
  pvIndexeAtIsUnique.insert(name, unique);
}

bool mdtSqlSchemaTable::addFieldToIndex(const QString & indexName, const QString & fieldName) 
{
  QSqlField field;
  int i;

  if(!pvIndexes.contains(indexName)){
    return false;
  }
  for(i = 0; i < pvFields.size(); ++i){
    if(pvFields.at(i).name() == fieldName){
      field = pvFields.at(i);
      break;
    }
  }
  if(field.name().isEmpty()){
    return false;
  }
  pvIndexes[indexName].append(field);

  return true;
}

void mdtSqlSchemaTable::addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate) 
{
  mdtSqlSchemaTableForeignKeyInfo info;

  if(pvForeignKeys.contains(name)){
    return;
  }
  info.referingTableName = referingTableName;
  info.actionOnDelete = actionOnDelete;
  info.actionOnUpdate = actionOnUpdate;
  pvForeignKeys.insert(name, info);
}

bool mdtSqlSchemaTable::addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & referingFieldName) 
{
  QSqlField field, referingField;
  int i;

  if(!pvForeignKeys.contains(foreignKeyName)){
    mdtError e("Foreign key named " + foreignKeyName + " not found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
    e.commit();
    return false;
  }
  for(i = 0; i < pvFields.size(); ++i){
    if(pvFields.at(i).name() == fieldName){
      field = pvFields.at(i);
    }
    /**
    if(pvFields.at(i).name() == referingFieldName){
      referingField = pvFields.at(i);
    }
    */
  }
  if(field.name().isEmpty()){
    mdtError e("Field named " + fieldName + " not found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
    e.commit();
    return false;
  }
  /**
  if(referingField.name().isEmpty()){
    mdtError e("Refering field named " + referingFieldName + " not found", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
    e.commit();
    return false;
  }
  */
  referingField.setName(referingFieldName);
  pvForeignKeys[foreignKeyName].fields.append(field);
  pvForeignKeys[foreignKeyName].referingFields.append(referingField);

  return true;
}

QString mdtSqlSchemaTable::sqlForCreateTable()  const
{
  QString sql;

  // Check if we have requiered info
  if(pvTableName.trimmed().isEmpty()){
    return sql;
  }
  if(pvFields.isEmpty()){
    return sql;
  }
  // Add the create table statement regarding driver
  if(pvDriverName == "QMYSQL"){
    sql += sqlForCreateTableMySql();
  }else if(pvDriverName == "QSQLITE"){
    sql += sqlForCreateTableSqlite();
  }else{
    sql = "";
    mdtError e(MDT_DATABASE_ERROR, "Unknown driver name '" + pvDriverName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
    e.commit();
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForDropTable() const
{
  QString sql;
  QString delimiter;

  if(pvTableName.isEmpty()){
    return "";
  }
  if(pvDriverName == "QMYSQL"){
    delimiter = "`";
  }else{
    delimiter = "'";
  }
  if(pvDatabaseName.trimmed().isEmpty()){
    sql = "DROP TABLE IF EXISTS " + delimiter + pvTableName + delimiter + ";\n";
  }else{
    sql = "DROP TABLE IF EXISTS " + delimiter + pvDatabaseName + delimiter + "." + delimiter + pvTableName + delimiter + ";\n";
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForCreateTableMySql() const
{
  QString sql;

  if(pvDatabaseName.trimmed().isEmpty()){
    sql = "CREATE TABLE `" + pvTableName + "` (\n";
  }else{
    sql = "CREATE TABLE `" + pvDatabaseName + "`.`" + pvTableName + "` (\n";
  }
  // Add fields
  sql += sqlForFieldsMySql();
  if((pvPrimaryKey.isEmpty())&&(pvIndexes.isEmpty())&&(pvForeignKeys.isEmpty())){
    sql += "\n";
  }
  // Add primary key
  sql += sqlForPrimaryKey("`");
  // Add indexes
  sql += sqlForIndexesMySql();
  // Add foreign keys
  sql += sqlForForeignKeys("`");
  // Add end + table attributes
  sql += "\n)";
  if(!pvStorageEngineName.trimmed().isEmpty()){
    sql += " ENGINE=" + pvStorageEngineName;
  }
  if(!pvCharset.trimmed().isEmpty()){
    sql += " DEFAULT CHARSET=" + pvCharset;
  }
  if(pvDriverName.trimmed() == "QMYSQL"){
    sql += sqlForCollateMySql();
  }
  sql += ";\n";

  return sql;
}

QString mdtSqlSchemaTable::sqlForCreateTableSqlite() const
{
  QString sql;
  QSqlField field;

  if(pvDatabaseName.trimmed().isEmpty()){
    sql = "CREATE TABLE '" + pvTableName + "' (\n";
  }else{
    sql = "CREATE TABLE '" + pvDatabaseName + "'.'" + pvTableName + "' (\n";
  }
  // Add fields
  sql += sqlForFieldsSqlite();
  if((pvPrimaryKey.isEmpty())&&(pvIndexes.isEmpty())&&(pvForeignKeys.isEmpty())){
    sql += "\n";
  }
  // Add primary key
  if(!((pvPrimaryKey.count() == 1)&&(pvPrimaryKey.field(0).type() == QVariant::Int)&&(pvPrimaryKey.field(0).isAutoValue()))){
    sql += sqlForPrimaryKey("'");
  }
  // Add indexes
  sql += sqlForIndexesSqlite();
  // Add foreign keys
  sql += sqlForForeignKeys("'");
  // Add end + table attributes
  sql += "\n);\n";

  return sql;
}

QString mdtSqlSchemaTable::sqlForFieldsMySql() const
{
  QString sql;
  QSqlField field;
  int i;

  for(i = 0; i < pvFields.size(); ++i){
    field = pvFields.at(i);
    sql += "  `" + field.name() + "` ";
    sql += fieldTypeNameMySql(field.type(), field.length());
    if((field.requiredStatus() == QSqlField::Required)||(pvPrimaryKey.contains(field.name()))){
      sql += " NOT NULL";
    }
    if((pvPrimaryKey.contains(field.name()))&&(field.isAutoValue())){
      sql += " AUTO_INCREMENT";
    }else{
      if(field.defaultValue().isNull()){
        sql += " DEFAULT NULL";
      }else{
        sql += " DEFAULT " + field.defaultValue().toString();
      }
      /**
      if(field.requiredStatus() != QSqlField::Required){
        if(field.defaultValue().isNull()){
          sql += " DEFAULT NULL";
        }else{
          sql += " DEFAULT " + field.defaultValue().toString();
        }
      }
      */
    }
    if(i < (pvFields.size() - 1)){
      sql += ",\n";
    }
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForFieldsSqlite() const
{
  QString sql;
  QSqlField field;
  int i;

  for(i = 0; i < pvFields.size(); ++i){
    field = pvFields.at(i);
    sql += "  '" + field.name() + "' ";
    // Auto increment is only accepted for one case and needs a special syntax in Sqlite
    if((pvPrimaryKey.count() == 1)&&(field.type() == QVariant::Int)&&(field.isAutoValue())&&(pvPrimaryKey.contains(field.name()))){
      sql += "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT";
    }else{
      sql += fieldTypeNameSqlite(field.type(), field.length());
      if((field.requiredStatus() == QSqlField::Required)||(pvPrimaryKey.contains(field.name()))){
        sql += " NOT NULL";
      }
      if(field.defaultValue().isNull()){
        sql += " DEFAULT NULL";
      }else{
        sql += " DEFAULT " + field.defaultValue().toString();
      }
      /**
      if(field.requiredStatus() != QSqlField::Required){
        if(field.defaultValue().isNull()){
          sql += " DEFAULT NULL";
        }else{
          sql += " DEFAULT " + field.defaultValue().toString();
        }
      }
      */
    }
    if(field.type() == QVariant::String){
      sql += sqlForCollateSqlite();
    }
    if(i < (pvFields.size() - 1)){
      sql += ",\n";
    }
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForPrimaryKey(const QString &delimiter) const
{
  QString sql;
  int i;

  if(pvPrimaryKey.count() > 0){
    sql += ",\n";
    sql += "  PRIMARY KEY (";
    for(i = 0; i < pvPrimaryKey.count(); ++i){
      sql += delimiter + pvPrimaryKey.field(i).name() + delimiter;
      if(i < (pvPrimaryKey.count() - 1)){
        sql += ",";
      }
    }
    sql += ")";
  }

  return sql;
}

/**
QString mdtSqlSchemaTable::sqlForIndexes()
{
  if(pvDriverName.trimmed() == "QMYSQL"){
    return sqlForIndexesMySql();
  }
  if(pvDriverName.trimmed() == "QSQLITE"){
    return sqlForIndexesSqlite();
  }
  return "";
}
*/

QString mdtSqlSchemaTable::sqlForIndexesMySql() const
{
  QString sql;
  QSqlIndex index;
  int i;

  if(pvIndexes.isEmpty()){
    return "";
  }
  sql = ",\n";
  QHashIterator<QString, QSqlIndex> it(pvIndexes);
  while(it.hasNext()){
    it.next();
    index = it.value();
    if(pvIndexeAtIsUnique.value(index.name())){
      sql += "  UNIQUE INDEX `" + index.name() + "` (";
    }else{
      sql += "  INDEX `" + index.name() + "` (";
    }
    for(i = 0; i < index.count(); ++i){
      sql += "`" + index.field(i).name() + "`";
      if(i < (index.count() - 1)){
        sql += ",";
      }
    }
    if(it.hasNext()){
      sql += "),\n";
    }else{
      sql += ")";
    }
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForIndexesSqlite() const
{
  QString sql, indexSql;
  QSqlIndex index;
  int i;

  if(pvIndexes.isEmpty()){
    return "";
  }
  QHashIterator<QString, QSqlIndex> it(pvIndexes);
  while(it.hasNext()){
    it.next();
    index = it.value();
    if(pvIndexeAtIsUnique.value(index.name())){
      indexSql += "  UNIQUE (";
      for(i = 0; i < index.count(); ++i){
        indexSql += "'" + index.field(i).name() + "'";
        if(i < (index.count() - 1)){
          indexSql += ",";
        }
      }
      if(it.hasNext()){
        indexSql += "),\n";
      }else{
        indexSql += ")";
      }
    }
  }
  if(!indexSql.trimmed().isEmpty()){
    sql = ",\n" + indexSql;
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForForeignKeys(const QString &delimiter) const
{
  QString sql;
  QSqlField field;
  QSqlRecord fields;
  mdtSqlSchemaTableForeignKeyInfo fkInfo;
  int i;

  if(pvForeignKeys.isEmpty()){
    return "";
  }
  sql = ",\n";
  QHashIterator<QString, mdtSqlSchemaTableForeignKeyInfo> itf(pvForeignKeys);
  while(itf.hasNext()){
    itf.next();
    fkInfo = itf.value();
    if(pvDriverName.trimmed() == "QSQLITE"){
      sql += "  FOREIGN KEY (";
    }else{
      sql += "  FOREIGN KEY " + delimiter + itf.key() + delimiter + " (";
    }
    fields = fkInfo.fields;
    for(i = 0; i < fields.count(); ++i){
      sql += delimiter + fields.field(i).name() + delimiter;
      if(i < (fields.count() - 1)){
        sql += ",";
      }
    }
    sql += ")\n";
    sql += "   REFERENCES " + delimiter + fkInfo.referingTableName + delimiter + " (";
    fields = fkInfo.referingFields;
    for(i = 0; i < fields.count(); ++i){
      sql += delimiter + fields.field(i).name() + delimiter;
      if(i < (fields.count() - 1)){
        sql += ",";
      }
    }
    sql += ")\n";
    sql += "   ON DELETE " + foreignKeyActionName((foreignKeyAction_t)fkInfo.actionOnDelete) + "\n";
    sql += "   ON UPDATE " + foreignKeyActionName((foreignKeyAction_t)fkInfo.actionOnUpdate);
    if(itf.hasNext()){
      sql += ",\n";
    }
  }

  return sql;
}

QString mdtSqlSchemaTable::fieldTypeNameMySql(QVariant::Type type, int length) const
{
  QString strType;

  switch(type){
    case QVariant::Int:
      strType = "INT";
      break;
    case QVariant::UInt:
      strType = "INT";
      break;
    case QVariant::LongLong:
      strType = "BIGINT";
      break;
    case QVariant::ULongLong:
      strType = "BIGINT";
      break;
    case QVariant::Double:
      strType = "DOUBLE PRECISION";
      break;
    case QVariant::Bool:
      strType = "BOOL";
      break;
    case QVariant::Date:
      strType = "DATE";
      break;
    case QVariant::Time:
      strType = "TIME";
      break;
    case QVariant::DateTime:
      strType = "DATETIME";
      break;
    case QVariant::String:
      strType = "VARCHAR";
      break;
    case QVariant::Bitmap:
      strType = "BLOB";
      break;
    case QVariant::Image:
      strType = "BLOB";
      break;
    default:
      mdtError e(MDT_DATABASE_ERROR, "Unknown mapping for type " + QString::number(type) + " (QVariant::Type)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
      e.commit();
  }
  // Add length if defined (We not check if length is supported for given type)
  if(length > 0){
    strType += "(" + QString::number(length) + ")";
  }
  // Add the UNSIGNED suffix if type is unsigned
  if((type == QVariant::UInt)||(type == QVariant::ULongLong)){
    strType += " UNSIGNED";
  }

  return strType;
}

QString mdtSqlSchemaTable::fieldTypeNameSqlite(QVariant::Type type, int length) const
{
  QString strType;

  switch(type){
    case QVariant::Int:
      strType = "INT";
      break;
    case QVariant::UInt:
      strType = "INT";
      break;
    case QVariant::LongLong:
      strType = "BIGINT";
      break;
    case QVariant::ULongLong:
      strType = "BIGINT";
      break;
    case QVariant::Double:
      strType = "DOUBLE";
      break;
    case QVariant::Bool:
      strType = "BOOLEAN";
      break;
    case QVariant::Date:
      strType = "DATE";
      break;
    case QVariant::Time:
      strType = "DATETIME";
      break;
    case QVariant::DateTime:
      strType = "DATETIME";
      break;
    case QVariant::String:
      strType = "VARCHAR";
      break;
    case QVariant::Bitmap:
      strType = "BLOB";
      break;
    case QVariant::Image:
      strType = "BLOB";
      break;
    default:
      mdtError e(MDT_DATABASE_ERROR, "Unknown mapping for type " + QString::number(type) + " (QVariant::Type)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
      e.commit();
  }
  // Add length if defined (We not check if length is supported for given type)
  if(length > 0){
    strType += "(" + QString::number(length) + ")";
  }
  // Add the UNSIGNED suffix if type is unsigned
  if((type == QVariant::UInt)||(type == QVariant::ULongLong)){
    strType += " UNSIGNED";
  }

  return strType;
}

QString mdtSqlSchemaTable::sqlForCollateMySql() const
{
  QString sql;

  if(pvCharset.trimmed().isEmpty()){
    return sql;
  }
  sql = " COLLATE=";
  // Set first part regarding charset
  if(pvCharset.toLower() == "utf8"){
    sql += "utf8_unicode";
  }
  // Set case sensitivity
  switch(pvCaseSensitivity){
    case Qt::CaseInsensitive:
      sql += "_ci";
      break;
    case Qt::CaseSensitive:
      sql += "_cs";
      break;
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForCollateSqlite() const
{
  switch(pvCaseSensitivity){
    case Qt::CaseInsensitive:
      return " COLLATE NOCASE";
    case Qt::CaseSensitive:
      return " COLLATE BINARY";
      break;
  }

  return "";
}

QString mdtSqlSchemaTable::foreignKeyActionName(foreignKeyAction_t action) const
{
  switch(action){
    case Cascade:
      return "CASCADE";
    case SetNull:
      return "SET NULL";
    case NoAction:
      return "NO ACTION";
    case Restrict:
      return "RESTRICT";
  }
  return "";
}
