/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
///#include "mdtAbstractSqlWidget.h"
///#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
///#include "mdtSqlRelation.h"
#include "mdtUiMessageHandler.h"
#include <QSqlTableModel>
#include <QSqlField>
#include <QSqlError>
#include <QList>
#include <QVariant>
#include <QVBoxLayout>
#include <QMessageBox>

#include <QDebug> 

using namespace std;

mdtSqlForm::mdtSqlForm(QWidget *parent, QSqlDatabase db)
 : QWidget(parent),
   pvController(new mdtSqlDataWidgetController)
{
  pvDatabase = db;
  ///pvMainSqlWidget = new mdtSqlFormWidget;
  pvMainTableWidget = 0;
  pvChildsTabWidget = 0;
  pvMainLayout = new QVBoxLayout;
  ///pvMainLayout->addWidget(pvMainSqlWidget);
  setLayout(pvMainLayout);
}

mdtSqlForm::~mdtSqlForm()
{
  qDebug() << "mdtSqlForm::~mdtSqlForm()";
}

/**
mdtSqlFormWidget *mdtSqlForm::mainSqlWidget()
{
  Q_ASSERT(pvMainSqlWidget != 0);

  return pvMainSqlWidget;
}
*/

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
  ///QSqlTableModel *model;

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
  // Setup model
  /**
  model = pvMainSqlWidget->model();
  if(model == 0){
    model = new QSqlTableModel(this, db);
  }
  model->setTable(tableName);
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    pvLastError.setError(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  */
  // Setup main widget
  if(!pvController->mapFormWidgets(pvMainTableWidget, firstWidgetInTabOrder)){
    pvLastError = pvController->lastError();
    return false;
  }
  /**
  pvMainSqlWidget->setModel(model);
  if(!userFriendlyTableName.isEmpty()){
    pvMainSqlWidget->setUserFriendlyTableName(userFriendlyTableName);
  }
  pvMainSqlWidget->mapFormWidgets();
  */
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

  return true;
}

/**
bool mdtSqlForm::addChildTable(const QString &tableName, const QString &userFriendlyTableName)
{
  return addChildTable(tableName, userFriendlyTableName, pvDatabase);
}
*/

// bool mdtSqlForm::addChildTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db)
// {
//   QSqlTableModel *model;
//   mdtSqlTableWidget *widget;
//   ///mdtSqlRelation *relation;
// 
//   // Check that db is open
//   if(!db.isOpen()){
//     QString msg;
//     msg = tr("Database is not open.") + "\n";
//     msg += tr("Please open a database and try again.");
//     pvLastError.setError(msg, mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
//     pvLastError.commit();
//     return false;
//   }
//   // Check that main table was set
//   if(pvMainSqlWidget->model() == 0){
//     pvLastError.setError(tr("Cannot add child table '") + tableName + tr("' because no main table was set"), mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
//     pvLastError.commit();
//     return false;
//   }
//   Q_ASSERT(pvMainSqlWidget->model() != 0);
//   // Setup model
//   model = new QSqlTableModel(this, db);
//   model->setTable(tableName);
//   if(!model->select()){
//     QSqlError sqlError = model->lastError();
//     pvLastError.setError(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
//     pvLastError.setSystemError(sqlError.number(), sqlError.text());
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
//     pvLastError.commit();
//     delete model;
//     return false;
//   }
//   // Setup relation
//   /**
//   relation = new mdtSqlRelation;
//   relation->setParentModel(pvMainSqlWidget->model());
//   relation->setChildModel(model);
//   pvRelationsByChildTableName.insert(tableName, relation);
//   */
//   // Setup child widget
//   /**
//   widget = new mdtSqlTableWidget;
//   widget->setModel(model);
//   if(!userFriendlyTableName.isEmpty()){
//     widget->setUserFriendlyTableName(userFriendlyTableName);
//   }
//   // Add child widget to main widget and tab
//   pvMainSqlWidget->addChildWidget(widget, relation);
//   addChildWidget(widget, widget->userFriendlyTableName());
//   */
// 
//   return true;
// }

