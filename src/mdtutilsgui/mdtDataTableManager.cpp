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
#include "mdtDataTableManager.h"
#include "mdtDataTableModel.h"
#include "mdtError.h"
#include <QFileInfo>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QStringList>

#include <QDebug>

mdtDataTableManager::mdtDataTableManager(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDb = db;
  pvModel = 0;
  // Set a default CSV format
  setCsvFormat(";", "\"", "#", '\\', MDT_NATIVE_EOL);
}

mdtDataTableManager::~mdtDataTableManager()
{
  ///delete pvModel;
}

bool mdtDataTableManager::setDataSetDirectory(const QDir &dir)
{
  if(!dir.exists()){
    mdtError e(MDT_FILE_IO_ERROR, "Directory not found, path: " + dir.absolutePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  pvDataSetDirectory = dir;

  return true;
}

QDir mdtDataTableManager::dataSetDirectory() const
{
  return pvDataSetDirectory;
}

QString mdtDataTableManager::getTableName(const QString &dataSetName)
{
  QString tableName;

  tableName = dataSetName.trimmed();
  tableName.replace(".", "_");
  tableName += "_tbl";

  return tableName;
}

bool mdtDataTableManager::createDataSet(const QDir &dir, const QString &name, const QSqlIndex &primaryKey, const QList<QSqlField> &fields, create_mode_t mode)
{
  QFile file;
  QFileInfo fileInfo;
  QString cnnName;  // Connection name
  QString dbName;   // Database name
  QString tableName;

  // Set names
  cnnName = name;
  fileInfo.setFile(dir, name);
  dbName = fileInfo.absoluteFilePath() + ".db";
  tableName = mdtDataTableManager::getTableName(name);

  // We check that data set directory exists
  if(!dir.exists()){
    mdtError e(MDT_FILE_IO_ERROR, "Directory not found, path: " + dir.absolutePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  // Check if we are allready connected
  pvDb = QSqlDatabase::database(cnnName);
  if(pvDb.isOpen()){
    // Check that we are connected to the correct database
    if(pvDb.databaseName() != dbName){
      mdtError e(MDT_DATABASE_ERROR, "A connection to dataset " + name + " exists, but contains wrong database", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
      e.commit();
      pvDb.close();
      return false;
    }
  }else{
    // Try to open/connect
    pvDb = QSqlDatabase::addDatabase("QSQLITE", cnnName);
    if(!pvDb.isValid()){
      mdtError e(MDT_DATABASE_ERROR, "Cannot create database connection (probably plugin/driver problem)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
      e.commit();
      return false;
    }
    pvDb.setDatabaseName(dbName);
    if(!pvDb.open()){
      mdtError e(MDT_DATABASE_ERROR, "Cannot open database " + dbName, mdtError::Error);
      e.setSystemError(pvDb.lastError().number(), pvDb.lastError().text());
      MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
      e.commit();
      return false;
    }
  }
  // Here, we are connected
  Q_ASSERT(pvDb.isValid());
  Q_ASSERT(pvDb.isOpen());
  // Check if table exists
  if(pvDb.tables().contains(tableName)){
    switch(mode){
      case OverwriteExisting:
        // If we cannot drop expected table, it's possibly a other file
        if(!mdtDataTableManager::dropDatabaseTable(tableName)){
          pvDb.close();
          return false;
        }
        break;
      case KeepExisting:
        // Nothing else to do
        return true;
      case FailIfExists:
        pvDb.close();
        return false;
      case AskUserIfExists:
        if(userChooseToOverwrite(dir, fileInfo.fileName() + ".db")){
          // If we cannot drop expected table, it's possibly a other file
          if(!mdtDataTableManager::dropDatabaseTable(tableName)){
            pvDb.close();
            return false;
          }
        }
        break;
    }
  }
  // Here, we must create database table
  if(!createDatabaseTable(tableName, primaryKey, fields)){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create database table for data set " + name, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    pvDb.close();
  }
  // Finished
  delete pvModel;
  pvModel = new mdtDataTableModel(this, pvDb);
  pvModel->setTable(tableName);

  return true;
}

QSqlDatabase mdtDataTableManager::database() const
{
  return pvDb;
}

void mdtDataTableManager::setCsvFormat(const QString &separator, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol)
{
  pvCsvSeparator = separator;
  pvCsvDataProtection = dataProtection;
  pvCsvComment = comment;
  pvCsvEscapeChar = escapeChar;
  pvCsvEol = eol;
}

bool mdtDataTableManager::exportToCsvFile(const QString & filePath, create_mode_t mode)
{
  return false;
}

bool mdtDataTableManager::importFromCsvFile(const QString &csvFilePath, create_mode_t mode, const QString &dir)
{
  QFileInfo fileInfo(csvFilePath);
  QDir dbDir;
  mdtCsvFile csvFile(0, "ISO 8859-15");
  QStringList header;
  QStringList line;
  QSqlDatabase db;
  QString dataSetName;
  QString dataSetTableName;
  QList<QStringList> rows;
  
  QSqlIndex pk;
  
  QString fieldName;
  QSqlField field;
  QList<QSqlField> fields;
  int i;
  
  QSqlTableModel::EditStrategy originalEditStrategy;;

  // Set DB directory
  if(dir.isEmpty()){
    dbDir = fileInfo.absoluteDir();
  }else{
    dbDir.setPath(dir);
  }
  if(!dbDir.exists()){
    mdtError e(MDT_FILE_IO_ERROR, "Directory not found: " + dbDir.path(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  // Open CSV file
  csvFile.setFileName(fileInfo.absoluteFilePath());
  if(!csvFile.open(QIODevice::ReadOnly)){
    mdtError e(MDT_FILE_IO_ERROR, "Cannot open CSV file: " + fileInfo.absoluteFilePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  setCsvFormat(";", "", "", '\0', "\r\n");
  // Read header and create fields
  header = csvFile.readHeader(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
  if(header.isEmpty()){
    mdtError e(MDT_FILE_IO_ERROR, "No header found in CSV file: " + fileInfo.absoluteFilePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    return false;
  }
  
  pk.append(QSqlField("id_PK", QVariant::Int));
  
  for(i=0; i<header.size(); i++){
    fieldName = header.at(i).trimmed();
    fieldName.replace(" ", "_");
    fieldName.replace("(", "_");
    fieldName.replace(")", "_");
    fieldName.replace("%", "_");
    fieldName.replace("?", "_");
    fieldName.replace("+", "_");
    fieldName.replace("/", "_");
    if(!fieldName.isEmpty()){
      qDebug() << "Creating field " << fieldName << " ...";
      field.setName(fieldName);
      field.setType(QVariant::String);
      fields.append(field);
    }
  }
  // Create data set
  dataSetName = fileInfo.baseName();
  dataSetTableName = getTableName(dataSetName);
  ///db = createDataSet(dbDir, dataSetName, pk, fields, mode);
  if(!createDataSet(dbDir, dataSetName, pk, fields, mode)){
    csvFile.close();
    return false;
  }
  // Check that model could be set
  if(model() == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create model for data set " + dataSetName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    pvDb.close();
    return false;
  }
  ///qDebug() << "Header: " << header;
  ///qDebug() << "Fields: " << fields;
  ///qDebug() << "/HEADER\nData:" << csvFile.readLine(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol).size();
  // For multiple rows inserstion, we need OnManualSubmit edit strategy
  originalEditStrategy = model()->editStrategy();
  if(originalEditStrategy != QSqlTableModel::OnManualSubmit){
    model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  }
  // Store data
  i=0;
  while(csvFile.hasMoreLines()){
    line = csvFile.readLine(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
    while(line.size() > fields.size()){
      ///qDebug() << "Line contains more columns than fields !";
      if(!line.last().isEmpty()){
        qDebug() << "WWW loosing data !!";
      }
      line.removeLast();
    }
    rows.append(line);
    if(i>50){
      qDebug() << "Commit...";
      // Begin transaction (without, insertions are very slow)
      if(!pvDb.transaction()){
        mdtError e(MDT_DATABASE_ERROR, "Unable to begin transaction, data set: " + dataSetName, mdtError::Error);
        e.setSystemError(pvDb.lastError().number(), pvDb.lastError().text());
        MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
        e.commit();
        csvFile.close();
        pvDb.close();
        return false;
      }
      // Commit some lines
      if(!model()->addRows(rows, true, Qt::EditRole)){
        mdtError e(MDT_DATABASE_ERROR, "Unable to add some rows in model, data set: " + dataSetName, mdtError::Error);
        e.setSystemError(model()->lastError().number(), model()->lastError().text());
        MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
        e.commit();
        csvFile.close();
        pvDb.close();
        return false;
      }
      // Commit transaction
      if(!pvDb.commit()){
        mdtError e(MDT_DATABASE_ERROR, "Unable to commit transaction, data set: " + dataSetName, mdtError::Error);
        e.setSystemError(pvDb.lastError().number(), pvDb.lastError().text());
        MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
        e.commit();
        csvFile.close();
        pvDb.close();
        return false;
      }
      rows.clear();
      i=0;
    }
    i++;
  }
  // Commit last lines
  qDebug() << "Commit last " << rows.size() << " lines...";
  if(!pvDb.transaction()){
    mdtError e(MDT_DATABASE_ERROR, "Unable to begin transaction, data set: " + dataSetName, mdtError::Error);
    e.setSystemError(pvDb.lastError().number(), pvDb.lastError().text());
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    pvDb.close();
    return false;
  }
  if(!model()->addRows(rows, true, Qt::EditRole)){
    mdtError e(MDT_DATABASE_ERROR, "Unable to add some rows in model, data set: " + dataSetName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    pvDb.close();
    return false;
  }
  if(!pvDb.commit()){
    mdtError e(MDT_DATABASE_ERROR, "Unable to commit transaction, data set: " + dataSetName, mdtError::Error);
    e.setSystemError(pvDb.lastError().number(), pvDb.lastError().text());
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    pvDb.close();
    return false;
  }

  csvFile.close();
  // Restore edit strategy
  if(originalEditStrategy != QSqlTableModel::OnManualSubmit){
    model()->setEditStrategy(originalEditStrategy);
  }

  return true;
}

mdtDataTableModel *mdtDataTableManager::model()
{
  return pvModel;
}

bool mdtDataTableManager::createDatabaseTable(const QString &tableName, const QSqlIndex &primaryKey, const QList<QSqlField> &fields)
{
  Q_ASSERT(pvDb.isValid());
  Q_ASSERT(pvDb.isOpen());

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
        mdtError e(MDT_DATABASE_ERROR, "Unsupported field type for primary key, database " + pvDb.databaseName(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
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
        mdtError e(MDT_DATABASE_ERROR, "Unsupported field type, database " + pvDb.databaseName(), mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
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
  QSqlQuery query(sql, pvDb);
  if(!query.exec()){
    ///mdtError e(MDT_DATABASE_ERROR, "Cannot create table " + tableName, mdtError::Error);
    mdtError e(MDT_DATABASE_ERROR, "Cannot create table " + tableName + ": " + query.lastError().databaseText(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    ///e.setSystemError(query.lastError().number(), query.lastError().text());
    e.setSystemError(query.lastError().number(), query.lastError().driverText());
    e.commit();
    return false;
  }

  return true;
}

/**
bool mdtDataTableManager::dropDatabaseTable(const QString &tableName, const QSqlDatabase &db)
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
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.setSystemError(query.lastError().number(), query.lastError().text());
    e.commit();
    return false;
  }

  return true;
}
*/

bool mdtDataTableManager::dropDatabaseTable(const QString &tableName)
{
  Q_ASSERT(pvDb.isValid());
  Q_ASSERT(pvDb.isOpen());

  QString sql;

  // Build SQL query
  sql = "DROP TABLE IF EXISTS " + tableName;
  // Run query
  QSqlQuery query(sql, pvDb);
  if(!query.exec()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot drop table " + tableName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.setSystemError(query.lastError().number(), query.lastError().text());
    e.commit();
    return false;
  }

  return true;
}

bool mdtDataTableManager::userChooseToOverwrite(const QDir &dir, const QString &fileName)
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
