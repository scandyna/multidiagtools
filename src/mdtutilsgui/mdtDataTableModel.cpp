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
#include "mdtDataTableModel.h"
#include "mdtError.h"
#include <QFileInfo>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

mdtDataTableModel::mdtDataTableModel(QObject *parent, QSqlDatabase db)
 : QSqlTableModel(parent, db)
{
}

mdtDataTableModel::~mdtDataTableModel()
{
}

bool mdtDataTableModel::setDataSetDirectory(const QDir &dir)
{
  if(!dir.exists()){
    mdtError e(MDT_FILE_IO_ERROR, "Directory not found, path: " + dir.absolutePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    e.commit();
    return false;
  }
  pvDataSetDirectory = dir;

  return true;
}

QDir mdtDataTableModel::dataSetDirectory() const
{
  return pvDataSetDirectory;
}

QSqlDatabase mdtDataTableModel::createDataSet(const QDir &dir, const QString &name, const QList<QSqlField> &fields, const QSqlIndex &primaryKey, create_mode_t mode)
{
  QFile file;
  QFileInfo fileInfo;
  QSqlDatabase db;
  QString cnnName;  // Connection name
  QString dbName;   // Database name
  QString tableName;
  int i;

  // Set names
  cnnName = name;
  fileInfo.setFile(dir, name);
  dbName = fileInfo.absoluteFilePath() + ".db";
  tableName = mdtDataTableModel::getTableName(name);

  // We check that data set directory exists
  if(!dir.exists()){
    mdtError e(MDT_FILE_IO_ERROR, "Directory not found, path: " + dir.absolutePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    e.commit();
    return db;
  }
  // Check if we are allready connected
  db = QSqlDatabase::database(cnnName);
  if(db.isOpen()){
    // Check that we are connected to the correct database
    if(db.databaseName() != dbName){
      mdtError e(MDT_DATABASE_ERROR, "A connection to dataset " + name + " exists, but contains wrong database", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
      e.commit();
      db.close();
      return db;
    }
  }else{
    // Try to open/connect
    db = QSqlDatabase::addDatabase("QSQLITE", cnnName);
    if(!db.isValid()){
      mdtError e(MDT_DATABASE_ERROR, "Cannot create database connection (probably plugin/driver problem)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
      e.commit();
      return db;
    }
    db.setDatabaseName(dbName);
    if(!db.open()){
      mdtError e(MDT_DATABASE_ERROR, "Cannot open database " + dbName, mdtError::Error);
      e.setSystemError(db.lastError().number(), db.lastError().text());
      MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
      e.commit();
      ///QSqlDatabase::removeDatabase(cnnName);
      return db;
    }
  }
  // Here, we are connected
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());
  // Check if table exists
  qDebug() << "TABLES: " << db.tables();
  if(db.tables().contains(tableName)){
    switch(mode){
      case OverwriteExisting:
        // If we cannot drop expected table, it's possibly a other file
        if(!mdtDataTableModel::dropDatabaseTable(tableName, db)){
          db.close();
          ///QSqlDatabase::removeDatabase(cnnName);
          return db;
        }
        break;
      case KeepExisting:
        // Nothing else to do
        return db;
      case FailIfExists:
        db.close();
        ///QSqlDatabase::removeDatabase(cnnName);
        return db;
      case AskUserIfExists:
        /// \todo Implement this !
        break;
    }
  }
  // Here, we must create database table
  if(!createDatabaseTable(tableName, fields, db)){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create database table for data set " + name, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    e.commit();
    db.close();
    ///QSqlDatabase::removeDatabase(cnnName);
  }
  // Finished
  return db;

  
  
  // 1: tester si connection existe
  
  // 1: test si rép. existe
  
  // 2: test si fichier existe
  
  // 3: test si db existe. Considéerer connection déjà ouverte
  
  
  // Check if connection to database allready exists
  db = QSqlDatabase::database(name);
  if(db.isOpen()){
    // Check create mode
    switch(mode){
      case OverwriteExisting:
        // If we cannot drop expected table, it's possibly a other file
        if(!mdtDataTableModel::dropDatabaseTable(name, db)){
          db.close();
          QSqlDatabase::removeDatabase(name);
          return db;
        }
        break;
      case KeepExisting:
        // Nothing to do
        return db;
      case FailIfExists:
        db.close();
        QSqlDatabase::removeDatabase(name);
        return db;
      case AskUserIfExists:
        /// \todo Implement this !
        break;
    }
    db.close();
    QSqlDatabase::removeDatabase(name);
  }else{
    // Check if database file exists
    fileInfo.setFile(dir, name);
    qDebug() << "DB file path: " << fileInfo.absoluteFilePath();
    if(fileInfo.exists()){
      if(!fileInfo.isFile()){
        mdtError e(MDT_FILE_IO_ERROR, "Given path is not a file: path " + fileInfo.absoluteFilePath(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
        e.commit();
        return db;
      }
      if(!fileInfo.isWritable()){
        mdtError e(MDT_FILE_IO_ERROR, "Cannot write to database file: path " + fileInfo.absoluteFilePath(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
        e.commit();
        return db;
      }
      // Check create mode
      switch(mode){
        case OverwriteExisting:
          // Clear existing file
          file.setFileName(fileInfo.absoluteFilePath());
          if(!file.open(QIODevice::Truncate | QIODevice::WriteOnly)){
            {
            mdtError e(MDT_FILE_IO_ERROR, "Cannot erase database file: path " + fileInfo.absoluteFilePath(), mdtError::Error);
            MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
            e.commit();
            }
            return db;
          }
          file.close();
          break;
        case KeepExisting:
          // Nothing to do
          break;
        case FailIfExists:
          return db;
        case AskUserIfExists:
          /// \todo Implement this !
          break;
      }
    }
    // Create database connection and object
    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(fileInfo.absoluteFilePath());
    if(!db.open()){
      {
      mdtError e(MDT_DATABASE_ERROR, "Cannot create database: name " + db.databaseName(), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
      e.setSystemError(db.lastError().number(), db.lastError().text());
      e.commit();
      }
      QSqlDatabase::removeDatabase(name);
      return db;
    }
    // Create table
    if(mode == OverwriteExisting){
      if(!createDatabaseTable(name, fields, db)){
        mdtError e(MDT_DATABASE_ERROR, "Cannot create database table: data set: " + fileInfo.absoluteFilePath(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
        e.commit();
        db.close();
        QSqlDatabase::removeDatabase(name);
        return db;
      }
    }
  }



}

bool mdtDataTableModel::addRow(const QMap<QString,QVariant> &data, int role)
{
}

bool mdtDataTableModel::setData(int row, int column, const QVariant &value, int role)
{
}

bool mdtDataTableModel::setData(int row, const QString & field, const QVariant & value, int role)
{
}

bool mdtDataTableModel::exportToCsvFile(const QString & filePath)
{
  return false;
}

bool mdtDataTableModel::importFromCsvFile(const QString & filePath)
{
  return false;
}

QString mdtDataTableModel::getTableName(const QString &dataSetName)
{
  QString tableName;

  tableName = dataSetName.trimmed();
  tableName.replace(".", "_");
  tableName += "_tbl";

  return tableName;
}

bool mdtDataTableModel::createDatabaseTable(const QString &tableName, const QList<QSqlField> &fields, const QSqlDatabase &db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());

  QString sql;
  int i;

  // Build SQL query
  sql = "CREATE TABLE " + tableName + "(\n";
  for(i=0; i<fields.size(); i++){
    qDebug() << "Field: " << fields.at(i).name();
    sql += fields.at(i).name() + " ";
    switch(fields.at(i).type()){
      case QVariant::Int:
        sql += " INTEGER ";
        break;
      case QVariant::Double:
        sql += " REAL ";
        break;
      case QVariant::String:
        sql += " TEXT ";
        break;
      default:
        {
        mdtError e(MDT_DATABASE_ERROR, "Unsupported field type, database " + db.databaseName(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
        e.commit();
        }
    }
    if(i < (fields.size()-1)){
      sql += ",";
    }
    sql += "\n";
  }
  sql += ")";
  
  ///sql = "CREATE TABLE IF NOT EXISTS data (id_PK INTEGER PRIMARY KEY)";
  sql = "CREATE TABLE IF NOT EXISTS " + tableName +" (id_PK INTEGER PRIMARY KEY)";
  qDebug() << "SQL: " << sql;
  
  qDebug() << "CNN name: " << db.connectionName();
  qDebug() << "db valid ?: " << db.isValid();
  qDebug() << "db open ?: " << db.isOpen();
  qDebug() << "db name: " << db.databaseName();
  qDebug() << "Current tables: " << db.tables();
  
  ///sql = "CREATE TABLE " + tableName + "(id_PK INTEGER PRIMARY KEY)";
  
  // Run query
  QSqlQuery query(sql, db);
  if(!query.exec()){
    ///mdtError e(MDT_DATABASE_ERROR, "Cannot create table " + tableName, mdtError::Error);
    mdtError e(MDT_DATABASE_ERROR, "Cannot create table " + tableName + ": " + query.lastError().databaseText(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    ///e.setSystemError(query.lastError().number(), query.lastError().text());
    e.setSystemError(query.lastError().number(), query.lastError().driverText());
    e.commit();
    return false;
  }

  qDebug() << "Current tables: " << db.tables();
  
  return true;
}

bool mdtDataTableModel::dropDatabaseTable(const QString &tableName, const QSqlDatabase &db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());

  QString sql;

  // Build SQL query
  sql = "DROP TABLE IF EXISTS " + tableName;
  // Run query
  QSqlQuery query(sql, db);
  if(!query.exec()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot drop table " + tableName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    e.setSystemError(query.lastError().number(), query.lastError().text());
    e.commit();
    return false;
  }

  return true;
}
