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
#include "CsvFileImportDialog.h"
#include "CsvFileImportMappingModel.h"
#include "CsvFileImportTableMappingDialog.h"
#include "CsvFileImportThread.h"
#include "mdtErrorDialog.h"
#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtProgressBarItemDelegate.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QModelIndex>
#include <QCloseEvent>
#include <algorithm>

#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportDialog::CsvFileImportDialog(QWidget *parent)
 : QDialog(parent),
   pvMappingModel(new CsvFileImportMappingModel(this)),
   pvThread(new CsvFileImportThread(this)),
   pvDestinationDatabaseSelectable(true)
{
  setupUi(this);

  tvMapping->setModel(pvMappingModel);
  auto *progressBarDelegate = new mdtProgressBarItemDelegate(tvMapping);
  tvMapping->setItemDelegateForColumn(4, progressBarDelegate);
  Q_ASSERT(tvMapping->selectionModel() != nullptr);
  connect(tvMapping->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CsvFileImportDialog::onMappingSelectionChanged);
  connect(tbSelectDestinationDatabase, &QToolButton::clicked, this, &CsvFileImportDialog::selectDestinationDatabase);
  connect(tbAddCopyItem, &QToolButton::clicked, this, &CsvFileImportDialog::addCopyItem);
  connect(tbEditCopyItem, &QToolButton::clicked, this, &CsvFileImportDialog::editCopyItem);
  connect(tbRemoveCopyItem, &QToolButton::clicked, this, &CsvFileImportDialog::removeCopyItem);
  connect(pbImport, &QPushButton::clicked, this, &CsvFileImportDialog::importFiles);
  connect(pbAbort, &QPushButton::clicked, pvThread, &CsvFileImportThread::abort);
  connect(pvThread, &CsvFileImportThread::tableCopyProgressChanged, pvMappingModel, &CsvFileImportMappingModel::setTableCopyProgress);
  connect(pvThread, &CsvFileImportThread::tableCopyStatusChanged, pvMappingModel, &CsvFileImportMappingModel::setTableCopyStatus);
  connect(pvThread, &CsvFileImportThread::tableCopyErrorOccured, pvMappingModel, &CsvFileImportMappingModel::setTableCopyError);
  connect(pvThread, &CsvFileImportThread::globalProgressRangeChanged, pbGlobalProgress, &QProgressBar::setRange );
  connect(pvThread, &CsvFileImportThread::globalProgressValueChanged, pbGlobalProgress, &QProgressBar::setValue);
  connect(pvThread, &CsvFileImportThread::globalErrorOccured, this, &CsvFileImportDialog::onThreadGlobalErrorOccured);
  connect(pvThread, &CsvFileImportThread::finished, this, &CsvFileImportDialog::onThreadFinished);

  setStateDatabaseNotSet();
}

void CsvFileImportDialog::setDestinationDatabase(const QSqlDatabase & db)
{
//   Q_ASSERT(pvState != ProcessingCopy);

  wDestinationDatabaseInfo->displayInfo(db);
  pvMappingModel->setDestinationDatabase(db);
  resizeTableViewToContents();
  setStateDatabaseSetOrNotSet();
}

void CsvFileImportDialog::setDestinationDatabaseSelectable(bool selectable)
{
  pvDestinationDatabaseSelectable = selectable;
  updateSelectDestinationDatabaseState();
}

void CsvFileImportDialog::selectDestinationDatabase()
{
//   Q_ASSERT(pvState != ProcessingCopy);

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

void CsvFileImportDialog::addCopyItem()
{
  CsvFileImportTableMappingDialog dialog(this);
  auto db = pvMappingModel->destinationDatabase();
  Q_ASSERT(db.isOpen());

  dialog.setDestinationDatabase(db);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.mapping());
  pvMappingModel->appendTableMapping(dialog.mapping());
  setStateImportPossible();
}

void CsvFileImportDialog::editCopyItem()
{
  Q_ASSERT(pvSelectedRows.size() == 1);

  const int row = pvSelectedRows[0];
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvMappingModel->rowCount());

  auto tm = pvMappingModel->tableMapping(row);
  Q_ASSERT(tm);
  CsvFileImportTableMappingDialog dialog(this);
  dialog.setMapping(tm);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.mapping());
  pvMappingModel->tableMappingUpdated(row);
  if(pvMappingModel->rowCount() > 0){
    setStateImportPossible();
  }else{
    setStateDatabaseSetOrNotSet();
  }
}

void CsvFileImportDialog::removeCopyItem()
{
  Q_ASSERT(pvSelectedRows.size() == 1);

  pvMappingModel->removeTableMapping(pvSelectedRows[0]);
}

void CsvFileImportDialog::onMappingSelectionChanged(const QItemSelection & selected, const QItemSelection & /*deselected*/)
{
  Q_ASSERT(tvMapping->selectionModel() != nullptr);

  qDebug() << "Selection changed:" << selected.indexes();
  ///qDebug() << "-> selection: " << tvMapping->selectionModel()->selection().indexes();
  updateSelectedRowsList(tvMapping->selectionModel()->selection());
  updateEditCopyItemState();
  updateRemoveCopyItemState();
}

