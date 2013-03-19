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
#include <QMessageBox>

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

QString mdtDataTableModel::getTableName(const QString &dataSetName)
{
  QString tableName;

  tableName = dataSetName.trimmed();
  tableName.replace(".", "_");
  tableName += "_tbl";

  return tableName;
}

QSqlDatabase mdtDataTableModel::createDataSet(const QDir &dir, const QString &name, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, create_mode_t mode)
{
  QFile file;
  QFileInfo fileInfo;
  QSqlDatabase db;
  QString cnnName;  // Connection name
  QString dbName;   // Database name
  QString tableName;

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
      return db;
    }
  }
  // Here, we are connected
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());
  // Check if table exists
  if(db.tables().contains(tableName)){
    switch(mode){
      case OverwriteExisting:
        // If we cannot drop expected table, it's possibly a other file
        if(!mdtDataTableModel::dropDatabaseTable(tableName, db)){
          db.close();
          return db;
        }
        break;
      case KeepExisting:
        // Nothing else to do
        return db;
      case FailIfExists:
        db.close();
        return db;
      case AskUserIfExists:
        if(userChooseToOverwrite(dir, fileInfo.fileName() + ".db")){
          // If we cannot drop expected table, it's possibly a other file
          if(!mdtDataTableModel::dropDatabaseTable(tableName, db)){
            db.close();
            return db;
          }
        }
        break;
    }
  }
  // Here, we must create database table
  if(!createDatabaseTable(tableName, primaryKey, fields, db)){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create database table for data set " + name, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
    e.commit();
    db.close();
  }
  // Finished
  return db;
}

bool mdtDataTableModel::addRow(const QMap<QString,QVariant> &data, int role)
{
  QMap<QString,QVariant>::const_iterator it;

  if(!insertRows(0, 1)){
    return false;
  }
  for(it = data.constBegin(); it != data.constEnd(); it++){
    if(!QSqlTableModel::setData(index(0, fieldIndex(it.key())) , it.value(), role)){
      revertRow(0);
      return false;
    }
  }

  return submit();
}

bool mdtDataTableModel::setData(int row, int column, const QVariant &value, int role)
{
  if(!QSqlTableModel::setData(index(row, column), value, role)){
    revertRow(row);
    return false;
  }

  return submit();
}

bool mdtDataTableModel::setData(int row, const QString &field, const QVariant & value, int role)
{
  return setData(row, fieldIndex(field), value, role);
}

bool mdtDataTableModel::exportToCsvFile(const QString & filePath)
{
  return false;
}

bool mdtDataTableModel::importFromCsvFile(const QString & filePath)
{
  return false;
}

bool mdtDataTableModel::createDatabaseTable(const QString &tableName, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, const QSqlDatabase &db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());

  QString sql;
  int i;

  // Build SQL query
  sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
  // Add primary key fields
  for(i=0; i<primaryKey.count(); i++){
    sql += primaryKey.field(i).name() + " ";
    switch(primaryKey.field(i).type()){
      case QVariant::Int:
        sql += " INTEGER ";
        break;
      case QVariant::String:
        sql += " TEXT ";
        break;
      default:
        {
        mdtError e(MDT_DATABASE_ERROR, "Unsupported field type for primary key, database " + db.databaseName(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableModel");
        e.commit();
        }
    }
    if(i < (primaryKey.count()-1)){
      sql += ", ";
    }
  }
  // Add other fields
  if(fields.size() > 0){
    sql += ", ";
  }
  for(i=0; i<fields.size(); i++){
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
      sql += ", ";
    }
  }
  // Add primary key constraint
  if(primaryKey.count() > 0){
    sql += " , CONSTRAINT ";
    if(primaryKey.name().isEmpty()){
      sql += tableName + "_PK ";
    }else{
      sql += primaryKey.name() + " ";
    }
    sql += " PRIMARY KEY (";
    for(i=0; i<primaryKey.count(); i++){
      sql += primaryKey.field(i).name() + " ";
      if(i < (primaryKey.count()-1)){
        sql += ", ";
      }
    }
    sql += ")";
  }
  sql += ")";
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

bool mdtDataTableModel::userChooseToOverwrite(const QDir &dir, const QString &fileName)
{
  QMessageBox msgBox;
  QString info;
  int retVal;

  msgBox.setText(tr("A file allready exists in destination directory"));
  info = tr("File: ") + fileName + "\n";
  info += tr("Directory: ") + dir.absolutePath() + "\n";
  info += tr("\nDo you want to overwrite this file ?");
  msgBox.setInformativeText(info);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::Question);
  retVal = msgBox.exec();
  if(retVal == QMessageBox::Yes){
    return true;
  }

  return false;
}