void mdtSqlForm::addChildWidget(QWidget *widget, const QString & label, const QIcon & icon)
{
  // Setup tab widget if needed
  if(pvChildsTabWidget == 0){
    pvChildsTabWidget = new QTabWidget;
    pvMainLayout->addWidget(pvChildsTabWidget);
  }
  Q_ASSERT(pvChildsTabWidget != 0);
  // Add widget
  if(icon.isNull()){
    pvChildsTabWidget->addTab(widget, label);
  }else{
    pvChildsTabWidget->addTab(widget, icon, label);
  }
}

/**
bool mdtSqlForm::addRelation(const QString &parentFieldName, const QString &childTableName, const QString &childFieldName, const QString &operatorWithPreviousItem)
{
  mdtSqlRelation *relation;

  // Find relation that contains child table
  relation = pvRelationsByChildTableName.value(childTableName, 0);
  if(relation == 0){
    pvLastError.setError(tr("Cannot find relation that contains child table '") + childTableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  // Add relation
  if(!relation->addRelation(parentFieldName, childFieldName, true, operatorWithPreviousItem)){
    return false;
  }
  // Force a update a first time
  relation->setParentCurrentIndex(pvMainSqlWidget->currentRow());

  return true;
}
*/

void mdtSqlForm::start()
{
  pvController->start();
  /**
  QList<mdtAbstractSqlWidget*> sqlWidgets;
  int i;

  sqlWidgets = pvMainSqlWidget->sqlWidgets();
  for(i = 0; i < sqlWidgets.size(); ++i){
    Q_ASSERT(sqlWidgets.at(i) != 0);
    Q_ASSERT(sqlWidgets.at(i)->model() != 0);
    sqlWidgets.at(i)->start();
  }
  */
}

/**
mdtAbstractSqlWidget *mdtSqlForm::sqlWidget(const QString &tableName)
{
  QList<mdtAbstractSqlWidget*> sqlWidgets;
  int i;

  sqlWidgets = pvMainSqlWidget->sqlWidgets();
  for(i = 0; i < sqlWidgets.size(); ++i){
    Q_ASSERT(sqlWidgets.at(i) != 0);
    Q_ASSERT(sqlWidgets.at(i)->model() != 0);
    if(sqlWidgets.at(i)->model()->tableName() == tableName){
      return sqlWidgets.at(i);
    }
  }

  return 0;
}
*/

/**
mdtSqlTableWidget *mdtSqlForm::sqlTableWidget(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  w = sqlWidget(tableName);
  if(w == 0){
    return 0;
  }

  return dynamic_cast<mdtSqlTableWidget*>(w);
}
*/

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

/**
QSqlDatabase mdtSqlForm::database()
{
  return pvDatabase;
}
*/

/**
QSqlDatabase mdtSqlForm::database(const QString &tableName)
{
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    return QSqlDatabase();
  }

  return m->database();
}
*/

/**
QSqlTableModel *mdtSqlForm::model(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  w = sqlWidget(tableName);
  if(w == 0){
    return 0;
  }

  return w->model();
}
*/

bool mdtSqlForm::select()
{
  if(!pvController->select()){
    pvLastError = pvController->lastError();
    return false;
  }
  return true;
  /**
  QSqlTableModel *m;

  m = pvMainSqlWidget->model();
  if(m == 0){
    pvLastError.setError(tr("Main table was not set."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  if(!m->select()){
    QSqlError sqlError = m->lastError();
    QString tableName = pvMainSqlWidget->userFriendlyTableName();
    pvLastError.setError(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }

  return true;
  */
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
  std::shared_ptr<mdtSqlTableViewController> controller;
  controller = pvController->childController<mdtSqlTableViewController>(tableName);
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
  /**
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    pvLastError.setError(tr("Cannot find model that acts on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  if(!m->select()){
    QSqlError sqlError = m->lastError();
    mdtAbstractSqlWidget *w = sqlWidget(tableName);
    Q_ASSERT(w != 0);
    QString tableName = w->userFriendlyTableName();
    pvLastError.setError(tr("Unable to select data in table '") + tableName + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }

  return true;
  */
}

