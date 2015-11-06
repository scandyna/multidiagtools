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
#include "mdtSqlDatabaseCopierDialog.h"
#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtSqlDatabaseCopierMappingModel.h"
#include "mdtSqlDatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierThread.h"
#include "mdtProgressBarItemDelegate.h"
#include "mdtAlgorithms.h"
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTableView>
#include <QCloseEvent>

mdtSqlDatabaseCopierDialog::mdtSqlDatabaseCopierDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierMappingModel),
   pvThread(new mdtSqlDatabaseCopierThread)
{
  setupUi(this);

  tvMapping->setModel(pvMappingModel);
  auto *progressBarDelegate = new mdtProgressBarItemDelegate(tvMapping);
  tvMapping->setItemDelegateForColumn(3, progressBarDelegate);
  connect(tvMapping, &QTableView::doubleClicked, this, &mdtSqlDatabaseCopierDialog::editTableMapping);
  connect(tbSelectSourceDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::selectSourceDatabase);
  connect(tbSelectDestinationDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::selectDestinationDatabase);
  connect(tbResetMapping, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::resetMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::mapByName);
  connect(pbCopy, &QPushButton::clicked, this, &mdtSqlDatabaseCopierDialog::copyData);
  connect(pbAbort, &QPushButton::clicked, pvThread, &mdtSqlDatabaseCopierThread::abort);
  connect(pvThread, &mdtSqlDatabaseCopierThread::tableCopyProgressChanged, pvMappingModel, &mdtSqlDatabaseCopierMappingModel::setTableCopyProgress);
  connect(pvThread, &mdtSqlDatabaseCopierThread::tableCopyStatusChanged, pvMappingModel, &mdtSqlDatabaseCopierMappingModel::setTableCopyStatus);
  connect(pvThread, &mdtSqlDatabaseCopierThread::tableCopyErrorOccured, pvMappingModel, &mdtSqlDatabaseCopierMappingModel::setTableCopyError);
  connect(pvThread, &mdtSqlDatabaseCopierThread::globalProgressRangeChanged, pbGlobalProgress, &QProgressBar::setRange );
  connect(pvThread, &mdtSqlDatabaseCopierThread::globalProgressValueChanged, pbGlobalProgress, &QProgressBar::setValue);
  connect(pvThread, &mdtSqlDatabaseCopierThread::globalErrorOccured, this, &mdtSqlDatabaseCopierDialog::onThreadGlobalErrorOccured);
  connect(pvThread, &mdtSqlDatabaseCopierThread::finished, this, &mdtSqlDatabaseCopierDialog::onThreadFinished);
  setStateDatabasesNotSet();
}

mdtSqlDatabaseCopierDialog::~mdtSqlDatabaseCopierDialog()
{
  delete pvThread;
  delete pvMappingModel;
  if(!pvOwningSourceConnectionName.isEmpty()){
    QSqlDatabase::removeDatabase(pvOwningSourceConnectionName);
  }
  if(!pvOwningDestinationConnectionName.isEmpty()){
    QSqlDatabase::removeDatabase(pvOwningDestinationConnectionName);
  }
}

void mdtSqlDatabaseCopierDialog::initSourceDatabase(mdtSqlDriverType::Type driverType)
{
  Q_ASSERT(pvState != ProcessingCopy);
  Q_ASSERT(pvOwningSourceConnectionName.isEmpty());

  QSqlDatabase db;

  pvOwningSourceConnectionName = mdtAlgorithms::generateString(6, QSqlDatabase::connectionNames());
  db = QSqlDatabase::addDatabase(mdtSqlDriverType::nameFromType(driverType), pvOwningSourceConnectionName);
  setSourceDatabasePv(db);
}

void mdtSqlDatabaseCopierDialog::setSourceDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);
  Q_ASSERT(pvOwningSourceConnectionName.isEmpty());

  setSourceDatabasePv(db);
}

void mdtSqlDatabaseCopierDialog::setDestinationDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);

  wDestinationDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setDestinationDatabase(db)){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
  setStateDatabasesSetOrNotSet();
}

