/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "DatabaseCopyDialog.h"
#include "mdtSqlDatabaseDialogSqlite.h"
// #include "DatabaseMappingModel.h"

#include "DatabaseMappingModel.h"
#include "DatabaseCopyThread.h"
#include "DatabaseCopierTableMappingDialog.h"

// #include "mdtSqlDatabaseCopierTableMappingDialog.h"

// #include "mdt/sql/copier/DatabaseCopierTableMappingDialog.h"
// #include "DatabaseCopyThread.h"
#include "mdtProgressBarItemDelegate.h"
#include "mdtAlgorithms.h"
#include "mdtErrorDialog.h"
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTableView>
#include <QCloseEvent>

namespace mdt{ namespace sql{ namespace copier{

DatabaseCopyDialog::DatabaseCopyDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new DatabaseMappingModel),
   pvThread(new DatabaseCopyThread),
   pvSourceDatabaseSelectable(true),
   pvDestinationDatabaseSelectable(true)
{
  setupUi(this);

  tvMapping->setModel(pvMappingModel);
  auto *progressBarDelegate = new mdtProgressBarItemDelegate(tvMapping);
  tvMapping->setItemDelegateForColumn(4, progressBarDelegate);
  connect(tvMapping, &QTableView::doubleClicked, this, &DatabaseCopyDialog::editTableMapping);
  connect(tbSelectSourceDatabase, &QToolButton::clicked, this, &DatabaseCopyDialog::selectSourceDatabase);
  connect(tbSelectDestinationDatabase, &QToolButton::clicked, this, &DatabaseCopyDialog::selectDestinationDatabase);
  connect(tbResetMapping, &QToolButton::clicked, this, &DatabaseCopyDialog::resetMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &DatabaseCopyDialog::mapByName);
  connect(pbCopy, &QPushButton::clicked, this, &DatabaseCopyDialog::copyData);
  connect(pbAbort, &QPushButton::clicked, pvThread, &DatabaseCopyThread::abort);
  connect(pvThread, &DatabaseCopyThread::tableCopyProgressChanged, pvMappingModel, &DatabaseMappingModel::setTableCopyProgress);
  connect(pvThread, &DatabaseCopyThread::tableCopyStatusChanged, pvMappingModel, &DatabaseMappingModel::setTableCopyStatus);
  connect(pvThread, &DatabaseCopyThread::tableCopyErrorOccured, pvMappingModel, &DatabaseMappingModel::setTableCopyError);
  connect(pvThread, &DatabaseCopyThread::globalProgressRangeChanged, pbGlobalProgress, &QProgressBar::setRange );
  connect(pvThread, &DatabaseCopyThread::globalProgressValueChanged, pbGlobalProgress, &QProgressBar::setValue);
  connect(pvThread, &DatabaseCopyThread::globalErrorOccured, this, &DatabaseCopyDialog::onThreadGlobalErrorOccured);
  connect(pvThread, &DatabaseCopyThread::finished, this, &DatabaseCopyDialog::onThreadFinished);
  setStateDatabasesNotSet();
}

DatabaseCopyDialog::~DatabaseCopyDialog()
{
  delete pvThread;
  delete pvMappingModel;
  if(!pvOwningSourceConnectionName.isEmpty()){
    QSqlDatabase::removeDatabase(pvOwningSourceConnectionName);
  }
//   if(!pvOwningDestinationConnectionName.isEmpty()){
//     QSqlDatabase::removeDatabase(pvOwningDestinationConnectionName);
//   }
}

void DatabaseCopyDialog::initSourceDatabase(mdtSqlDriverType::Type driverType)
{
  Q_ASSERT(pvState != ProcessingCopy);
  Q_ASSERT(pvOwningSourceConnectionName.isEmpty());

  QSqlDatabase db;

  pvOwningSourceConnectionName = mdtAlgorithms::generateString(6, QSqlDatabase::connectionNames());
  db = QSqlDatabase::addDatabase(mdtSqlDriverType::nameFromType(driverType), pvOwningSourceConnectionName);
  setSourceDatabasePv(db);
}

void DatabaseCopyDialog::setSourceDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);
  Q_ASSERT(pvOwningSourceConnectionName.isEmpty());

  setSourceDatabasePv(db);
}

void DatabaseCopyDialog::setSourceDatabaseSelectable(bool selectable)
{
  Q_ASSERT(pvState != ProcessingCopy);

  pvSourceDatabaseSelectable = selectable;
  tbSelectSourceDatabase->setEnabled(selectable);
}

void DatabaseCopyDialog::setDestinationDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);

  wDestinationDatabaseInfo->displayInfo(db);
  auto ret = pvMappingModel->setDestinationDatabase(db);
  if(!ret){
    displayError(ret.error());
    return;
  }
//   if(!pvMappingModel->setDestinationDatabase(db)){
//     displayError(pvMappingModel->lastError());
//     return;
//   }
  resizeTableViewToContents();
  setStateDatabasesSetOrNotSet();
}

void DatabaseCopyDialog::setDestinationDatabaseSelectable(bool selectable)
{
  Q_ASSERT(pvState != ProcessingCopy);

  pvDestinationDatabaseSelectable = selectable;
  tbSelectDestinationDatabase->setEnabled(selectable);
}

void DatabaseCopyDialog::selectSourceDatabase()
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

