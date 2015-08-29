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
#include "mdtSqlConnectionNameWidget.h"
#include <QInputDialog>
#include <QMessageBox>

#include <QDebug>

mdtSqlConnectionNameWidget::mdtSqlConnectionNameWidget(QWidget* parent)
 : QWidget(parent),
   pvDriverType(mdtSqlDriverType::Unknown)
{
  setupUi(this);
  connect(cbConnectionNames, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtSqlConnectionNameWidget::onCbConnectionNamesIndexChanged);
  connect(tbAddConnection, &QToolButton::clicked, this, &mdtSqlConnectionNameWidget::addConnection);
  connect(tbRemoveConnection, &QToolButton::clicked, this, &mdtSqlConnectionNameWidget::removeSelectedConnection);
  updateConnectionsList();
}

void mdtSqlConnectionNameWidget::setDriverType(mdtSqlDriverType::Type type)
{
  pvDriverType = type;
  updateConnectionsList();
}

void mdtSqlConnectionNameWidget::setCurrentConnection(const QString& name)
{
  cbConnectionNames->setCurrentText(name);
}

void mdtSqlConnectionNameWidget::updateConnectionsList()
{
  cbConnectionNames->clear();
  cbConnectionNames->addItems(getConnectionNames(pvDriverType, true));
}

QStringList mdtSqlConnectionNameWidget::getConnectionNames(mdtSqlDriverType::Type type, bool sort)
{
  QStringList connectionNames;

  for(const auto & cnnName : QSqlDatabase::connectionNames()){
    QSqlDatabase db = QSqlDatabase::database(cnnName, false);
    if(mdtSqlDriverType::typeFromName(db.driverName()) == type){
      connectionNames.append(cnnName);
    }
  }
  if(sort){
    connectionNames.sort();
  }

  return connectionNames;
}

void mdtSqlConnectionNameWidget::addConnection()
{
  QInputDialog dialog(this);
  QString connectionName;
  bool connectionExists;

  /// \todo setup a validator
  // Setup and show dialog
  dialog.setInputMode(QInputDialog::TextInput);
  dialog.setLabelText(tr("Connection name:"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Get user's input
  connectionName = dialog.textValue().trimmed();
  // Check if connection name allready exists
  if(connectionName.isEmpty()){
    connectionExists = QSqlDatabase::contains();
  }else{
    connectionExists = QSqlDatabase::contains(connectionName);
  }
  if(connectionExists){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Connection name") + " '" + connectionName + "' " + tr("allready exists."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  /// \todo Should check returned database validity
  QSqlDatabase::addDatabase(mdtSqlDriverType::nameFromType(pvDriverType), connectionName);
  updateConnectionsList();
  // Select freshly added connection name
  cbConnectionNames->setCurrentText(connectionName);
}

void mdtSqlConnectionNameWidget::removeSelectedConnection()
{
  QString connectionName = cbConnectionNames->currentText();
  Q_ASSERT(!QSqlDatabase::database(connectionName, false).isOpen());

  emit aboutToRemoveDatabaseConnection(connectionName);
  QSqlDatabase::removeDatabase(connectionName);
  updateConnectionsList();
}

void mdtSqlConnectionNameWidget::onCbConnectionNamesIndexChanged(int index)
{
  qDebug() << "onCbConnectionNamesIndexChanged(" << index << ")";
  QSqlDatabase db = QSqlDatabase::database(cbConnectionNames->currentText(), false);

  if(db.isOpen()){
    tbRemoveConnection->setEnabled(false);
  }else{
    tbRemoveConnection->setEnabled(true);
  }
  emit currentDatabaseChanged(db);
  qDebug() << "currentDatabaseChanged() emited";
}
