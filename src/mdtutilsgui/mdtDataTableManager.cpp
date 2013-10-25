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
#include "mdtFieldMapItem.h"
#include <QSqlTableModel>
#include <QFileInfo>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QStringList>
#include <QProgressDialog>
#include <QApplication>
#include <QTextCodec>

#include <QDebug>

mdtDataTableManager::mdtDataTableManager(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabaseManager = new mdtSqlDatabaseManager(this);
  pvDatabaseManager->setDatabase(db);
  ///pvDb = db;
  pvModel = 0;
  pvProgressDialogEnabled = false;
  // Set a default CSV format
  setCsvFormat(";", "\"", "#", '\\', MDT_NATIVE_EOL, "UTF-8");
}

mdtDataTableManager::~mdtDataTableManager()
{
  ///delete pvModel;
}

void mdtDataTableManager::close()
{
  if(pvModel != 0){
    delete pvModel;
    pvModel = 0;
  }
  pvDatabaseManager->database().close();
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
  tableName.replace(" ", "_");
  tableName.replace("+", "_");
  tableName.replace("-", "_");
  tableName.replace("*", "_");
  tableName.replace("/", "_");
  tableName.replace("<", "_");
  tableName.replace(">", "_");
  tableName.replace("=", "_");
  tableName.replace("~", "_");
  tableName.replace("!", "_");
  tableName.replace("@", "_");
  tableName.replace("#", "_");
  tableName.replace("%", "_");
  tableName.replace("^", "_");
  tableName.replace("&", "_");
  tableName.replace("|", "_");
  tableName.replace("`", "_");
  tableName.replace("?", "_");
  tableName.replace("$", "_");
  tableName.replace("(", "_");
  tableName.replace(")", "_");
  tableName.replace("[", "_");
  tableName.replace("]", "_");
  tableName.replace(",", "_");
  tableName.replace(";", "_");
  tableName.replace(":", "_");
  tableName.replace(".", "_");
  tableName += "_tbl";

  return tableName;
}

QString mdtDataTableManager::getFieldName(const QString &columnName)
{
  QString fieldName;

  fieldName = columnName.trimmed();
  fieldName.replace(" ", "_");
  fieldName.replace("+", "_");
  fieldName.replace("-", "_");
  fieldName.replace("*", "_");
  fieldName.replace("/", "_");
  fieldName.replace("<", "_");
  fieldName.replace(">", "_");
  fieldName.replace("=", "_");
  fieldName.replace("~", "_");
  fieldName.replace("!", "_");
  fieldName.replace("@", "_");
  fieldName.replace("#", "_");
  fieldName.replace("%", "_");
  fieldName.replace("^", "_");
  fieldName.replace("&", "_");
  fieldName.replace("|", "_");
  fieldName.replace("`", "_");
  fieldName.replace("?", "_");
  fieldName.replace("$", "_");
  fieldName.replace("(", "_");
  fieldName.replace(")", "_");
  fieldName.replace("[", "_");
  fieldName.replace("]", "_");
  fieldName.replace(",", "_");
  fieldName.replace(";", "_");
  fieldName.replace(":", "_");
  fieldName.replace(".", "_");

  return fieldName;
}

/// \todo Use mdtSqlDatabaseManager createMode_t !
bool mdtDataTableManager::createDataSet(const QDir &dir, const QString &name, mdtSqlSchemaTable &table, mdtSqlDatabaseManager::createMode_t mode)
///bool mdtDataTableManager::createDataSet(const QDir &dir, const QString &name, mdtSqlSchemaTable &table, create_mode_t mode)
///bool mdtDataTableManager::createDataSet(const QDir &dir, const QString &name, const QSqlIndex &primaryKey, bool createPrimaryKeyFields, const QList<QSqlField> &fields, create_mode_t mode)
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

  qDebug() << "Creating dataset ...";
  
  // Create database
  if(!pvDatabaseManager->createDatabaseSqlite(fileInfo, mode, cnnName)){
    /// \todo Error storage/output
    return false;
  }
  
  /**
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
        }else{
          pvDb.close();
          return false;
        }
        break;
    }
  }
  
  */
  
  // Here, we must create database table
  ///if(!createDatabaseTable(tableName, primaryKey, createPrimaryKeyFields, fields)){
  ///table.setDriverName(pvDb.driverName());
  table.setTableName(tableName);
  qDebug() << "create table " << table.tableName() << " ...";
  if(!pvDatabaseManager->createTable(table, mode)){
    /// \todo Err output
    return false;
  }
  /**
  if(!createDatabaseTable(table)){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create database table for data set " + name, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    pvDb.close();
    return false;
  }
  */
  // Finished
  delete pvModel;
  ///pvModel = new mdtDataTableModel(this, pvDb);
  pvModel = new mdtDataTableModel(this, pvDatabaseManager->database());
  pvModel->setTable(tableName);

  return true;
}

