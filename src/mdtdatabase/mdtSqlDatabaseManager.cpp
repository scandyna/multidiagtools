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
#include "mdtSqlDatabaseManager.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlForeignKeySetting.h"
#include <QWidget>
#include <QFile>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>

//#include <QDebug>

mdtSqlDatabaseManager::mdtSqlDatabaseManager(QObject *parent)
 : QObject(parent)
{
  pvForDialogParentWidget = 0;
}

mdtSqlDatabaseManager::~mdtSqlDatabaseManager() 
{
}

void mdtSqlDatabaseManager::setDatabase(QSqlDatabase db) 
{
  pvDatabase = db;
  pvDatabaseName = pvDatabase.databaseName();
}

QSqlDatabase mdtSqlDatabaseManager::database() 
{
  return pvDatabase;
}

// bool mdtSqlDatabaseManager::setForeignKeysEnabled(bool enable)
// {
//   QString sql;
// 
//   // Check that database is open
//   if(!pvDatabase.isOpen()){
//     if(enable){
//       pvLastError.setError(tr("Cannot enable foreign keys contraint, because database is closed."), mdtError::Error);
//     }else{
//       pvLastError.setError(tr("Cannot disable foreign keys contraint, because database is closed."), mdtError::Error);
//     }
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
//     return false;
//   }
//   // Build query
//   QSqlQuery query(pvDatabase);
//   if(pvDatabase.driverName() == "QSQLITE"){
//     if(enable){
//       sql = "PRAGMA foreign_keys = ON";
//     }else{
//       sql = "PRAGMA foreign_keys = OFF";
//     }
//   }else{
//     if(enable){
//       pvLastError.setError(tr("Cannot enable foreign keys contraint, because database type is not supported."), mdtError::Error);
//     }else{
//       pvLastError.setError(tr("Cannot disable foreign keys contraint, because database type is not supported."), mdtError::Error);
//     }
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
//     return false;
//   }
//   // Exec query
//   if(!query.exec(sql)){
//     pvLastError.setError(tr("Cannot enable/disable foreign keys support on database '") + pvDatabaseName + "'", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
//     return false;
//   }
// 
//   return true;
// }

void mdtSqlDatabaseManager::setForDialogParentWidget(QWidget *widget) 
{
  pvForDialogParentWidget = widget;
}

mdtError mdtSqlDatabaseManager::lastError() const 
{
  return pvLastError;
}

QFileInfo mdtSqlDatabaseManager::chooseDatabaseSqlite(const QDir & startDirectory) 
{
  QFileDialog dialog(pvForDialogParentWidget);
  QFileInfo fileInfo;

  // Setup and show dialog
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(startDirectory);
  dialog.setNameFilter(tr("(Sqlite) Database file (*.sqlite)"));
  dialog.setFileMode(QFileDialog::ExistingFile);
  if(dialog.exec() != QDialog::Accepted){
    return fileInfo;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));

  return fileInfo;
}

