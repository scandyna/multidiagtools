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
#include <QToolButton>
#include <QMessageBox>

mdtSqlDatabaseCopierDialog::mdtSqlDatabaseCopierDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierMappingModel(this))
{
  setupUi(this);
  
  tvMapping->setModel(pvMappingModel);
  
  connect(tbSelectSourceDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::selectSourceDatabase);
  connect(tbSelectDestinationDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::selectDestinationDatabase);
  connect(tbMapByName, &QToolButton::clicked, this, &mdtSqlDatabaseCopierDialog::mapByName);
}

void mdtSqlDatabaseCopierDialog::selectSourceDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  QSqlDatabase db = dialog.database().database();
  wSourceDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setSourceDatabase(db)){
    displayError(pvMappingModel->lastError());
  }
}

void mdtSqlDatabaseCopierDialog::selectDestinationDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  QSqlDatabase db = dialog.database().database();
  wDestinationDatabaseInfo->displayInfo(db);
  if(!pvMappingModel->setDestinationDatabase(db)){
    displayError(pvMappingModel->lastError());
  }
}

void mdtSqlDatabaseCopierDialog::resetMapping()
{

}

void mdtSqlDatabaseCopierDialog::mapByName()
{
  if(!pvMappingModel->generateTableMappingByName()){
    displayError(pvMappingModel->lastError());
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
