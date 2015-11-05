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
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTableView>

mdtSqlDatabaseCopierDialog::mdtSqlDatabaseCopierDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierMappingModel(this)),
   pvThread(new mdtSqlDatabaseCopierThread(this))
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
}

void mdtSqlDatabaseCopierDialog::setSourceDatabase(const QSqlDatabase & db)
{
  wSourceDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setSourceDatabase(db)){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::setDestinationDatabase(const QSqlDatabase & db)
{
  wDestinationDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setDestinationDatabase(db)){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::selectSourceDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  setSourceDatabase(dialog.database().database());
}

void mdtSqlDatabaseCopierDialog::selectDestinationDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  setDestinationDatabase(dialog.database().database());
}

void mdtSqlDatabaseCopierDialog::resetMapping()
{
  if(!pvMappingModel->resetTableMapping()){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::mapByName()
{
  if(!pvMappingModel->generateTableMappingByName()){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierDialog::editTableMapping(const QModelIndex& index)
{
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

}

void mdtSqlDatabaseCopierDialog::onThreadFinished()
{

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
