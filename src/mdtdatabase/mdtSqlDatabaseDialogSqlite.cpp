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
#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtSqlDatabaseSqlite.h"
#include "mdtSqlConnectionNameWidget.h"
#include "mdtErrorDialog.h"
#include <QComboBox>
#include <QToolButton>
#include <QDir>
#include <QSqlError>
#include <QFileDialog>
#include <QMessageBox>
// #include <QInputDialog>

#include <QDebug>

mdtSqlDatabaseDialogSqlite::mdtSqlDatabaseDialogSqlite(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  connect(wConnectionName, &mdtSqlConnectionNameWidget::currentDatabaseChanged, this, &mdtSqlDatabaseDialogSqlite::updateDatabase);
  connect(wConnectionName, &mdtSqlConnectionNameWidget::aboutToRemoveDatabaseConnection, this, &mdtSqlDatabaseDialogSqlite::maybeClearDatabase);
  wConnectionName->setDriverType(mdtSqlDriverType::SQLite); // Will also update connections names
  connect(tbClearDatabaseName, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::clearDatabaseName);
  connect(tbChooseDatabaseFile, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::chooseDatabaseFile);
  connect(tbOpenDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::openDatabase);
  connect(tbCloseDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::closeDatabase);
  connect(tbCreateDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::createDatabase);
  connect(tbDeleteDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::deleteDatabase);
}

void mdtSqlDatabaseDialogSqlite::setNonEditableConnectionNames(const QStringList & names)
{
  pvNonEditableConnectionNames = names;
}

void mdtSqlDatabaseDialogSqlite::addNonEditableConnectionName(const QString & name)
{
  pvNonEditableConnectionNames.append(name);
}

void mdtSqlDatabaseDialogSqlite::updateConnectionsList()
{
  wConnectionName->updateConnectionsList();
}

void mdtSqlDatabaseDialogSqlite::setDatabase(const mdtSqlDatabaseSqlite & db)
{
  wConnectionName->setCurrentConnection(db.database().connectionName());
}

void mdtSqlDatabaseDialogSqlite::setDefaultDirectory(const QString& directory)
{
  pvDefaultDirectory = directory;
}

