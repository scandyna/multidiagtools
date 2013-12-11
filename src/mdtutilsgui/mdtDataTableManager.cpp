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
#include "mdtFieldMapItem.h"
#include <QSqlTableModel>
#include <QAbstractTableModel>
#include <QFileInfo>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlIndex>
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
  pvProgressDialogEnabled = false;
  // Set a default CSV format
  setCsvFormat(";", "\"", "#", '\\', MDT_NATIVE_EOL, "UTF-8");
}

mdtDataTableManager::~mdtDataTableManager()
{
}

mdtError mdtDataTableManager::lastError() const
{
  return pvLastError;
}

void mdtDataTableManager::close()
{
  pvDatabaseManager->database().close();
  pvDataSetTableName = "";
}

bool mdtDataTableManager::setDataSetDirectory(const QDir &dir)
{
  if(!dir.exists()){
    pvLastError.setError(tr("Directory not found, path: ") + dir.absolutePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }
  pvDataSetDirectory = dir;

  return true;
}

QDir mdtDataTableManager::dataSetDirectory() const
{
  return pvDataSetDirectory;
}

const QString mdtDataTableManager::dataSetTableName() const
{
  return pvDataSetTableName;
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

bool mdtDataTableManager::createDataSet(const QDir &dir, const QString &name, mdtSqlSchemaTable &table, mdtSqlDatabaseManager::createMode_t mode)
{
  QFile file;
  QFileInfo fileInfo;
  QString cnnName;  // Connection name
  QString dbName;   // Database name

  // Set names
  cnnName = name;
  fileInfo.setFile(dir, name);
  dbName = fileInfo.absoluteFilePath();
  if(dbName.right(7) != ".sqlite"){
    dbName += ".sqlite";
  }
  pvDataSetTableName = mdtDataTableManager::getTableName(name);
  // Create database
  if(!pvDatabaseManager->createDatabaseSqlite(fileInfo, mode, cnnName)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }
  // Create database table
  table.setTableName(pvDataSetTableName);
  if(!pvDatabaseManager->createTable(table, mode)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }

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
    pvLastError.setError(tr("Encoding not found: ") + encoding + tr(" , using ") + pvEncoding + tr("."), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
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
  QList<QVariantList> rows;
  int lineCount;
  mdtFieldMapItem *mapItem;
  QList<mdtFieldMapItem*> mapItems;
  int csvFieldIndex;
  int modelFieldIndex;
  mdtSqlSchemaTable table;
  QProgressDialog *progress = 0;
  int totalLineCount;
  int dataLossCount;
  int i;
  QSqlField field;
  QList<mdtFieldMapField> fields;
  QString autoValuePkFieldName;

  // Set DB directory
  if(dir.isEmpty()){
    dbDir = fileInfo.absoluteDir();
  }else{
    dbDir.setPath(dir);
  }
  if(!dbDir.exists()){
    pvLastError.setError(tr("Directory not found: ") + dbDir.path(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }
  // Open CSV file
  csvFile.setFileName(fileInfo.absoluteFilePath());
  if(!csvFile.open(QIODevice::ReadOnly)){
    pvLastError.setError(tr("Cannot open CSV file: ") + fileInfo.absoluteFilePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }
  // Read header and create fields
  header = csvFile.readHeader(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
  if(header.isEmpty()){
    pvLastError.setError(tr("No header found in CSV file: ") + fileInfo.absoluteFilePath(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    csvFile.close();
    return false;
  }
  pvDatabaseHeader.clear();
  // If no CSV field is part of primary key, we will create one with auto generated value
  if(pkFields.isEmpty()){
    autoValuePkFieldName = "id_PK";
    pvDatabaseHeader << autoValuePkFieldName;
  }
  // Create or update field mapping regarding allready set-up map
  modelFieldIndex = 0;
  for(csvFieldIndex = 0; csvFieldIndex < header.size(); ++csvFieldIndex){
    if(!header.at(csvFieldIndex).isEmpty()){
      mapItems = pvFieldMap.itemsAtSourceFieldName(header.at(csvFieldIndex));
      if(mapItems.isEmpty()){
        // No mapping for this header item, generate one
        mapItem = new mdtFieldMapItem;
        mapItem->setDestinationFieldName(mdtFieldMapField::getFieldName(header.at(csvFieldIndex)));
        mapItem->setDestinationFieldDisplayText(header.at(csvFieldIndex));
        mapItem->setSourceFieldName(header.at(csvFieldIndex));
        mapItem->setDataType(QVariant::String);
        pvFieldMap.addItem(mapItem);
        mapItems.append(mapItem);
      }
      // Set indexes
      for(i = 0; i < mapItems.size(); i++){
        mapItem = mapItems.at(i);
        Q_ASSERT(mapItem != 0);
        mapItem->setSourceFieldIndex(csvFieldIndex);
        mapItem->setDestinationFieldIndex(modelFieldIndex);
        modelFieldIndex++;
      }
    }
  }
  pvDatabaseHeader << pvFieldMap.destinationHeader();
  // Create a primary key if needed
  if(pkFields.isEmpty()){
    // Create primary key field
    field = QSqlField();
    field.setName("id_PK");
    field.setType(QVariant::Int);
    field.setAutoValue(true);
    table.addField(field, true);
  }else{
    // Primary key is one of the CSV header
    for(i = 0; i < pkFields.size(); ++i){
      Q_ASSERT(header.contains(pkFields.at(i)));
    }
  }
  // Create database fields
  fields = pvFieldMap.mappedDestinationFields();
  for(i = 0; i < fields.size(); ++i){
    // Check if field is part of primary key and add it to database table
    if(pkFields.contains(fields.at(i).displayText())){
      table.addField(fields.at(i).sqlField(), true);
    }else{
      table.addField(fields.at(i).sqlField(), false);
    }
  }
  // Create data set
  close();
  pvDataSetName = fileInfo.baseName();
  if(!createDataSet(dbDir, pvDataSetName, table, mode)){
    csvFile.close();
    return false;
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
  pvAbort = false;
  while(csvFile.hasMoreLines()){
    // Check if we have to abort
    if(pvAbort){
      csvFile.close();
      close();
      return false;
    }
    // Read a line in CSV file
    line = csvFile.readLine(pvCsvSeparator, pvCsvDataProtection, pvCsvComment, pvCsvEscapeChar, pvCsvEol);
    // On some parse error, it can happen that we have to much columns
    // Common known issue is the ending ; with no more data, but we don't loose data in this case.
    while(line.size() > table.fieldCount()){
      if(!line.last().isEmpty()){
        dataLossCount++;
      }
      line.removeLast();
    }
    rows.append(pvFieldMap.destinationDataRow(line));
    if(lineCount > 100){
      if(!commitRowsToDatabase(rows, fields, autoValuePkFieldName)){
        csvFile.close();
        close();
        pvLastError.commit();
        return false;
      }
      // Update progress dialog
      if(progress != 0){
        totalLineCount += rows.size();
        progress->setLabelText(tr("Importing line ") + QString::number(totalLineCount));
        qApp->processEvents();
      }
      rows.clear();
      lineCount=0;
    }
    lineCount++;
  }
  // Commit last lines
  if(!commitRowsToDatabase(rows, fields, autoValuePkFieldName)){
    csvFile.close();
    close();
    pvLastError.commit();
    return false;
  }
  csvFile.close();
  if(progress != 0){
    delete progress;
  }
  // Log a warning if some data were lost
  if(dataLossCount > 0){
    pvLastError.setError(tr("Some data has been lost during import of CSV file: ") + fileInfo.absoluteFilePath(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
  }

  return true;
}

bool mdtDataTableManager::copyTable(const QString & sourceTableName, const QString & destinationTableName, mdtSqlDatabaseManager::createMode_t mode, QSqlDatabase sourceDatabase, QSqlDatabase destinationDatabase)
{
  Q_ASSERT(sourceDatabase.isOpen());
  Q_ASSERT(destinationDatabase.isOpen());

  QSqlRecord record;
  QList<mdtFieldMapField> sourceFields, destinationFields;
  QList<mdtFieldMapItem*> mapItems;
  mdtFieldMapItem *mapItem;
  int i, sourceFieldIndex, destinationFieldIndex;
  mdtSqlSchemaTable destinationTable;
  QSqlIndex pkFields;
  QProgressDialog *progress = 0;
  int lineCount, totalLineCount;
  QList<QVariant> rowData;
  QList<QVariantList> rows;
  QString sql;
  QSqlError sqlError;
  QString errorText;

  // Build source fields list
  record = sourceDatabase.record(sourceTableName);
  for(i = 0; i < record.count(); ++i){
    mdtFieldMapField field;
    field.setSqlField(record.field(i));
    sourceFields.append(field);
  }
  pvFieldMap.setSourceFields(sourceFields);
  // Create or update field mapping regarding allready set-up map
  destinationFieldIndex = 0;
  for(sourceFieldIndex = 0; sourceFieldIndex < sourceFields.size(); ++sourceFieldIndex){
    mapItems = pvFieldMap.itemsAtSourceFieldName(sourceFields.at(sourceFieldIndex).name());
    if(mapItems.isEmpty()){
      // No mapping for this source item, generate one
      mapItem = new mdtFieldMapItem;
      mdtFieldMapField field;
      field.setSqlField(sourceFields.at(sourceFieldIndex).sqlField());
      mapItem->setSourceField(field);
      mapItem->setDestinationField(field);
      pvFieldMap.addItem(mapItem);
      mapItems.append(mapItem);
    }
    // Set indexes
    for(i = 0; i < mapItems.size(); ++i){
      mapItem = mapItems.at(i);
      Q_ASSERT(mapItem != 0);
      mapItem->setSourceFieldIndex(sourceFieldIndex);
      mapItem->setDestinationFieldIndex(destinationFieldIndex);
      destinationFieldIndex++;
    }
  }
  sourceFields = pvFieldMap.mappedSourceFields();
  // Create destination table
  pkFields = sourceDatabase.primaryIndex(sourceTableName);
  destinationFields = pvFieldMap.mappedDestinationFields();
  destinationTable.setTableName(destinationTableName);
  pvDataSetTableName = destinationTableName;
  for(i = 0; i < destinationFields.size(); ++i){
    // Check if field is part of primary key and add it to database table
    if(pkFields.contains(destinationFields.at(i).name())){
      destinationTable.addField(destinationFields.at(i).sqlField(), true);
    }else{
      destinationTable.addField(destinationFields.at(i).sqlField(), false);
    }
  }
  pvDatabaseManager->setDatabase(destinationDatabase);
  if(!pvDatabaseManager->createTable(destinationTable, mode)){
    pvLastError = pvDatabaseManager->lastError();
    return false;
  }
  // Prepare copy
  if(pvProgressDialogEnabled){
    totalLineCount = 0;
    progress = new QProgressDialog;
    progress->setWindowTitle(tr("Database table copy progress"));
    progress->setMinimum(0);
    progress->setMaximum(0);
    connect(progress, SIGNAL(canceled()), this, SLOT(setAbortFlag()));
    progress->show();
  }
  sql = "SELECT ";
  sourceFields = pvFieldMap.mappedSourceFields();
  for(i = 0; i < sourceFields.size(); ++i){
    sql += sourceFields.at(i).name();
    if(i < (sourceFields.size() - 1)){
      sql += ",";
    }
  }
  sql += "\n FROM '" + sourceTableName +"'";
  QSqlQuery sourceDataQuery(sourceDatabase);
  if(!sourceDataQuery.exec(sql)){
    sqlError = sourceDataQuery.lastError();
    pvLastError.setError(tr("Unable to get source data from table '") + sourceTableName + tr("' for copy."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }
  // Copy data
  lineCount = 0;
  pvAbort = false;
  while(sourceDataQuery.next()){
    // Check if we have to abort
    if(pvAbort){
      close();
      return false;
    }
    // Get a row od data from source
    record = sourceDataQuery.record();
    rowData.clear();
    for(i = 0; i < record.count(); ++i){
      rowData.append(sourceDataQuery.value(i));
    }
    // Copy row
    rows.append(pvFieldMap.destinationDataRow(rowData));
    if(lineCount > 100){
      if(!commitRowsToDatabase(rows, destinationFields, "")){
        close();
        errorText = pvLastError.text();
        errorText += " (Source table: " + sourceTableName + " , destination table: " + destinationTableName + ")";
        pvLastError.updateText(errorText);
        pvLastError.commit();
        return false;
      }
      // Update progress dialog
      if(progress != 0){
        totalLineCount += rows.size();
        progress->setLabelText(tr("Importing row ") + QString::number(totalLineCount));
        qApp->processEvents();
      }
      rows.clear();
      lineCount = 0;
    }
    lineCount++;
  }
  // Commit last lines
  if(!commitRowsToDatabase(rows, destinationFields, "")){
    errorText = pvLastError.text();
    errorText += " (Source table: " + sourceTableName + " , destination table: " + destinationTableName + ")";
    pvLastError.updateText(errorText);
    pvLastError.commit();
    close();
    return false;
  }
  if(progress != 0){
    delete progress;
  }

  return true;
}

void mdtDataTableManager::addFieldMapping(const QString &csvHeaderItem, const QString &fieldName, const QString &displayText, QVariant::Type dataType, int csvDataItemStartOffset, int csvDataItemEndOffset)
{
  mdtFieldMapItem *item;

  item = new mdtFieldMapItem;
  if(item == 0){
    pvLastError.setError(tr("Unable to create a mdtFieldMapItem (memory allocation failed)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return;
  }
  item->setSourceFieldName(csvHeaderItem);
  item->setDestinationFieldName(fieldName);
  item->setDestinationFieldDisplayText(displayText);
  item->setDataType(dataType);
  item->setSourceFieldDataStartOffset(csvDataItemStartOffset);
  item->setSourceFieldDataEndOffset(csvDataItemEndOffset);
  pvFieldMap.addItem(item);
}

void mdtDataTableManager::clearFieldMap()
{
  pvFieldMap.clear();
}

QStringList mdtDataTableManager::sourceHeader() const
{
  return pvFieldMap.sourceHeader();
}

const QStringList mdtDataTableManager::databaseHeader() const
{
  return pvDatabaseHeader;
}

void mdtDataTableManager::setDatabaseHeaderToModel(QAbstractTableModel *model)
{
  Q_ASSERT(model != 0);

  int i;

  for(i = 0; i < pvDatabaseHeader.size(); ++i){
    if(!model->setHeaderData(i, Qt::Horizontal, pvDatabaseHeader.at(i))){
      // Possibly out of range
      return;
    }
  }
}

QHash<QString, QString> mdtDataTableManager::displayTextsByFieldNames() const
{
  return pvFieldMap.displayTextsByFieldNames();
}

void mdtDataTableManager::setAbortFlag()
{
  pvAbort = true;
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

bool mdtDataTableManager::commitRowsToDatabase(const QList<QVariantList> &rows, const QList<mdtFieldMapField> &fields, const QString & autoValuePkFieldName)
{
  QSqlDatabase db = pvDatabaseManager->database();
  QSqlQuery query(db);
  QString sql;
  QSqlError sqlError;
  int row, col, autoValuePkFieldCount;

  // Begin transaction (without, insertions are very slow)
  if(!db.transaction()){
    pvLastError.setError(tr("Unable to begin transaction, data set: ") + pvDataSetName, mdtError::Error);
    pvLastError.setSystemError(db.lastError().number(), db.lastError().text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }
  // Prepare query for insertion
  sql = insertBindValuesPrepareStatement(fields, autoValuePkFieldName);
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for insertion in database", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    db.rollback();
    return false;
  }
  // Set data
  for(row = 0; row < rows.size(); ++row){
    // Add auto generated PK
    if(autoValuePkFieldName.isEmpty()){
      autoValuePkFieldCount = 0;
    }else{
      autoValuePkFieldCount = 1;
      query.bindValue(0, QVariant());
    }
    for(col = 0; col < rows.at(row).size(); ++col){
      query.bindValue(col + autoValuePkFieldCount, rows.at(row).at(col));
    }
    // Execute query
    if(!query.exec()){
      sqlError = query.lastError();
      pvLastError.setError("Cannot execute query for insertion in database", mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
      ///pvLastError.commit();
      db.rollback();
      return false;
    }
  }
  // Commit transaction
  if(!db.commit()){
    pvLastError.setError(tr("Unable to commit transaction, data set: ") + pvDataSetName, mdtError::Error);
    pvLastError.setSystemError(db.lastError().number(), db.lastError().text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtDataTableManager");
    pvLastError.commit();
    return false;
  }

  return true;
}

const QString mdtDataTableManager::insertBindValuesPrepareStatement(const QList<mdtFieldMapField> &fields, const QString & autoValuePkFieldName) const
{
  QString sql;
  int i;

  sql = "INSERT INTO " + pvDataSetTableName + " (";
  if(!autoValuePkFieldName.isEmpty()){
    sql += autoValuePkFieldName + ",";
  }
  for(i = 0; i < fields.size(); ++i){
    sql += fields.at(i).name();
    if(i < (fields.size() - 1)){
      sql += ",";
    }
  }
  sql += ")";

  sql += " VALUES(";
  if(!autoValuePkFieldName.isEmpty()){
    sql += "?,";
  }
  for(i = 0; i < fields.size(); ++i){
    sql += "?";
    if(i < (fields.size() - 1)){
      sql += ",";
    }
  }
  sql += ")";

  return sql;
}