QSqlDatabase mdtDataTableManager::database() const
{
  return pvDatabaseManager->database();
}

void mdtDataTableManager::enableProgressDialog(bool enable)
{
  pvProgressDialogEnabled = enable;
}

void mdtDataTableManager::setCsvFormat(const QString &separator, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol, const QByteArray &encoding)
{
  pvCsvSeparator = separator;
  pvCsvDataProtection = dataProtection;
  pvCsvComment = comment;
  pvCsvEscapeChar = escapeChar;
  pvCsvEol = eol;
  if(!QTextCodec::availableCodecs().contains(encoding)){
    QTextCodec *codec = QTextCodec::codecForLocale();
    Q_ASSERT(codec != 0);
    pvEncoding = codec->name();
    mdtError e(MDT_FILE_IO_ERROR, "Encoding not found: " + encoding + " , using " + pvEncoding, mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
  }else{
    pvEncoding = encoding;
  }
}

bool mdtDataTableManager::exportToCsvFile(const QString & filePath, mdtSqlDatabaseManager::createMode_t mode)
{
  return false;
}

bool mdtDataTableManager::importFromCsvFile(const QString &csvFilePath, mdtSqlDatabaseManager::createMode_t mode, const QString &dir, const QStringList &pkFields)
{
  QFileInfo fileInfo(csvFilePath);
  QDir dbDir;
  mdtCsvFile csvFile(0, pvEncoding);
  QStringList header;
  QStringList line;
  QList<QStringList> rows;
  int lineCount;
  QString dataSetTableName;
  mdtFieldMapItem *mapItem;
  QList<mdtFieldMapItem*> mapItems;
  int csvFieldIndex;
  int modelFieldIndex;
  ///QSqlIndex pk;
  QSqlField field;
  ///QList<QSqlField> fields;
  
  mdtSqlSchemaTable table;
  
  QSqlTableModel::EditStrategy originalEditStrategy;;
  QProgressDialog *progress = 0;
  int totalLineCount;
  int dataLossCount;
  int i;

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
  // Read header and create fields
  header = csvFile.readHeader(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
  if(header.isEmpty()){
    mdtError e(MDT_FILE_IO_ERROR, "No header found in CSV file: " + fileInfo.absoluteFilePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    return false;
  }
  // Start creating fields
  pvAbort = false;
  modelFieldIndex = 0;
  // Create primary key
  if(pkFields.isEmpty()){
    // Primary key is not in CSV file, generate a field and add to PK constraint list
    // Add primary key to field map
    mapItem = new mdtFieldMapItem;
    mapItem->setFieldIndex(0);
    mapItem->setFieldName("id_PK");
    mapItem->setFieldDisplayText("id_PK");
    mapItem->setDataType(QVariant::Int);
    pvFieldMap.addItem(mapItem);
    ///pk.append(QSqlField("id_PK", QVariant::Int));
    // Create primary key field
    field = QSqlField();
    field.setName("id_PK");
    field.setType(QVariant::Int);
    field.setAutoValue(true);
    table.addField(field, true);
    ///fields.append(field);
    modelFieldIndex++;
  }else{
    // Primary key is one of the CSV header
    /**
    for(i = 0; i < pkFields.size(); ++i){
      Q_ASSERT(header.contains(pkFields.at(i)));
      pk.append(getFieldName(pkFields.at(i)));
    }
    */
  }
  // Create fields regarding field map
  for(csvFieldIndex = 0; csvFieldIndex < header.size(); ++csvFieldIndex){
    if(!header.at(csvFieldIndex).isEmpty()){
      mapItems = pvFieldMap.itemsAtSourceFieldName(header.at(csvFieldIndex));
      if(mapItems.isEmpty()){
        // No mapping for this header item, generate one
        mapItem = new mdtFieldMapItem;
        mapItem->setFieldName(getFieldName(header.at(csvFieldIndex)));
        mapItem->setFieldDisplayText(header.at(csvFieldIndex));
        mapItem->setSourceFieldName(header.at(csvFieldIndex));
        mapItem->setDataType(QVariant::String);
        pvFieldMap.addItem(mapItem);
        mapItems.append(mapItem);
      }
      // Create fields
      for(i = 0; i < mapItems.size(); i++){
        // Update field map indexes
        mapItem = mapItems.at(i);
        Q_ASSERT(mapItem != 0);
        mapItem->setSourceFieldIndex(csvFieldIndex);
        mapItem->setFieldIndex(modelFieldIndex);
        pvFieldMap.updateItem(mapItem);
        field = QSqlField();
        field.setName(mapItem->fieldName());
        field.setType(mapItem->dataType());
        if(pkFields.contains(field.name())){
          table.addField(field, true);
        }else{
          table.addField(field, false);
        }
        ///fields.append(field);
        modelFieldIndex++;
      }
    }
  }
  // Create data set
  close();
  pvDataSetName = fileInfo.baseName();
  dataSetTableName = getTableName(pvDataSetName);
  ///if(!createDataSet(dbDir, pvDataSetName, pk, false, fields, mode)){
  if(!createDataSet(dbDir, pvDataSetName, table, mode)){
    csvFile.close();
    return false;
  }
  // Check that model could be set
  if(model() == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot create model for data set " + pvDataSetName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    csvFile.close();
    close();
    ///pvDb.close();
    pvDatabaseManager->database().close();
    return false;
  }
  // For multiple rows inserstion, we need OnManualSubmit edit strategy
  originalEditStrategy = model()->editStrategy();
  if(originalEditStrategy != QSqlTableModel::OnManualSubmit){
    model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  }
  // Store data
  if(pvProgressDialogEnabled){
    totalLineCount = 0;
    progress = new QProgressDialog;
    progress->setWindowTitle(tr("CSV import progress"));
    progress->setMinimum(0);
    progress->setMaximum(0);
    connect(progress, SIGNAL(canceled()), this, SLOT(setAbortFlag()));
    progress->show();
  }
  lineCount=0;
  dataLossCount = 0;
  while(csvFile.hasMoreLines()){
    // Check if we have to abort
    if(pvAbort){
      csvFile.close();
      ///pvDb.close();
      ///pvDatabaseManager->database().close();
      close();
      return false;
    }
    // Read a line in CSV file
    line = csvFile.readLine(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
    // On some parse error, it can happen that we have to much columns
    // Common known issue is the ending ; with no more data, but we don't loose data in this case.
    ///while(line.size() > fields.size()){
    while(line.size() > table.fieldCount()){
      if(!line.last().isEmpty()){
        dataLossCount++;
      }
      line.removeLast();
    }
    rows.append(line);
    if(lineCount>100){
      if(!commitRowsToModel(rows)){
        csvFile.close();
        ///pvDb.close();
        ///pvDatabaseManager->database().close();
        close();
        return false;
      }
      // Update progress dialog
      if(progress != 0){
        totalLineCount += rows.size();
        qDebug() << "Commit..., line count: " << totalLineCount;
        progress->setLabelText(tr("Importing line ") + QString::number(totalLineCount));
        qApp->processEvents();
      }
      rows.clear();
      lineCount=0;
    }
    lineCount++;
  }
  // Commit last lines
  qDebug() << "Commit last " << rows.size() << " lines...";
  if(!commitRowsToModel(rows)){
    csvFile.close();
    ///pvDb.close();
    ///pvDatabaseManager->database().close();
    close();
    return false;
  }
  csvFile.close();
  // Restore edit strategy
  if(originalEditStrategy != QSqlTableModel::OnManualSubmit){
    model()->setEditStrategy(originalEditStrategy);
  }
  if(progress != 0){
    delete progress;
  }
  // Log a warning if some data were lost
  if(dataLossCount > 0){
    mdtError e(MDT_FILE_IO_ERROR, "Some data has been lost during import of CSV file: " + fileInfo.absoluteFilePath(), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
  }

  return true;
}

void mdtDataTableManager::addFieldMapping(const QString &csvHeaderItem, const QString &fieldName, const QString &displayText, QVariant::Type dataType, int csvDataItemStartOffset, int csvDataItemEndOffset)
{
  mdtFieldMapItem *item;

  item = new mdtFieldMapItem;
  if(item == 0){
    mdtError e(MDT_MEMORY_ALLOC_ERROR, "Unable to create a mdtFieldMapItem (memory allocation failed)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return;
  }
  item->setSourceFieldName(csvHeaderItem);
  item->setFieldName(fieldName);
  item->setFieldDisplayText(displayText);
  item->setDataType(dataType);
  item->setSourceFieldDataStartOffset(csvDataItemStartOffset);
  item->setSourceFieldDataEndOffset(csvDataItemEndOffset);
  pvFieldMap.addItem(item);
}

void mdtDataTableManager::clearFieldMap()
{
  pvFieldMap.clear();
}

QStringList mdtDataTableManager::csvHeader() const
{
  QStringList header;
  QString headerItem;
  QSqlRecord record;
  int i;

  if(pvModel == 0){
    mdtError e(MDT_GUI_ERROR, "Model is null", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return header;
  }
  record = pvModel->record();
  for(i=0; i<record.count(); i++){
    headerItem = pvFieldMap.sourceFieldNameAtFieldIndex(i);
    if((!headerItem.isEmpty())&&(!header.contains(headerItem))){
      header.append(headerItem);
    }
  }

  return header;
}

void mdtDataTableManager::setDisplayTextsToModelHeader()
{
  Q_ASSERT(pvModel != 0);

  int i;
  mdtFieldMapItem *mapItem;

  for(i=0; i<pvModel->columnCount(); i++){
    mapItem = pvFieldMap.itemAtFieldIndex(i);
    if(mapItem != 0){
      pvModel->setHeaderData(i, Qt::Horizontal, mapItem->fieldDisplayText());
    }
  }
}

QHash<QString, QString> mdtDataTableManager::displayTextsByFieldNames() const
{
  return pvFieldMap.displayTextsByFieldNames();
}

mdtDataTableModel *mdtDataTableManager::model()
{
  return pvModel;
}

void mdtDataTableManager::setAbortFlag()
{
  pvAbort = true;
}

/**
bool mdtDataTableManager::createDatabaseTable(const mdtSqlSchemaTable &table)
{
  Q_ASSERT(pvDb.isValid());
  Q_ASSERT(pvDb.isOpen());

  QString sql;
  QSqlQuery query(pvDb);
  QSqlError sqlError;

  sql = table.sqlForCreateTable();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot create table " + table.tableName(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.setSystemError(sqlError.number(), sqlError.driverText());
    e.commit();
    return false;
  }

  return true;
}
*/

/**
bool mdtDataTableManager::createDatabaseTable(const QString &tableName, const QSqlIndex &primaryKey, bool createPrimaryKeyFields, const QList<QSqlField> &fields)
{
  Q_ASSERT(pvDb.isValid());
  Q_ASSERT(pvDb.isOpen());

  QString sql;
  int i;

  // Build SQL query
  sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
  // Add primary key fields if needed
  if(createPrimaryKeyFields){
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
*/

/**
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
*/

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

bool mdtDataTableManager::commitRowsToModel(const QList<QStringList> &rows)
{
  QSqlDatabase db = pvDatabaseManager->database();

  // Begin transaction (without, insertions are very slow)
  if(!db.transaction()){
  ///if(!pvDb.transaction()){
    mdtError e(MDT_DATABASE_ERROR, "Unable to begin transaction, data set: " + pvDataSetName, mdtError::Error);
    e.setSystemError(db.lastError().number(), db.lastError().text());
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  // Commit some lines
  if(!model()->addRows(rows, pvFieldMap, Qt::EditRole)){
    mdtError e(MDT_DATABASE_ERROR, "Unable to add some rows in model, data set: " + pvDataSetName, mdtError::Error);
    e.setSystemError(model()->lastError().number(), model()->lastError().text());
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }
  // Commit transaction
  if(!db.commit()){
  ///if(!pvDb.commit()){
    mdtError e(MDT_DATABASE_ERROR, "Unable to commit transaction, data set: " + pvDataSetName, mdtError::Error);
    e.setSystemError(db.lastError().number(), db.lastError().text());
    MDT_ERROR_SET_SRC(e, "mdtDataTableManager");
    e.commit();
    return false;
  }

  return true;
}
