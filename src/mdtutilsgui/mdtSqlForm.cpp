/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlError>
#include <QList>
#include <QVariant>

mdtSqlForm::mdtSqlForm(QObject *parent)
 : QObject(parent)
{
  pvMainSqlWidget = new mdtSqlFormWidget;
}

mdtSqlForm::~mdtSqlForm()
{
}

mdtSqlFormWidget *mdtSqlForm::mainSqlWidget()
{
  Q_ASSERT(pvMainSqlWidget != 0);

  return pvMainSqlWidget;
}

bool mdtSqlForm::setTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db)
{
  QSqlTableModel *model;

  // Setup model
  model = pvMainSqlWidget->model();
  if(model == 0){
    model = new QSqlTableModel(this, db);
  }
  model->setTable(tableName);
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table '" + tableName + "'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return false;
  }
  // Setup main widget
  pvMainSqlWidget->setModel(model);
  if(!userFriendlyTableName.isEmpty()){
    pvMainSqlWidget->setUserFriendlyTableName(userFriendlyTableName);
  }
  pvMainSqlWidget->mapFormWidgets();

  return true;
}

bool mdtSqlForm::addChildTable(const QString &tableName, const QString &userFriendlyTableName, QSqlDatabase db)
{
  QSqlTableModel *model;
  mdtSqlTableWidget *widget;
  mdtSqlRelation *relation;

  // Check that main table was set
  if(pvMainSqlWidget->model() == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot add child table '" + tableName + "' because no main table was set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return false;
  }
  Q_ASSERT(pvMainSqlWidget->model() != 0);
  // Setup model
  model = new QSqlTableModel(this, db);
  model->setTable(tableName);
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table '" + tableName + "'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    delete model;
    return false;
  }
  // Setup relation
  relation = new mdtSqlRelation;
  relation->setParentModel(pvMainSqlWidget->model());
  relation->setChildModel(model);
  pvRelationsByChildTableName.insert(tableName, relation);
  // Setup child widget
  widget = new mdtSqlTableWidget;
  widget->setModel(model);
  if(!userFriendlyTableName.isEmpty()){
    widget->setUserFriendlyTableName(userFriendlyTableName);
  }
  // Add child widget to main widget
  pvMainSqlWidget->addChildWidget(widget, relation);
  addChildWidget(widget);

  return true;
}

bool mdtSqlForm::addRelation(const QString &parentFieldName, const QString &childTableName, const QString &childFieldName)
{
  mdtSqlRelation *relation;

  // Find relation that contains child table
  relation = pvRelationsByChildTableName.value(childTableName, 0);
  if(relation == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find relation that contains child table '" + childTableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return false;
  }
  // Add relation
  if(!relation->addRelation(parentFieldName, childFieldName)){
    return false;
  }
  // Force a update a first time
  relation->setParentCurrentIndex(pvMainSqlWidget->currentRow());

  return true;
}

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

mdtSqlFormWidget *mdtSqlForm::sqlFormWidget(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  w = sqlWidget(tableName);
  if(w == 0){
    return 0;
  }

  return dynamic_cast<mdtSqlFormWidget*>(w);
}

mdtSqlTableWidget *mdtSqlForm::sqlTableWidget(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  w = sqlWidget(tableName);
  if(w == 0){
    return 0;
  }

  return dynamic_cast<mdtSqlTableWidget*>(w);
}

QSqlTableModel *mdtSqlForm::model(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  w = sqlWidget(tableName);
  if(w == 0){
    return 0;
  }

  return w->model();
}

bool mdtSqlForm::select(const QString &tableName)
{
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find model that acts on table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return false;
  }

  return m->select();
}

QSqlError mdtSqlForm::lastError(const QString &tableName)
{
  QSqlTableModel *m;

  m = model(tableName);
  if(m == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find model that acts on table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return QSqlError();
  }

  return m->lastError();
}

int mdtSqlForm::currentRow(const QString &tableName)
{
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find SQL widget acting on table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return -1;
  }

  return w->currentRow();
}

QVariant mdtSqlForm::currentData(const QString &tableName, const QString &fieldName)
{
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find SQL widget acting on table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return QVariant();
  }

  return w->currentData(fieldName);
}

QVariant mdtSqlForm::data(const QString &tableName, int row, const QString &fieldName)
{
  mdtAbstractSqlWidget *w;

  // Find SQL widget
  w = sqlWidget(tableName);
  if(w == 0){
    mdtError e(MDT_DATABASE_ERROR, "Cannot find SQL widget acting on table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlForm");
    e.commit();
    return QVariant();
  }

  return w->data(row, fieldName);
}
