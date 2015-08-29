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
// #include <QInputDialog>
// #include <QMessageBox>

#include <QDebug>

mdtSqlDatabaseDialogSqlite::mdtSqlDatabaseDialogSqlite(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  connect(wConnectionName, &mdtSqlConnectionNameWidget::currentDatabaseChanged, this, &mdtSqlDatabaseDialogSqlite::updateDatabase);
  wConnectionName->setDriverType(mdtSqlDriverType::SQLite); // Will also update connections names
//   wConnectionName->updateConnectionsList();
//   connect(cbConnectionNames, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
//           this, &mdtSqlDatabaseDialogSqlite::onCbConnectionNamesIndexChanged);
//   connect(tbAddConnection, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::addConnection);
//   connect(tbRemoveConnection, &QToolButton::clicked, this, &mdtSqlDatabaseDialogSqlite::removeSelectedConnection);
//   updateConnectionsList();
}

void mdtSqlDatabaseDialogSqlite::updateConnectionsList()
{
  wConnectionName->updateConnectionsList();
}

// void mdtSqlDatabaseDialogSqlite::updateConnectionsList()
// {
//   QStringList connectionNames = mdtSqlDatabaseSqlite::getConnectionNames();
//   connectionNames.sort();
//   cbConnectionNames->clear();
//   cbConnectionNames->addItems(connectionNames);
// }

void mdtSqlDatabaseDialogSqlite::setDatabase(const mdtSqlDatabaseSqlite & db)
{
  wConnectionName->setCurrentConnection(db.database().connectionName());
}

// void mdtSqlDatabaseDialogSqlite::addConnection()
// {
//   QInputDialog dialog(this);
//   QString connectionName;
//   bool connectionExists;
// 
//   /// \todo setup a validator
//   // Setup and show dialog
//   dialog.setInputMode(QInputDialog::TextInput);
//   dialog.setLabelText(tr("Connection name:"));
//   if(dialog.exec() != QDialog::Accepted){
//     return;
//   }
//   // Get user's input
//   connectionName = dialog.textValue().trimmed();
//   // Check if connection name allready exists
//   if(connectionName.isEmpty()){
//     connectionExists = QSqlDatabase::contains();
//   }else{
//     connectionExists = QSqlDatabase::contains(connectionName);
//   }
//   if(connectionExists){
//     QMessageBox msgBox(this);
//     msgBox.setText(tr("Connection name") + " '" + connectionName + "' " + tr("allready exists."));
//     msgBox.setIcon(QMessageBox::Warning);
//     msgBox.exec();
//     return;
//   }
//   /// \todo Should check returned database validity
//   QSqlDatabase::addDatabase("QSQLITE", connectionName);
//   updateConnectionsList();
//   // Select freshly added connection name
//   cbConnectionNames->setCurrentText(connectionName);
// }

// void mdtSqlDatabaseDialogSqlite::removeSelectedConnection()
// {
//   // We update pvDatabase on selection, so pvDatabase refers to selected connection
//   Q_ASSERT(!pvDatabase.database().isOpen());
// 
//   QString connectionName = pvDatabase.database().connectionName();
//   pvDatabase.clear();
//   QSqlDatabase::removeDatabase(connectionName);
//   updateConnectionsList();
// }

// void mdtSqlDatabaseDialogSqlite::onCbConnectionNamesIndexChanged(int index)
// {
//   qDebug() << "onCbConnectionNamesIndexChanged(" << index << ")";
//   
//   if(index < 0){
//     pvDatabase.clear();
//     
//     return;
//   }
//   pvDatabase.setDatabase(QSqlDatabase::database(cbConnectionNames->itemText(index), false));
//   
//   qDebug() << pvDatabase.database().databaseName();
// }

void mdtSqlDatabaseDialogSqlite::updateDatabase(const QSqlDatabase & db)
{
  qDebug() << "Update DB " << db.connectionName();
  if(db.isValid()){
    pvDatabase.setDatabase(db);
  }else{
    pvDatabase.clear();
  }
}
