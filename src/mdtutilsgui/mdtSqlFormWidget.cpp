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
#include "mdtSqlFormWidget.h"
#include "mdtSqlDataWidgetMapper.h"
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QLayout>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QMessageBox>

#include <QDebug>

mdtSqlFormWidget::mdtSqlFormWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  pvWidgetMapper = new mdtSqlDataWidgetMapper(this);
  pvWidgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

mdtSqlFormWidget::~mdtSqlFormWidget()
{
  qDeleteAll(pvFieldHandlers);
}

void mdtSqlFormWidget::setModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvWidgetMapper->setModel(model);
  pvInsertionPending = false;
}

void mdtSqlFormWidget::mapFormWidgets()
{
  Q_ASSERT(layout() != 0);
  Q_ASSERT(pvWidgetMapper->model() != 0);

  int i, fieldIndex;
  QString fieldName;
  QWidget *w;
  mdtSqlFieldHandler *fieldHandler;
  QSqlRecord record;

  // Clear previous mapping
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper->clearMapping();
  // Search widgets in layout's childs
  for(i=0; i<layout()->count(); ++i){
    w = layout()->itemAt(i)->widget();
    if(w != 0){
      fieldName = w->objectName();
      // If name begins with fld_ , search matching field in model
      if(fieldName.left(4) == "fld_"){
        fieldName = fieldName.right(fieldName.length()-4);
        fieldIndex = pvWidgetMapper->model()->record().indexOf(fieldName);
        // If field was found, map it
        if(fieldIndex >= 0){
          pvWidgetMapper->addMapping(w, fieldIndex);
          fieldHandler = new mdtSqlFieldHandler;
          // If we want informations about fields, we must get record from database instance
          record = pvWidgetMapper->model()->database().record(pvWidgetMapper->model()->tableName());
          fieldHandler->setField(record.field(fieldIndex));
          fieldHandler->setDataWidget(w);
          pvFieldHandlers.append(fieldHandler);
        }else{
          w->setEnabled(false);
          mdtError e(MDT_DATABASE_ERROR, "Cannot find field for widget " + w->objectName(), mdtError::Warning);
          MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
          e.commit();
        }
      }
    }
  }
  pvWidgetMapper->toFirst();
}

int mdtSqlFormWidget::currentRow() const
{
  return pvWidgetMapper->currentIndex();
}

void mdtSqlFormWidget::submit()
{
  Q_ASSERT(pvWidgetMapper->model() != 0);

  int row;

  // Remember current row (will be lost during submit)
  row = pvWidgetMapper->currentIndex();
  qDebug() << "SAV: mapper current row (A): " << pvWidgetMapper->currentIndex();

  // Do some check before real submit
  if(!checkBeforeSubmit()){
    QMessageBox msgBox;
    msgBox.setText("Test");
    msgBox.exec();
    return;
  }
  // Call widget mapper submit() (will commit data from widgets to model)
  if(!pvWidgetMapper->submit()){
    qDebug() << "mdtSqlFormWidget::submit(): Error handling not implemented !!!!!!!!!!!!!!!!!!!!";
    return;
  }
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!pvWidgetMapper->model()->submitAll()){
    qDebug() << "mdtSqlFormWidget::submit(): Error handling not implemented !!!!!!!!!!!!!!!!!!!!";
    qDebug() << "-> Error: " << pvWidgetMapper->model()->lastError();
    return;
  }
  pvInsertionPending = false;
  // Go back to current row
  pvWidgetMapper->setCurrentIndex(row);
  qDebug() << "SAV: current row (B): " << pvWidgetMapper->currentIndex();
}

void mdtSqlFormWidget::revert()
{
  /*
   * We have 2 cases of revert:
   *  - A insertion is pending (currently not submitted to model)
   *  - We are editing a row
   *
   * If a insertion is pending, we must delete new inserted row.
   * For editing, we call widget mapper's revert() method,
   *  wich will repopulate widgets with model's data
   */
  if(pvInsertionPending){
    remove();
  }else{
    pvWidgetMapper->revert();
  }
}

void mdtSqlFormWidget::insert()
{
  Q_ASSERT(pvWidgetMapper->model() != 0);

  int row;

  // Insert new row at end
  row = pvWidgetMapper->model()->rowCount();
  pvWidgetMapper->model()->insertRow(row);
  pvWidgetMapper->setCurrentIndex(row);
  clearWidgets();
  pvInsertionPending = true;
}

void mdtSqlFormWidget::remove()
{
  Q_ASSERT(pvWidgetMapper->model() != 0);

  int row;

  // Remeber current row (will be lost during submit)
  row = pvWidgetMapper->currentIndex();
  if(!pvWidgetMapper->model()->removeRow(row)){
    qDebug() << "mdtSqlFormWidget::remove(): removeRow() failed Error handling not implemented !!!!!!!!!!!!!!!!!!!!";
    return;
  }
  // If a insertion is pending, it was never submitted, so we are finished here
  if(pvInsertionPending){
    pvInsertionPending = false;
    clearWidgets();
    pvWidgetMapper->setCurrentIndex(qMin(row, pvWidgetMapper->model()->rowCount()) - 1);
    return;
  }
  // Call widget mapper submit() (will commit data from widgets to model)
  if(!pvWidgetMapper->submit()){
    qDebug() << "mdtSqlFormWidget::remove(): submit() failed Error handling not implemented !!!!!!!!!!!!!!!!!!!!";
    return;
  }
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!pvWidgetMapper->model()->submitAll()){
    qDebug() << "mdtSqlFormWidget::remove(): Error handling not implemented !!!!!!!!!!!!!!!!!!!!";
    qDebug() << "-> Error: " << pvWidgetMapper->model()->lastError();
    return;
  }
  pvInsertionPending = false;
  // Go back to current row
  pvWidgetMapper->setCurrentIndex(qMin(row, pvWidgetMapper->model()->rowCount()) - 1);

}

void mdtSqlFormWidget::clearWidgets()
{
  int i;

  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    pvFieldHandlers.at(i)->clearWidgetData();
  }
}

bool mdtSqlFormWidget::checkBeforeSubmit()
{
  int i;
  bool allOk = true;

  // We call checkBeforeSubmit() for all items
  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    if(!pvFieldHandlers.at(i)->checkBeforeSubmit()){
      allOk = false;
    }
  }

  return allOk;
}