void DatabaseCopyDialog::selectDestinationDatabase()
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

void DatabaseCopyDialog::resetMapping()
{
  Q_ASSERT(pvState != ProcessingCopy);

  auto ret = pvMappingModel->resetMapping();
  if(!ret){
    displayError(ret.error());
    return;
  }
  pvMappingModel->setAllCompleteTableMappingSelected();
  resizeTableViewToContents();
}

void DatabaseCopyDialog::mapByName()
{
  Q_ASSERT(pvState != ProcessingCopy);

  auto ret = pvMappingModel->generateTableMappingByName();
  if(!ret){
    displayError(ret.error());
    return;
  }
  pvMappingModel->setAllCompleteTableMappingSelected();
  resizeTableViewToContents();
}

void DatabaseCopyDialog::editTableMapping(const QModelIndex& index)
{
  Q_ASSERT(pvMappingModel->sourceDatabase().isOpen());
  Q_ASSERT(pvMappingModel->destinationDatabase().isOpen());

  if(pvState == ProcessingCopy){
    return;
  }
  if(!index.isValid()){
    return;
  }
  mdt::sql::copier::DatabaseCopierTableMappingDialog dialog(this);
  int row = index.row();
  auto tm = pvMappingModel->tableMapping(row);
  Q_ASSERT(tm->destinationDatabase().isOpen());
  if(!tm->sourceDatabase().isOpen()){
    tm->setSourceDatabase(pvMappingModel->sourceDatabase());
  }
  dialog.setMapping(tm);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvMappingModel->tableMappingUpdated(row);
//   pvMappingModel->setTableMapping(row, dialog.mapping());
}

void DatabaseCopyDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void DatabaseCopyDialog::copyData()
{
  Q_ASSERT(pvState != ProcessingCopy);

  setStateProcessingCopy();
  pvMappingModel->clearCopyStatusAndProgress();
  pvThread->startCopy(pvMappingModel);
}

// void mdtSqlDatabaseCopierDialog::abortCopy()
// {
// 
// }

// void mdtSqlDatabaseCopierDialog::updateGlobalProgress(int progress)
// {
//   pbGlobalProgress->setValue(progress);
// }

void DatabaseCopyDialog::onThreadGlobalErrorOccured(mdtError error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  ///msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

void DatabaseCopyDialog::onThreadFinished()
{
  setStateDatabasesSetOrNotSet();
}

void DatabaseCopyDialog::setSourceDatabasePv(const QSqlDatabase & db)
{
  Q_ASSERT(pvState != ProcessingCopy);

  wSourceDatabaseInfo->displayInfo(db);
  auto ret = pvMappingModel->setSourceDatabase(db);
  if(!ret){
    displayError(ret.error());
    return;
  }
//   if(!pvMappingModel->setSourceDatabase(db)){
//     displayError(pvMappingModel->lastError());
//     return;
//   }
  resizeTableViewToContents();
  setStateDatabasesSetOrNotSet();
}

void DatabaseCopyDialog::setStateDatabasesSetOrNotSet()
{
  if( hasDatabaseNeededInformations(pvMappingModel->sourceDatabase()) && hasDatabaseNeededInformations(pvMappingModel->destinationDatabase()) ){
    setStateDatabasesSet();
  }else{
    setStateDatabasesNotSet();
  }
}

void DatabaseCopyDialog::setStateDatabasesNotSet()
{
  pvState = DatabasesNotSet;
  if(pvSourceDatabaseSelectable){
    tbSelectSourceDatabase->setEnabled(true);
  }
  if(pvDestinationDatabaseSelectable){
    tbSelectDestinationDatabase->setEnabled(true);
  }
  tbResetMapping->setEnabled(false);
  tbMapByName->setEnabled(false);
  tvMapping->setEnabled(false);
  pbCopy->setEnabled(false);
  pbAbort->setEnabled(false);
  setClosable(true);
}

void DatabaseCopyDialog::setStateDatabasesSet()
{
  pvState = DatabasesSet;
  if(pvSourceDatabaseSelectable){
    tbSelectSourceDatabase->setEnabled(true);
  }
  if(pvDestinationDatabaseSelectable){
    tbSelectDestinationDatabase->setEnabled(true);
  }
  tbResetMapping->setEnabled(true);
  tbMapByName->setEnabled(true);
  tvMapping->setEnabled(true);
  pbCopy->setEnabled(true);
  pbAbort->setEnabled(false);
  setClosable(true);
}

bool DatabaseCopyDialog::hasDatabaseNeededInformations(const QSqlDatabase & db) const
{
  /// \todo Currently only SQLite is supported
  if(db.databaseName().isEmpty()){
    return false;
  }
  return true;
}

void DatabaseCopyDialog::setStateProcessingCopy()
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

void DatabaseCopyDialog::setClosable(bool closable)
{
  buttonBox->setEnabled(closable);
  pvClosable = closable;
}

void DatabaseCopyDialog::closeEvent(QCloseEvent* event)
{
  Q_ASSERT(event != nullptr);

  if(pvClosable){
    event->accept();
  }else{
    event->ignore();
  }
}

void DatabaseCopyDialog::reject()
{
  if(pvClosable){
    QDialog::reject();
  }
}

void DatabaseCopyDialog::displayError(const mdtError& error)
{
  mdtErrorDialog dialog(this);  /// \todo Pass error..
  dialog.exec();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