void CsvFileImportDialog::importFiles()
{
  Q_ASSERT(pvState == ImportPossible);

  setStateProcessingImport();
  pvMappingModel->clearCopyStatusAndProgress();
  pvThread->startCopy(pvMappingModel);
}

void CsvFileImportDialog::onThreadGlobalErrorOccured(mdtError error)
{
  displayError(error);
}

void CsvFileImportDialog::onThreadFinished()
{
  qDebug() << "Thread finished..";
  setStateImportPossible();
}


void CsvFileImportDialog::updateSelectedRowsList(const QItemSelection & s)
{
  const auto indexes = s.indexes();

  pvSelectedRows.clear();
  for(const auto & index : indexes){
    pvSelectedRows.push_back(index.row());
  }
  if(pvSelectedRows.empty()){
    return;
  }
  std::sort(pvSelectedRows.begin(), pvSelectedRows.end());
  auto last = std::unique(pvSelectedRows.begin(), pvSelectedRows.end());
  pvSelectedRows.erase(last, pvSelectedRows.end());
  
  qDebug() << "Selected rows:";
  for(int r : pvSelectedRows){
    qDebug() << " " << r;
  }
}

void CsvFileImportDialog::updateSelectDestinationDatabaseState()
{
  if(pvState == ProcessingImport){
    tbSelectDestinationDatabase->setEnabled(false);
    return;
  }
  tbSelectDestinationDatabase->setEnabled(pvDestinationDatabaseSelectable);
}

void CsvFileImportDialog::updateAddCopyItemState()
{
  tbAddCopyItem->setEnabled(pvState != ProcessingImport);
}

void CsvFileImportDialog::updateEditCopyItemState()
{
  if(pvState == ProcessingImport){
    tbEditCopyItem->setEnabled(false);
    return;
  }
  tbEditCopyItem->setEnabled(pvSelectedRows.size() == 1);
}

void CsvFileImportDialog::updateRemoveCopyItemState()
{
  if(pvState == ProcessingImport){
    tbRemoveCopyItem->setEnabled(false);
    return;
  }
  tbRemoveCopyItem->setEnabled(pvSelectedRows.size() == 1);
}

void CsvFileImportDialog::setStateDatabaseSetOrNotSet()
{
  if(hasDatabaseNeededInformations(pvMappingModel->destinationDatabase())){
    setStateDatabaseSet();
  }else{
    setStateDatabaseNotSet();
  }
}

void CsvFileImportDialog::setStateDatabaseNotSet()
{
  pvState = DatabaseNotSet;
  updateSelectDestinationDatabaseState();
  updateAddCopyItemState();
  updateEditCopyItemState();
  updateRemoveCopyItemState();
  pbAbort->setEnabled(false);
  pbImport->setEnabled(false);
  setClosable(true);
}

void CsvFileImportDialog::setStateDatabaseSet()
{
  pvState = DatabaseSet;
  updateSelectDestinationDatabaseState();
  updateAddCopyItemState();
  updateEditCopyItemState();
  updateRemoveCopyItemState();
  pbAbort->setEnabled(false);
  pbImport->setEnabled(false);
  setClosable(true);
}

void CsvFileImportDialog::setStateImportPossible()
{
  pvState = ImportPossible;
  updateSelectDestinationDatabaseState();
  updateAddCopyItemState();
  updateEditCopyItemState();
  updateRemoveCopyItemState();
  pbAbort->setEnabled(false);
  pbImport->setEnabled(true);
  setClosable(true);
}

void CsvFileImportDialog::setStateProcessingImport()
{
  pvState = ProcessingImport;
  updateSelectDestinationDatabaseState();
  updateAddCopyItemState();
  updateEditCopyItemState();
  updateRemoveCopyItemState();
  pbAbort->setEnabled(true);
  pbImport->setEnabled(false);
  setClosable(false);
}

void CsvFileImportDialog::setClosable(bool closable)
{
  buttonBox->setEnabled(closable);
  pvClosable = closable;
}

void CsvFileImportDialog::closeEvent(QCloseEvent *event)
{
  Q_ASSERT(event != nullptr);

  if(pvClosable){
    event->accept();
  }else{
    event->ignore();
  }
}

void CsvFileImportDialog::reject()
{
  if(pvClosable){
    QDialog::reject();
  }
}

bool CsvFileImportDialog::hasDatabaseNeededInformations(const QSqlDatabase & db) const
{
  /// \todo Currently only SQLite is supported
  if(db.databaseName().isEmpty()){
    return false;
  }
  return db.isOpen();
}

void CsvFileImportDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void CsvFileImportDialog::displayError(const mdtError & error)
{
  mdtErrorDialog dialog(error, this);
  dialog.exec();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
