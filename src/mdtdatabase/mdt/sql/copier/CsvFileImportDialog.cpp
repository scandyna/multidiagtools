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
#include "mdtErrorDialog.h"
#include "mdtSqlDatabaseDialogSqlite.h"

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportDialog::CsvFileImportDialog(QWidget *parent)
 : QDialog(parent),
   pvMappingModel(new CsvFileImportMappingModel(this))
{
  setupUi(this);

  tvMapping->setModel(pvMappingModel);
  connect(tbSelectDestinationDatabase, &QToolButton::clicked, this, &CsvFileImportDialog::selectDestinationDatabase);
  connect(tbAddCopyItem, &QToolButton::clicked, this, &CsvFileImportDialog::addCopyItem);
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
}

void CsvFileImportDialog::setDestinationDatabase(const QSqlDatabase & db)
{
//   Q_ASSERT(pvState != ProcessingCopy);

  wDestinationDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setDestinationDatabase(db)){
    displayError(pvMappingModel->lastError());
    return;
  }
  resizeTableViewToContents();
//   setStateDatabasesSetOrNotSet();
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