bool mdtSqlDatabaseManager::openDatabaseSqlite(const QFileInfo & fileInfo, const QString & connectionName) 
{
  // Check if database is allready open
  if(databaseIsOpen(false)){
    return false;
  }
  Q_ASSERT(!pvDatabase.isOpen());
  // Check if file exists
  if(!fileInfo.exists()){
    pvLastError.setError(tr("File '") + fileInfo.fileName() + tr("' does not exists.") + "\n"\
                           + tr("Directory: '") + fileInfo.absoluteDir().path() + tr("'."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
    pvLastError.commit();
    return false;
  }
  // Store the database name
  pvDatabaseName = fileInfo.absoluteFilePath();
  // Try to open database
  if(connectionName.isEmpty()){
    if(QSqlDatabase::contains()){
      pvDatabase = QSqlDatabase::database();
    }else{
      pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
    }
  }else{
    if(QSqlDatabase::contains(connectionName)){
      pvDatabase = QSqlDatabase::database(connectionName);
    }else{
      pvDatabase = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }
  }
  if(!pvDatabase.isValid()){
//     pvLastError = mdtError(tr("Cannot create database connection (probably plugin/driver problem)"), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    return false;
  }
  if(pvDatabase.driverName() != "QSQLITE"){
//     pvLastError = mdtError(tr("Requested connection allready exists but is not for Sqlite."), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
    return false;
  }
  pvDatabase.setDatabaseName(pvDatabaseName);
  if(!pvDatabase.open()){
//     pvLastError = mdtError(tr("Cannot open database '") + pvDatabaseName + "'", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setInformativeText(tr("Check that you have write access to directory '") + fileInfo.absoluteDir().path() + tr("'."));
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    return false;
  }
  Q_ASSERT(pvDatabase.isValid());
  Q_ASSERT(pvDatabase.isOpen());
  // Enable foreign key support
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Permanent);
  if(!fkSetting.enable()){
//   if(!setForeignKeysEnabled(true)){
    /// \todo Close here ??
    return false;
  }
  QSqlQuery query(pvDatabase);
  // At default, synchronous write is set to FULL. This is safe, but really slow..
  if(!query.exec("PRAGMA synchronous = NORMAL")){
//     pvLastError.setError(tr("Cannot set PRAGMA synchronous to NORMAL (its probably set to FULL now). Database: '") + pvDatabaseName + "'", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    /// \todo Error ? Close here ?
    return false;
  }

  return true;
}

bool mdtSqlDatabaseManager::openDatabaseSqlite(const QDir & startDirectory, const QString & connectionName) 
{
  QFileInfo fileInfo;

  // Check if database is allready open
  if(databaseIsOpen(true)){
    return false;
  }
  // Let the user choose the sqlite file
  fileInfo = chooseDatabaseSqlite(startDirectory);
  if(fileInfo.fileName().isEmpty()){
    pvLastError.clear();
    return false;
  }
  // Try to open database
  return openDatabaseSqlite(fileInfo, connectionName);
}

bool mdtSqlDatabaseManager::createDatabaseSqlite(const QDir & startDirectory, const QString & connectionName) 
{
  QFileDialog dialog(pvForDialogParentWidget);
  QFileInfo fileInfo;

  // Check if database is allready open
  if(databaseIsOpen(true)){
    return false;
  }
  // Setup and show dialog
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDirectory(startDirectory);
  dialog.setNameFilter(tr("(Sqlite) Database file (*.sqlite)"));
  dialog.setConfirmOverwrite(true);
  if(dialog.exec() != QDialog::Accepted){
    return false;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));
  // Create database
  if(!createDatabaseSqlite(fileInfo, OverwriteExisting, connectionName)){
    QMessageBox msgBox(pvForDialogParentWidget);
    msgBox.setText(pvLastError.text());
    msgBox.setInformativeText(pvLastError.informativeText());
//     msgBox.setDetailedText(pvLastError.systemErrorString(&msgBox));
    ///msgBox.setIcon(pvLastError.levelIcon());
    msgBox.setWindowTitle(tr("Error during database creation."));
    msgBox.exec();
    return false;
  }

  return true;
}

