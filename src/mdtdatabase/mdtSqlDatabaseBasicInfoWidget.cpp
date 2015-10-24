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
#include "mdtSqlDatabaseBasicInfoWidget.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QDir>

mdtSqlDatabaseBasicInfoWidget::mdtSqlDatabaseBasicInfoWidget(QWidget* parent)
 : QWidget(parent),
   pvLayout(new QFormLayout),
   lbDatabaseNamelabel(nullptr),
   lbDatabaseName(nullptr),
   lbDirectoryLabel(nullptr),
   lbDirectory(nullptr),
   lbHostLabel(nullptr),
   lbHost(nullptr),
   lbPortLabel(nullptr),
   lbPort(nullptr),
   lbUserLabel(nullptr),
   lbUser(nullptr)
{
  setLayout(pvLayout);
}

void mdtSqlDatabaseBasicInfoWidget::setDriverType(mdtSqlDriverType::Type driverType)
{
  switch(driverType){
    case mdtSqlDriverType::Unknown:
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
      updateDatabaseNameWidgets(true);
      updateDirectoryWidgets(false);
      updateHostAndPortWidgets(true);
      updateUserWidgets(true);
      break;
    case mdtSqlDriverType::SQLite:
      updateDatabaseNameWidgets(true);
      updateDirectoryWidgets(true);
      updateHostAndPortWidgets(false);
      updateUserWidgets(false);
      break;
  }
  clearDatabaseInfo();
}

void mdtSqlDatabaseBasicInfoWidget::displayInfo(const QSqlDatabase & db)
{
  updateWidgets(db);
}

void mdtSqlDatabaseBasicInfoWidget::updateWidgets(const QSqlDatabase & db)
{
  mdtSqlDriverType::Type driverType = mdtSqlDriverType::typeFromName(db.driverName());

  setDriverType(driverType);
  switch(driverType){
    case mdtSqlDriverType::Unknown:
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
      clearDatabaseInfo();
      break;
    case mdtSqlDriverType::SQLite:
      displayInfoSqlite(db);
      break;
  }
}

void mdtSqlDatabaseBasicInfoWidget::updateDatabaseNameWidgets(bool on)
{
  if(on){
    buildDatabaseNameWidgets();
  }else{
    removeDatabaseNameWidgets();
  }
}

void mdtSqlDatabaseBasicInfoWidget::buildDatabaseNameWidgets()
{
  if(lbDatabaseNamelabel == nullptr){
    lbDatabaseNamelabel = new QLabel(tr("Database:"));
  }
  if(lbDatabaseName == nullptr){
    lbDatabaseName = new QLabel;
  }
  pvLayout->addRow(lbDatabaseNamelabel, lbDatabaseName);
}

void mdtSqlDatabaseBasicInfoWidget::removeDatabaseNameWidgets()
{
  if(lbDatabaseNamelabel != nullptr){
    delete lbDatabaseNamelabel;
    lbDatabaseNamelabel = nullptr;
  }
  if(lbDatabaseName != nullptr){
    delete lbDatabaseName;
    lbDatabaseName = nullptr;
  }
}

void mdtSqlDatabaseBasicInfoWidget::updateDirectoryWidgets(bool on)
{
  if(on){
    buildDirectoryWidgets();
  }else{
    removeDirectoryWidgets();
  }
}

void mdtSqlDatabaseBasicInfoWidget::buildDirectoryWidgets()
{
  if(lbDirectoryLabel == nullptr){
    lbDirectoryLabel = new QLabel(tr("Directory:"));
  }
  if(lbDirectory == nullptr){
    lbDirectory = new QLabel;
  }
  pvLayout->addRow(lbDirectoryLabel, lbDirectory);
}

void mdtSqlDatabaseBasicInfoWidget::removeDirectoryWidgets()
{
  if(lbDirectoryLabel != nullptr){
    delete lbDirectoryLabel;
    lbDirectoryLabel = nullptr;
  }
  if(lbDirectory != nullptr){
    delete lbDirectory;
    lbDirectory = nullptr;
  }
}

void mdtSqlDatabaseBasicInfoWidget::updateHostAndPortWidgets(bool on)
{
  if(on){
    buildHostAndPortWidgets();
  }else{
    removeHostAndPortWidgets();
  }
}

void mdtSqlDatabaseBasicInfoWidget::buildHostAndPortWidgets()
{
  if(lbHostLabel == nullptr){
    lbHostLabel = new QLabel(tr("Host:"));
  }
  if(lbHost == nullptr){
    lbHost = new QLabel;
  }
  if(lbPortLabel == nullptr){
    lbPortLabel = new QLabel(tr("Port:"));
  }
  if(lbPort == nullptr){
    lbPort = new QLabel;
  }
  QHBoxLayout *l = new QHBoxLayout;
  l->addWidget(lbHost);
  l->addWidget(lbPortLabel);
  l->addWidget(lbPort);
  pvLayout->addRow(lbHostLabel, l);
}

void mdtSqlDatabaseBasicInfoWidget::removeHostAndPortWidgets()
{
  if(lbHostLabel != nullptr){
    delete lbHostLabel;
    lbHostLabel = nullptr;
  }
  if(lbHost != nullptr){
    delete lbHost;
    lbHost = nullptr;
  }
  if(lbPortLabel != nullptr){
    delete lbPortLabel;
    lbPortLabel = nullptr;
  }
  if(lbPort != nullptr){
    delete lbPort;
    lbPort = nullptr;
  }
}

void mdtSqlDatabaseBasicInfoWidget::updateUserWidgets(bool on)
{
  if(on){
    buildUserWidgets();
  }else{
    removeUserWidgets();
  }
}

void mdtSqlDatabaseBasicInfoWidget::buildUserWidgets()
{
  if(lbUserLabel == nullptr){
    lbUserLabel = new QLabel(tr("User:"));
  }
  if(lbUser == nullptr){
    lbUser = new QLabel;
  }
  pvLayout->addRow(lbUserLabel, lbUser);
}

void mdtSqlDatabaseBasicInfoWidget::removeUserWidgets()
{
  if(lbUserLabel != nullptr){
    delete lbUserLabel;
    lbUserLabel = nullptr;
  }
  if(lbUser != nullptr){
    delete lbUser;
    lbUser = nullptr;
  }
}

void mdtSqlDatabaseBasicInfoWidget::clearDatabaseInfo()
{
  if(lbDatabaseName != nullptr){
    lbDatabaseName->clear();
  }
  if(lbDirectory != nullptr){
    lbDirectory->clear();
  }
  if(lbHost != nullptr){
    lbHost->clear();
  }
  if(lbPort != nullptr){
    lbPort->clear();
  }
  if(lbUser != nullptr){
    lbUser->clear();
  }
}

void mdtSqlDatabaseBasicInfoWidget::displayInfoSqlite(const QSqlDatabase & db)
{
  Q_ASSERT(lbDatabaseName != nullptr);
  Q_ASSERT(lbDirectory != nullptr);

  if(db.databaseName().isEmpty()){
    lbDatabaseName->clear();
    lbDirectory->clear();
    return;
  }
  QFileInfo fi(db.databaseName());
  lbDatabaseName->setText(fi.fileName());
  lbDirectory->setText(fi.absoluteDir().path());
}