/**
bool mdtSqlForm::setMainTableFilter(const QString & fieldName, const QVariant & matchData)
{
  QSqlTableModel *m;
  QVariant::Type dataType;
  QString delimiter;
  QString filter;
  int fieldIndex;

  // Get model
  m = pvMainSqlWidget->model();
  if(m == 0){
    pvLastError.setError(tr("Trying to apply a filter to main table, but table was not set."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  // Check that fieldName exists
  fieldIndex = m->record().indexOf(fieldName);
  if(fieldIndex < 0){
    pvLastError.setError(tr("Table ") + m->tableName() + tr(" has no field named ") + fieldName + tr("."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  dataType = m->record().field(fieldIndex).type();
  // Choose delimiter regarding data type
  switch(dataType){
    case QVariant::String:
      delimiter = "'";
    default:
      ;
  }
  // Setup and apply filter
  filter = fieldName + " = " + delimiter + matchData.toString() + delimiter;
  qDebug() << "Filter: " << filter;
  m->setFilter(filter);

  return select();
}
*/

/**
QSqlError mdtSqlForm::lastSqlError(const QString &tableName)
{
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    ///mdtError e(MDT_DATABASE_ERROR, "Cannot find model that acts on table '" + tableName + "'", mdtError::Error);
    pvLastError.setError(tr("Cannot find model that acts on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return QSqlError();
  }

  return m->lastError();
}
*/

mdtError mdtSqlForm::lastError() const
{
  return pvLastError;
}

void mdtSqlForm::displayLastError()
{
  QMessageBox msgBox;

  msgBox.setText(pvLastError.text());
  msgBox.setInformativeText(pvLastError.informativeText());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

/**
int mdtSqlForm::rowCount(const QString &tableName)
{
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    pvLastError.setError(tr("Cannot find model that acts on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return -1;
  }

  return m->rowCount();
}
*/

/**
int mdtSqlForm::currentRow(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    pvLastError.setError(tr("Cannot find SQL widget acting on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return -1;
  }

  return w->currentRow();
}
*/

/**
bool mdtSqlForm::setCurrentRecord(const QString &fieldName, const QVariant &value)
{
  if(pvMainSqlWidget == 0){
    pvLastError.setError(tr("Cannot set current record because main table was not set."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }
  if(!pvMainSqlWidget->setCurrentRecord(fieldName, value)){
    pvLastError = pvMainSqlWidget->lastError();
    return false;
  }
  return true;
}
*/

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
  /**
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    pvLastError.setError(tr("Cannot find SQL widget acting on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return false;
  }

  return w->setCurrentData(fieldName, data, submit);
  */
}

QVariant mdtSqlForm::currentData(const QString &tableName, const QString &fieldName, bool & ok)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->currentData(fieldName, ok);
  /**
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    pvLastError.setError(tr("Cannot find SQL widget acting on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return QVariant();
  }

  return w->currentData(fieldName);
  */
}

QVariant mdtSqlForm::data(const QString &tableName, int row, const QString &fieldName, bool & ok)
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;

  controller = tableController<mdtAbstractSqlTableController>(tableName);
  Q_ASSERT(controller);

  return controller->data(row, fieldName, ok);

  /**
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    pvLastError.setError(tr("Cannot find SQL widget acting on table '") + tableName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForm");
    pvLastError.commit();
    return QVariant();
  }

  return w->data(row, fieldName);
  */
}

/**
void mdtSqlForm::insert()
{
  Q_ASSERT(pvMainSqlWidget != 0);

  pvMainSqlWidget->insert();
}
*/

bool mdtSqlForm::setupTables()
{
  return false;
}

/**
bool mdtSqlForm::allDataAreSaved()
{
  if(pvMainSqlWidget == 0){
    return true;
  }
  return pvMainSqlWidget->allDataAreSaved();
}
*/
