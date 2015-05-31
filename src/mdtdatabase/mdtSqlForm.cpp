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
#include "mdtSqlForm.h"
#include "mdtSqlTableWidget.h"
#include "mdtUiMessageHandler.h"
#include <QSqlTableModel>
#include <QSqlField>
#include <QSqlError>
#include <QList>
#include <QVariant>
#include <QVBoxLayout>
#include <QMessageBox>

//#include <QDebug> 

using namespace std;

mdtSqlForm::mdtSqlForm(QWidget *parent, QSqlDatabase db)
 : QWidget(parent),
   pvController(new mdtSqlDataWidgetController)
{
  pvDatabase = db;
  pvMainTableWidget = 0;
  pvChildsTabWidget = 0;
  pvMainLayout = new QVBoxLayout;
  setLayout(pvMainLayout);
}

mdtSqlForm::~mdtSqlForm()
{
}

void mdtSqlForm::setMainTableWidget(QWidget* widget)
{
  Q_ASSERT(widget != 0);

  pvMainTableWidget = widget;
  pvMainLayout->addWidget(pvMainTableWidget);
  connect(pvController.get(), SIGNAL(mainWidgetEnableStateChanged(bool)), pvMainTableWidget, SLOT(setEnabled(bool)));
}

bool mdtSqlForm::setMainTable(const QString& tableName, const QString& userFriendlyTableName, const QString& firstWidgetInTabOrder)
{
  return setMainTable(tableName, userFriendlyTableName, pvDatabase, firstWidgetInTabOrder);
}

bool mdtSqlForm::setMainTable(const QString& tableName, const QString& userFriendlyTableName, QSqlDatabase db, const QString& firstWidgetInTabOrder)
{
  Q_ASSERT(pvMainTableWidget != 0);
  Q_ASSERT(pvMainTableWidget->layout() != 0);

  // Check that db is open
  if(!db.isOpen()){
    QString msg;
    msg = tr("Database is not open.") + "\n";
    msg += tr("Please open a database and try again.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  // Setup controller
  pvController->setTableName(tableName, db, userFriendlyTableName);
  // Setup main widget
  if(!pvController->mapFormWidgets(pvMainTableWidget, firstWidgetInTabOrder)){
    pvLastError = pvController->lastError();
    return false;
  }
  // Setup message handler
  std::shared_ptr<mdtUiMessageHandler> messageHandler(new mdtUiMessageHandler(this));
  pvController->setMessageHandler(messageHandler);

  return true;
}

bool mdtSqlForm::addChildTable(const mdtSqlRelationInfo& relationInfo, const QString& userFriendlyTableName)
{
  return addChildTable(relationInfo, userFriendlyTableName, pvDatabase);
}

bool mdtSqlForm::addChildTable(const mdtSqlRelationInfo& relationInfo, const QString& userFriendlyTableName, QSqlDatabase db)
{
  mdtSqlTableWidget *widget;
  std::shared_ptr<mdtSqlTableViewController> controller;

  // Check that db is open
  if(!db.isOpen()){
    QString msg;
    msg = tr("Database is not open.") + "\n";
    msg += tr("Please open a database and try again.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  // Add child controller
  if(!pvController->addChildController<mdtSqlTableViewController>(relationInfo, db, userFriendlyTableName)){
    pvLastError = pvController->lastError();
    return false;
  }
  controller = pvController->childController<mdtSqlTableViewController>(relationInfo.childTableName());
  Q_ASSERT(controller);
  // Setup child widget and add it to tab widget
  widget = new mdtSqlTableWidget;
  widget->setTableController(controller);
  connect(pvController.get(), SIGNAL(childWidgetEnableStateChanged(bool)), widget, SLOT(setEnabled(bool)));
  pvChildsTableWidgets.append(widget);
  addChildWidget(widget, controller->userFriendlyTableName());
  // Setup message handler
  std::shared_ptr<mdtUiMessageHandler> messageHandler(new mdtUiMessageHandler(this));
  controller->setMessageHandler(messageHandler);


  return true;
}

void mdtSqlForm::addChildWidget(QWidget *widget, const QString & label, const QIcon & icon)
{
  // Setup tab widget if needed
  if(pvChildsTabWidget == 0){
    pvChildsTabWidget = new QTabWidget;
    pvMainLayout->addWidget(pvChildsTabWidget, 5);
  }
  Q_ASSERT(pvChildsTabWidget != 0);
  // Add widget
  if(icon.isNull()){
    pvChildsTabWidget->addTab(widget, label);
  }else{
    pvChildsTabWidget->addTab(widget, icon, label);
  }
}

void mdtSqlForm::start()
{
  pvController->start();
}

mdtSqlTableWidget *mdtSqlForm::sqlTableWidget(const QString &tableName)
{
  int i;

  for(i = 0; i < pvChildsTableWidgets.size(); ++i){
    Q_ASSERT(pvChildsTableWidgets.at(i) != 0);
    if(pvChildsTableWidgets.at(i)->tableName() == tableName){
      return pvChildsTableWidgets.at(i);
    }
  }

  return 0;
}

bool mdtSqlForm::select()
{
  if(!pvController->select()){
    pvLastError = pvController->lastError();
    return false;
  }
  return true;
}

bool mdtSqlForm::select(const QString &tableName)
{
  if(pvController->tableName() == tableName){
    if(!pvController->select()){
      pvLastError = pvController->lastError();
      return false;
    }
    return true;
  }
  ///std::shared_ptr<mdtSqlTableViewController> controller;
  ///controller = pvController->childController<mdtSqlTableViewController>(tableName);
  auto controller = pvController->childController<mdtAbstractSqlTableController>(tableName);
  if(!controller){
    pvLastError.setError(tr("Cannot find controller that acts on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  if(!controller->select()){
    pvLastError = controller->lastError();
    return false;
  }
  return true;
}

mdtError mdtSqlForm::lastError() const
{
  return pvLastError;
}

void mdtSqlForm::displayLastError(const QString & windowTitle)
{
  QMessageBox msgBox(this);

  msgBox.setText(pvLastError.text());
  msgBox.setInformativeText(pvLastError.informativeText());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  if(!windowTitle.isEmpty()){
    msgBox.setWindowTitle(windowTitle);
  }
  msgBox.exec();
}

int mdtSqlForm::rowCount(const QString& tableName, bool fetchAll)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->rowCount(fetchAll);
}

bool mdtSqlForm::setCurrentRow(const QString& tableName, const QString& fieldName, const QVariant& matchData)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);
  if(!controller->setCurrentRow(fieldName, matchData)){
    pvLastError = controller->lastError();
    return false;
  }

  return true;
}

bool mdtSqlForm::setCurrentData(const QString &tableName, const QString &fieldName, const QVariant &data, bool submit)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);
  if(!controller->setCurrentData(fieldName, data, submit)){
    pvLastError = controller->lastError();
    return false;
  }

  return true;
}

QVariant mdtSqlForm::currentData(const QString &tableName, const QString &fieldName, bool & ok)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->currentData(fieldName, ok);
}

QVariant mdtSqlForm::data(const QString &tableName, int row, const QString &fieldName, bool & ok)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->data(row, fieldName, ok);
}

QList< QVariant > mdtSqlForm::dataList(const QString& tableName, const QString& fieldName, bool& ok, bool fetchAll)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->dataList(fieldName, ok, fetchAll);
}

bool mdtSqlForm::setupTables()
{
  return false;
}
