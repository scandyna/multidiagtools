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
///#include "mdtSqlDataWidgetMapper.h"
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include <QDataWidgetMapper>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QLayout>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>

//#include <QDebug>

mdtSqlFormWidget::mdtSqlFormWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  ///pvWidgetMapper = new mdtSqlDataWidgetMapper(this);
  pvWidgetMapper = new QDataWidgetMapper(this);
  pvWidgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  pvFirstDataWidget = 0;
}

mdtSqlFormWidget::~mdtSqlFormWidget()
{
  qDeleteAll(pvFieldHandlers);
}

void mdtSqlFormWidget::mapFormWidgets(const QString &firstWidgetInTabOrder)
{
  Q_ASSERT(layout() != 0);
  Q_ASSERT(model() != 0);

  int i, fieldIndex;
  QString fieldName;
  QWidget *w;
  mdtSqlFieldHandler *fieldHandler;
  QSqlRecord record;

  // Clear previous mapping
  disconnect(pvWidgetMapper, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper->clearMapping();
  pvFirstDataWidget = 0;
  // Search widgets in layout's childs
  for(i=0; i<layout()->count(); ++i){
    w = layout()->itemAt(i)->widget();
    if(w != 0){
      fieldName = w->objectName();
      // If name begins with fld_ , search matching field in model
      if(fieldName.left(4) == "fld_"){
        fieldName = fieldName.right(fieldName.length()-4);
        fieldIndex = model()->record().indexOf(fieldName);
        // If field was found, map it
        if(fieldIndex >= 0){
          pvWidgetMapper->addMapping(w, fieldIndex);
          fieldHandler = new mdtSqlFieldHandler;
          // If we want informations about fields, we must get record from database instance
          record = model()->database().record(model()->tableName());
          fieldHandler->setField(record.field(fieldIndex));
          fieldHandler->setDataWidget(w);
          connect(fieldHandler, SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
          /*
           * When we go out from Editing state, we must update flags
           */
          /// \todo Check redoundances with onCurrentIndexChanged()
          connect(this, SIGNAL(stateEditingExited()), fieldHandler, SLOT(updateFlags()));
          connect(this, SIGNAL(stateEditingNewRowExited()), fieldHandler, SLOT(updateFlags()));
          pvFieldHandlers.append(fieldHandler);
          // If this widget is the first in focus chain, ref it
          if(w->objectName() == firstWidgetInTabOrder){
            pvFirstDataWidget = w;
          }
        }else{
          w->setEnabled(false);
          mdtError e(MDT_DATABASE_ERROR, "Cannot find field for widget " + w->objectName(), mdtError::Warning);
          MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
          e.commit();
        }
      }
    }
  }
  connect(pvWidgetMapper, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  pvWidgetMapper->toFirst();
}

int mdtSqlFormWidget::currentRow() const
{
  return pvWidgetMapper->currentIndex();
}

void mdtSqlFormWidget::toFirst()
{
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  pvWidgetMapper->toFirst();
}

void mdtSqlFormWidget::toLast()
{
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  pvWidgetMapper->toLast();
}

void mdtSqlFormWidget::toPrevious()
{
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  pvWidgetMapper->toPrevious();
}

void mdtSqlFormWidget::toNext()
{
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  pvWidgetMapper->toNext();
}

void mdtSqlFormWidget::doSetModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvWidgetMapper->setModel(model);
}

void mdtSqlFormWidget::onCurrentIndexChanged(int row)
{
  Q_ASSERT(model() != 0);

  bool widgetsEnabled;
  mdtSqlFieldHandler *fieldHandler;
  int i;

  // Check if we have to enable/disable widgets regarding current row
  if(row < 0){
    widgetsEnabled = false;
  }else{
    widgetsEnabled = true;
  }
  // Update widgets
  for(i=0; i<pvFieldHandlers.size(); ++i){
    fieldHandler = pvFieldHandlers.at(i);
    Q_ASSERT(fieldHandler != 0);
    if(fieldHandler->dataWidget() != 0){
      fieldHandler->dataWidget()->setEnabled(widgetsEnabled);
      if(!widgetsEnabled){
        fieldHandler->clearWidgetData();
      }
      fieldHandler->updateFlags();
    }
  }
  // Update navigation states
  if(row < 1){
    emit toFirstEnabledStateChanged(false);
    emit toPreviousEnabledStateChanged(false);
  }else{
    emit toFirstEnabledStateChanged(true);
    emit toPreviousEnabledStateChanged(true);
  }
  if(row < (model()->rowCount()-1)){
    emit toLastEnabledStateChanged(true);
    emit toNextEnabledStateChanged(true);
  }else{
    emit toLastEnabledStateChanged(false);
    emit toNextEnabledStateChanged(false);
  }
  // emit mdtAbstractSqlWidget's signal
  emit currentRowChanged(row);
}

bool mdtSqlFormWidget::doSubmit()
{
  Q_ASSERT(model() != 0);

  int row;

  // Remember current row (will be lost during submit)
  row = pvWidgetMapper->currentIndex();

  // Do some check before real submit
  if(!checkBeforeSubmit()){
    QMessageBox msgBox;
    msgBox.setText(tr("There are some errors in edited data."));
    msgBox.setInformativeText( \
      tr("Fields that are not correct should be highlighted.\nMoving cursor over field with error should display the reason\nPlease correct errors, or cancel modifications, and try again."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();    return false;
  }
  // Call widget mapper submit() (will commit data from widgets to model)
  if(!pvWidgetMapper->submit()){
    displayDatabaseError(model()->lastError());
    return false;
  }
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    return false;
  }
  // Go back to current row
  pvWidgetMapper->setCurrentIndex(row);

  return true;
}

bool mdtSqlFormWidget::doRevert()
{
  pvWidgetMapper->revert();

  return true;
}

bool mdtSqlFormWidget::doInsert()
{
  Q_ASSERT(model() != 0);

  int row;

  if(currentState() != Inserting){
    warnUserAboutUnsavedRow();
    return false;
  }
  // Insert new row at end
  row = model()->rowCount();
  model()->insertRow(row);
  pvWidgetMapper->setCurrentIndex(row);
  clearWidgets();
  setFocusOnFirstDataWidget();

  return true;
}

bool mdtSqlFormWidget::doSubmitNewRow()
{
  Q_ASSERT(model() != 0);

  return doSubmit();
}

bool mdtSqlFormWidget::doRevertNewRow()
{
  Q_ASSERT(model() != 0);

  int row;

  // Remeber current row and remove it
  row = pvWidgetMapper->currentIndex();
  if(!model()->removeRow(row)){
    displayDatabaseError(model()->lastError());
    return false;
  }
  // Data was never submit to model, we simply go to last row
  clearWidgets();
  pvWidgetMapper->setCurrentIndex(qMin(row, model()->rowCount()-1));

  return true;
}

bool mdtSqlFormWidget::doRemove()
{
  Q_ASSERT(model() != 0);

  int row;
  int ret;
  QMessageBox msgBox;

  // Remeber current row (will be lost during submit)
  row = pvWidgetMapper->currentIndex();
  // If we are not on a row, we do nothing
  if(row < 0){
    return true;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete some data."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return true;
  }
  // Remove current row
  if(!model()->removeRow(row)){
    displayDatabaseError(model()->lastError());
    return false;
  }
  // Call widget mapper submit() (will commit data from widgets to model)
  if(!pvWidgetMapper->submit()){
    displayDatabaseError(model()->lastError());
    return false;
  }
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    return false;
  }
  // Go back to current row
  row = qMin(row, model()->rowCount()-1);
  if(row < 0){
    onCurrentIndexChanged(-1);
  }else{
    pvWidgetMapper->setCurrentIndex(row);
  }

  return true;
}

void mdtSqlFormWidget::clearWidgets()
{
  int i;

  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    pvFieldHandlers.at(i)->clearWidgetData();
  }
}

void mdtSqlFormWidget::setFocusOnFirstDataWidget()
{
  if(pvFirstDataWidget != 0){
    pvFirstDataWidget->setFocus();
  }
}

void mdtSqlFormWidget::setWidgetsEnabled(bool enabled)
{
  int i;
  QWidget *w;

  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    w = pvFieldHandlers.at(i)->dataWidget();
    if(w != 0){
      w->setEnabled(enabled);
    }
  }
}

bool mdtSqlFormWidget::checkBeforeSubmit()
{
  int i;
  bool allOk = true;
  QWidget *firstNokWidget = 0;

  // We call checkBeforeSubmit() for all items
  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    if(!pvFieldHandlers.at(i)->checkBeforeSubmit()){
      allOk = false;
      if(firstNokWidget == 0){
        firstNokWidget = pvFieldHandlers.at(i)->dataWidget();
      }
    }
  }
  // We set the focus on first Nok widget
  if(firstNokWidget != 0){
    firstNokWidget->setFocus();
  }

  return allOk;
}

void mdtSqlFormWidget::warnUserAboutUnsavedRow()
{
  QMessageBox msgBox;

  msgBox.setText(tr("Current record was modified."));
  msgBox.setInformativeText(tr("Please save or cancel your modifications and try again."));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}