void mdtSqlDatabaseCopierDialog::selectSourceDatabase()
{
  Q_ASSERT(pvState != ProcessingCopy);

  mdtSqlDatabaseDialogSqlite dialog(this);
  QSqlDatabase db = pvMappingModel->sourceDatabase();

  if(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite){
    dialog.setDatabase(db);
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  setSourceDatabasePv(dialog.database().database());
}

void mdtSqlDatabaseCopierDialog::selectDestinationDatabase()
{
  Q_ASSERT(pvState != ProcessingCopy);

  mdtSqlDatabaseDialogSqlite dialog(this);
  QSqlDatabase db = pvMappingModel->destinationDatabase();

  if(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite){
    dialog.setDatabase(db);
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  setDestinationDatabase(dialog.database().database());
}

void mdtSqlDatabaseCopierDialog::resetMapping()
{
  Q_ASSERT(pvState != ProcessingCopy);

  if(!pvMappingModel->resetTableMapping()){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::mapByName()
{
  Q_ASSERT(pvState != ProcessingCopy);

  if(!pvMappingModel->generateTableMappingByName()){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::editTableMapping(const QModelIndex& index)
{
  if(pvState == ProcessingCopy){
    return;
  }
  if(!index.isValid()){
    return;
  }
  int row = index.row();
  auto tm = pvMappingModel->tableMapping(row);
  mdtSqlDatabaseCopierTableMappingDialog dialog(this);
  dialog.setDestinationTables(pvMappingModel->destinationDatabase(), pvMappingModel->getAvailableDestinationTableNameList());
  dialog.setMapping(tm);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvMappingModel->setTableMapping(row, dialog.mapping());
}

void mdtSqlDatabaseCopierDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void mdtSqlDatabaseCopierDialog::copyData()
{
  Q_ASSERT(pvState != ProcessingCopy);

  setStateProcessingCopy();
  pvMappingModel->clearCopyStatusAndProgress();
  pvThread->copyData(pvMappingModel->mapping());
}

// void mdtSqlDatabaseCopierDialog::abortCopy()
// {
// 
// }

// void mdtSqlDatabaseCopierDialog::updateGlobalProgress(int progress)
// {
//   pbGlobalProgress->setValue(progress);
// }

void mdtSqlDatabaseCopierDialog::onThreadGlobalErrorOccured(mdtError error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

void mdtSqlDatabaseCopierDialog::onThreadFinished()
{
  setStateDatabasesSetOrNotSet();
}

void mdtSqlDatabaseCopierDialog::setSourceDatabasePv(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);

  wSourceDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setSourceDatabase(db)){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
  setStateDatabasesSetOrNotSet();
}

void mdtSqlDatabaseCopierDialog::setStateDatabasesSetOrNotSet()
{
  if( hasDatabaseNeededInformations(pvMappingModel->sourceDatabase()) && hasDatabaseNeededInformations(pvMappingModel->destinationDatabase()) ){
    setStateDatabasesSet();
  }else{
    setStateDatabasesNotSet();
  }
}

void mdtSqlDatabaseCopierDialog::setStateDatabasesNotSet()
{
  pvState = DatabasesNotSet;
  tbSelectSourceDatabase->setEnabled(true);
  tbSelectDestinationDatabase->setEnabled(true);
  tbResetMapping->setEnabled(false);
  tbMapByName->setEnabled(false);
  tvMapping->setEnabled(false);
  pbCopy->setEnabled(false);
  pbAbort->setEnabled(false);
  setClosable(true);
}

void mdtSqlDatabaseCopierDialog::setStateDatabasesSet()
{
  pvState = DatabasesSet;
  tbSelectSourceDatabase->setEnabled(true);
  tbSelectDestinationDatabase->setEnabled(true);
  tbResetMapping->setEnabled(true);
  tbMapByName->setEnabled(true);
  tvMapping->setEnabled(true);
  pbCopy->setEnabled(true);
  pbAbort->setEnabled(false);
  setClosable(true);
}

bool mdtSqlDatabaseCopierDialog::hasDatabaseNeededInformations(const QSqlDatabase & db) const
{
  /// \todo Currently only SQLite is supported
  if(db.databaseName().isEmpty()){
    return false;
  }
  return true;
}

void mdtSqlDatabaseCopierDialog::setStateProcessingCopy()
{
  pvState = ProcessingCopy;
  tbSelectSourceDatabase->setEnabled(false);
  tbSelectDestinationDatabase->setEnabled(false);
  tbResetMapping->setEnabled(false);
  tbMapByName->setEnabled(false);
  tvMapping->setEnabled(true);
  pbCopy->setEnabled(false);
  pbAbort->setEnabled(true);
  setClosable(false);
}

void mdtSqlDatabaseCopierDialog::setClosable(bool closable)
{
  buttonBox->setEnabled(closable);
  pvClosable = closable;
}

void mdtSqlDatabaseCopierDialog::closeEvent(QCloseEvent* event)
{
  Q_ASSERT(event != nullptr);

  if(pvClosable){
    event->accept();
  }else{
    event->ignore();
  }
}

void mdtSqlDatabaseCopierDialog::reject()
{
  if(pvClosable){
    QDialog::reject();
  }
}

void mdtSqlDatabaseCopierDialog::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