bool mdtSqlDatabaseManager::createDatabaseSqlite(const QFileInfo & fileInfo, createMode_t createMode, const QString & connectionName) 
{
  QFile file;

  // Check if directory exists
  if(!fileInfo.absoluteDir().exists()){
//     pvLastError = mdtError(tr("Directory '") + fileInfo.absoluteDir().path() + tr("' does not exists.") , mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
    return false;
  }
  // Store the database name
  pvDatabaseName = fileInfo.absoluteFilePath();
  /*
   * If database that was requested with connectionName (or maybe default) is allready open,
   *  we can only success for KeepExisting mode.
   * We cannot simply close the database (it's possibly used somewhere by the application).
   * This can happen if caller forgot to close connection, or give a wrong connection name.
   */
  if(pvDatabase.isOpen()){
    // We are allready connected to the database that we want to create
    switch(createMode){
      case KeepExisting:
        return true;
      case FailIfExists:
      case OverwriteExisting:
      case AskUserIfExists:
        pvLastError.setError(tr("Current database handle is allready open, "\
                                "must be closed before a new database can be created "\
                                "(connection name: ") + pvDatabase.connectionName() + tr(")."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
        pvLastError.commit();
        return false;
    }
  }
  Q_ASSERT(!pvDatabase.isOpen());
  // Here, we know that database is not open, now check if file exists
  if(fileInfo.exists()){
    switch(createMode){
      case KeepExisting:
        break;
      case FailIfExists:
//         pvLastError = mdtError(tr("Database '") + fileInfo.fileName() + tr("' allready exists") , mdtError::Error);
//         MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//         pvLastError.commit();
        return false;
      case OverwriteExisting:
        file.setFileName(fileInfo.absoluteFilePath());
        if(!file.resize(0)){
//           pvLastError = mdtError(tr("File truncate failed for file '") + fileInfo.absoluteFilePath() + "'", mdtError::Error);
//           MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//           pvLastError.setSystemError(file.error(), file.errorString());
//           pvLastError.commit();
          return false;
        }
        break;
      case AskUserIfExists:
        if(userChooseToOverwriteFile(fileInfo)){
          file.setFileName(fileInfo.absoluteFilePath());
          if(!file.resize(0)){
//             pvLastError = mdtError(tr("File truncate failed for file '") + fileInfo.absoluteFilePath() + "'", mdtError::Error);
//             MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//             pvLastError.setSystemError(file.error(), file.errorString());
//             pvLastError.commit();
            return false;
          }
        }
        break;
    }
  }
  // Try to open database
  if(connectionName.isEmpty()){
    if(QSqlDatabase::contains()){
      pvDatabase = QSqlDatabase::database();
    }else{
      pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
    }
  }else{
    if(QSqlDatabase::contains(connectionName)){
      pvDatabase = QSqlDatabase::database(connectionName);
    }else{
      pvDatabase = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }
  }
  if(!pvDatabase.isValid()){
//     pvLastError = mdtError(tr("Cannot create database connection (probably plugin/driver problem)"), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    return false;
  }
  if(pvDatabase.driverName() != "QSQLITE"){
//     pvLastError = mdtError(tr("Requested connection allready exists but is not for Sqlite."), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
    return false;
  }
  pvDatabase.setDatabaseName(pvDatabaseName);
  if(!pvDatabase.open()){
//     pvLastError = mdtError(tr("Cannot open database '") + pvDatabaseName + "'", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setInformativeText(tr("Check that you have write access to directory '") + fileInfo.absoluteDir().path() + tr("'."));
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    return false;
  }
  Q_ASSERT(pvDatabase.isValid());
  Q_ASSERT(pvDatabase.isOpen());
  // Enable foreign key support
  /**
  QSqlQuery query(pvDatabase);
  if(!query.exec("PRAGMA foreign_keys = ON")){
    pvLastError.setError(tr("Cannot enable foreign keys support on database '") + pvDatabaseName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
    pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
    pvLastError.commit();
    return false;
  }
  */
  // Enable foreign key support
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Permanent);
  if(!fkSetting.enable()){
//   if(!setForeignKeysEnabled(true)){
    /// \todo Close here ??
    return false;
  }
  QSqlQuery query(pvDatabase);
  // At defoult, synchronous write is set to FULL. This is safe, but really slow..
  if(!query.exec("PRAGMA synchronous = NORMAL")){
//     pvLastError.setError(tr("Cannot set PRAGMA synchronous to NORMAL (its probably set to FULL now). Database: '") + pvDatabaseName + "'", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
//     pvLastError.commit();
    /// \todo Error ? Close here ?
    return false;
  }

  return true;
}

bool mdtSqlDatabaseManager::createTable(mdtSqlTableSchema & table, createMode_t createMode) 
{
  QString sql;
  QSqlError sqlError;

  if(!pvDatabase.isOpen()){
//     pvLastError.setError(tr("Cannot create table because database is closed."), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
//     pvLastError.commit();
    return false;
  }
  QSqlQuery query(pvDatabase);
  table.setDriverName(pvDatabase.driverName());
  // Check if table allready exists
  if(pvDatabase.tables().contains(table.tableName())){
    switch(createMode){
      case KeepExisting:
        pvLastError.setError(tr("Table '") + table.tableName()\
                             + tr("' allready exists and choose is to keep it.") + "\n"\
                             + tr("It is possible that the structure does not match expected one."), mdtError::Warning);
        MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
        pvLastError.commit();
        return true;
      case FailIfExists:
        pvLastError.setError(tr("Table '") + table.tableName() + tr("' allready exists."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
        pvLastError.commit();
        return false;
      case AskUserIfExists:
        if(!userChooseToOverwriteTable(table.tableName())){
          return false;
        }
        /// \todo Should drop table here ?
        break;
      case OverwriteExisting:
        sql = table.sqlForDropTable();
        if(!query.exec(sql)){
          sqlError = query.lastError();
          pvLastError.setError(tr("Cannot drop table '") + table.tableName() + tr("'"), mdtError::Error);
          MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
          pvLastError.setSystemError(sqlError.number(), sqlError.text());
          pvLastError.commit();
          return false;
        }
        break;
    }
  }
  // Create table
  sql = table.sqlForCreateTable();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError(tr("Cannot create table '") + table.tableName() + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtSqlDatabaseManager::userChooseToOverwriteTable(const QString &tableName)
{
  QMessageBox msgBox(pvForDialogParentWidget);
  QString info;
  int retVal;

  msgBox.setText(tr("A table allready exists in database."));
  info = tr("Table: ") + tableName + "\n";
  info += tr("\nDo you want to overwrite this table ?");
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

bool mdtSqlDatabaseManager::userChooseToOverwriteFile(const QFileInfo &fileInfo)
{
  QMessageBox msgBox(pvForDialogParentWidget);
  QString info;
  int retVal;

  msgBox.setText(tr("A file allready exists in destination directory."));
  info = tr("File: ") + fileInfo.fileName() + "\n";
  info += tr("Directory: ") + fileInfo.absoluteDir().path() + "\n";
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

bool mdtSqlDatabaseManager::databaseIsOpen(bool askUser)
{
  QString text;

  if(!pvDatabase.isOpen()){
    return false;
  }
  text = tr("Current database handle is allready open, must be closed before a new database can be created "\
            "(connection name: ") + pvDatabase.connectionName() + tr(").");
  if(askUser){
    QMessageBox msgBox(pvForDialogParentWidget);
    msgBox.setText(text);
    msgBox.setInformativeText(tr("Do you want to close currently open database handle ?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(tr("Database handle allready open"));
    if(msgBox.exec() == QMessageBox::Yes){
      pvDatabase.close();
    }
  }
  if(pvDatabase.isOpen()){
    pvLastError.setError(text, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseManager");
    pvLastError.commit();
    return true;
  }

  return false;
}