void mdtSqlDatabaseDialogSqlite::updateDatabase(const QSqlDatabase & db)
{
  if(db.isValid()){
    pvDatabase.setDatabase(db);
  }else{
    pvDatabase.clear();
  }
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::maybeClearDatabase(const QString & connectionName)
{
  if(pvDatabase.database().connectionName() == connectionName){
    pvDatabase.clear();
  }
}

void mdtSqlDatabaseDialogSqlite::chooseDatabaseFile()
{
  QFileInfo fileInfo;
  QFileDialog dialog(this);

  // Let the user choose a database file
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  if(!pvDefaultDirectory.isEmpty()){
    dialog.setDirectory(pvDefaultDirectory);
  }
  dialog.setNameFilter(tr("(Sqlite) Database file (*.sqlite *.db)"));
  dialog.setFileMode(QFileDialog::ExistingFile);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));
  pvDatabase.clearDatabaseName();
  // Check that selected database is not allready referenced by a connection
  QString cnn = pvDatabase.getConnectionNameUsingDatabase(fileInfo);
  if(!cnn.isEmpty()){
    warnAboutDatabaseReferencedByConnection(fileInfo, cnn);
    return;
  }
  // Update database name
  if(!pvDatabase.setDatabaseFile(fileInfo)){
    displayError(pvDatabase.lastError());
    return;
  }
  // Update dialog
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::openDatabase()
{
  Q_ASSERT(!pvDatabase.isOpen());

  if(pvDatabase.database().databaseName().isEmpty()){
//     QFileInfo fileInfo;
//     QFileDialog dialog(this);
//     // Let the user choose a database file
//     dialog.setAcceptMode(QFileDialog::AcceptOpen);
//     if(!pvDefaultDirectory.isEmpty()){
//       dialog.setDirectory(pvDefaultDirectory);
//     }
//     dialog.setNameFilter(tr("(Sqlite) Database file (*.sqlite *.db)"));
//     dialog.setFileMode(QFileDialog::ExistingFile);
//     if(dialog.exec() != QDialog::Accepted){
//       return;
//     }
//     Q_ASSERT(dialog.selectedFiles().size() == 1);
//     fileInfo.setFile(dialog.selectedFiles().at(0));
//     // Check that selected database is not allready referenced by a connection
//     QString cnn = pvDatabase.getConnectionNameUsingDatabase(fileInfo);
//     if(!cnn.isEmpty()){
//       warnAboutDatabaseReferencedByConnection(fileInfo, cnn);
//       return;
//     }
    chooseDatabaseFile();
    if(pvDatabase.database().databaseName().isEmpty()){
      return;
    }
    // Open database
    if(!pvDatabase.openDatabase()){
      displayError(pvDatabase.lastError());
      return;
    }
  }else{
    // Re-open allready set database
    if(!pvDatabase.openDatabase()){
      displayError(pvDatabase.lastError());
      return;
    }
  }
  // Setup and show dialog
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::clearDatabaseName()
{
  Q_ASSERT(!pvDatabase.isOpen());

  pvDatabase.clearDatabaseName();
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::createDatabase()
{
  Q_ASSERT(!pvDatabase.isOpen());

  QFileDialog dialog(this);
  QFileInfo fileInfo;

  // Setup and show dialog
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if(!pvDefaultDirectory.isEmpty()){
    dialog.setDirectory(pvDefaultDirectory);
  }
  dialog.setNameFilter(tr("(Sqlite) Database file (*.sqlite *.db)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));
  // Check that selected database is not allready referenced by a connection
  QString cnn = pvDatabase.getConnectionNameUsingDatabase(fileInfo);
  if(!cnn.isEmpty()){
    warnAboutDatabaseReferencedByConnection(fileInfo, cnn);
    return;
  }
  // Create database
  if(!pvDatabase.createDatabase(fileInfo, mdtSqlDatabaseSqlite::OverwriteExisting)){
    displayError(pvDatabase.lastError());
    return;
  }
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::closeDatabase()
{
  pvDatabase.close();
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::deleteDatabase()
{
  Q_ASSERT(!pvDatabase.isOpen());

  QString msg;

  // Ask confirmation
  QMessageBox msgBox(this);
  msgBox.setText(tr("Delete database file."));
  msg = tr("File") + " '" + pvDatabase.database().databaseName() + "' " + tr("will be deleted.") + "\n";
  msg += tr("Do you want to continue ?");
  msgBox.setInformativeText(msg);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setIcon(QMessageBox::Warning);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete database
  if(!pvDatabase.deleteDatabase(pvDatabase.database().databaseName())){
    displayError(pvDatabase.lastError());
    return;
  }
  updateDatabaseInformations();
  updateButtonsState();
}

void mdtSqlDatabaseDialogSqlite::updateDatabaseInformations()
{
  if(!pvDatabase.isValid()){
    lbFileName->clear();
    lbDirectory->clear();
    return;
  }
  QFileInfo fileInfo(pvDatabase.database().databaseName());
  lbFileName->setText(fileInfo.fileName());
  if(fileInfo.fileName().isEmpty()){
    lbDirectory->clear();
  }else{
    lbDirectory->setText(fileInfo.dir().absolutePath());
  }
}

void mdtSqlDatabaseDialogSqlite::updateButtonsState()
{
  if(pvNonEditableConnectionNames.contains(pvDatabase.database().connectionName())){
    tbOpenDatabase->setEnabled(false);
    tbClearDatabaseName->setEnabled(false);
    tbCloseDatabase->setEnabled(false);
    tbCreateDatabase->setEnabled(false);
    tbDeleteDatabase->setEnabled(false);
  }else if(pvDatabase.isOpen()){
    tbOpenDatabase->setEnabled(false);
    tbClearDatabaseName->setEnabled(false);
    tbCloseDatabase->setEnabled(true);
    tbCreateDatabase->setEnabled(false);
    tbDeleteDatabase->setEnabled(false);
  }else{
    tbOpenDatabase->setEnabled(true);
    if(pvDatabase.database().databaseName().isEmpty()){
      tbClearDatabaseName->setEnabled(false);
      tbDeleteDatabase->setEnabled(false);
    }else{
      tbClearDatabaseName->setEnabled(true);
      tbDeleteDatabase->setEnabled(true);
    }
    tbCloseDatabase->setEnabled(false);
    tbCreateDatabase->setEnabled(true);
  }
  wConnectionName->updateState(pvDatabase.database());
}

void mdtSqlDatabaseDialogSqlite::displayError(const mdtError & error)
{
  mdtErrorDialog dialog(error, this);
  dialog.exec();
}

void mdtSqlDatabaseDialogSqlite::warnAboutDatabaseReferencedByConnection(const QFileInfo & fileInfo, const QString& connectionName)
{
  QMessageBox msgBox(this);
  QString msg;

  msg = tr("Selected database is allready referenced by a connection.\n");
  msg += tr("File: ") + fileInfo.fileName() + tr("\n");
  msg += tr("Directory: ") + fileInfo.absoluteDir().path() + tr("\n");
  msg += tr("Referenced by: ") + connectionName;
  msgBox.setText(msg);
  msgBox.setInformativeText(tr("Use connection that refers to this database, or chose a other database."));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}
