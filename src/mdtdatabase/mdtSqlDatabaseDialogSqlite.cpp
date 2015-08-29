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
#include <QComboBox>
#include <QToolButton>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
// #include <QInputDialog>
// #include <QMessageBox>

#include <QDebug>

mdtSqlDatabaseDialogSqlite::mdtSqlDatabaseDialogSqlite(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  connect(wConnectionName, &mdtSqlConnectionNameWidget::currentDatabaseChanged, this, &mdtSqlDatabaseDialogSqlite::updateDatabase);
  connect(wConnectionName, &mdtSqlConnectionNameWidget::aboutToRemoveDatabaseConnection, this, &mdtSqlDatabaseDialogSqlite::maybeClearDatabase);
  wConnectionName->setDriverType(mdtSqlDriverType::SQLite); // Will also update connections names
}

void mdtSqlDatabaseDialogSqlite::updateConnectionsList()
{
  wConnectionName->updateConnectionsList();
}

void mdtSqlDatabaseDialogSqlite::setDatabase(const mdtSqlDatabaseSqlite & db)
{
  wConnectionName->setCurrentConnection(db.database().connectionName());
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
    Q_ASSERT(!pvDatabase.database().isOpen());
    pvDatabase.clear();
  }
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
  if(pvDatabase.database().isOpen()){
    tbOpenDatabase->setEnabled(false);
    tbCloseDatabase->setEnabled(true);
    tbCreateDatabase->setEnabled(false);
    tbDeleteDatabase->setEnabled(false);
  }else{
    //
    tbOpenDatabase->setEnabled(true);
    tbCloseDatabase->setEnabled(false);
    tbCreateDatabase->setEnabled(true);
    tbDeleteDatabase->setEnabled(true);
  }
}
