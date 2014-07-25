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
#include "mdtSqlFormWidget.h"
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include "mdtSqlFormWidgetDataValidator.h"
#include "mdtSqlSchemaTable.h"
#include <QDataWidgetMapper>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QLayout>
#include <QLayoutItem>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>
#include <QMessageBox>

#include <QDebug>

mdtSqlFormWidget::mdtSqlFormWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  pvWidgetMapper = new QDataWidgetMapper(this);
  pvWidgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  pvFirstDataWidget = 0;
}

mdtSqlFormWidget::~mdtSqlFormWidget()
{
  qDeleteAll(pvFieldHandlers);
}

void mdtSqlFormWidget::mapFormWidgets(QWidget *widget, const QString &firstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->layout() != 0);
  Q_ASSERT(model() != 0);

  int i, fieldIndex;
  QString fieldName;
  QObjectList objectList;
  QWidget *w;
  mdtSqlFieldHandler *fieldHandler;
  QSqlRecord record;
  mdtSqlSchemaTable st;

  // Clear previous mapping
  ///disconnect(this, SIGNAL(modelSelected()), pvWidgetMapper, SLOT(toFirst()));
  disconnect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  disconnect(pvWidgetMapper, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper->clearMapping();
  pvFirstDataWidget = 0;

  // Search widgets with fld_ as prefix in they objectName
  buildWidgetsList(widget, "fld_");
  // If we want informations about fields, we must get record from database instance
  record = model()->database().record(model()->tableName());
  // Fetch table information - record returned by QSqlDatabase does not return Date and DateTime field infomration
  if(!st.setupFromTable(model()->tableName(), model()->database())){
    return;
  }
  // Map found widgets
  for(i = 0; i < pvFoundWidgets.size(); ++i){
    w = pvFoundWidgets.at(i);
    Q_ASSERT(w != 0);
    fieldName = w->objectName();
    // Search matching field in model
    fieldName = fieldName.right(fieldName.length()-4);
    fieldIndex = model()->record().indexOf(fieldName);
    // If field was found, map it
    if(fieldIndex >= 0){
      fieldHandler = new mdtSqlFieldHandler;
      fieldHandler->setField(st.field(fieldName));
      fieldHandler->setDataWidget(w);
      connect(fieldHandler, SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
      /*
        * When we go out from Editing state, we must update flags
        */
      /// \todo Check redoundances with onCurrentIndexChanged()
      /**
      connect(this, SIGNAL(stateEditingExited()), fieldHandler, SLOT(updateFlags()));
      connect(this, SIGNAL(stateEditingNewRowExited()), fieldHandler, SLOT(updateFlags()));
      */
      pvFieldHandlers.append(fieldHandler);
      // If this widget is the first in focus chain, ref it
      if(w->objectName() == firstWidgetInTabOrder){
        pvFirstDataWidget = w;
      }
      // Add to widget mapper
      pvWidgetMapper->addMapping(fieldHandler, fieldIndex, "data");
    }else{
      w->setEnabled(false);
      mdtError e(tr("Cannot find field for widget '") + w->objectName() + tr("'"), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
      e.commit();
    }
  }
  connect(pvWidgetMapper, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  /*
   * When calling select() , setQuery() , setFilter() or something similar on model,
   * Widget mapper will not update. This problem is not in QTableView
   *  (witch connects model's rowsInserted() , rowsRemoved(), etc signals to internal slots to handle this, I think).
   * As workaround, we will connect a signal signal to Widget mapper.
   */
  ///connect(this, SIGNAL(modelSelected()), pvWidgetMapper, SLOT(toFirst()));
  connect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  if(model()->rowCount() < 1){
    onCurrentIndexChanged(-1);
  }else{
    pvWidgetMapper->toFirst();
  }
  // Add data validator
  addDataValidator(new mdtSqlFormWidgetDataValidator(model(), 0, pvFieldHandlers));
}

void mdtSqlFormWidget::mapFormWidgets(const QString &firstWidgetInTabOrder)
{
  mapFormWidgets(this, firstWidgetInTabOrder);
}

QWidgetList mdtSqlFormWidget::mappedWidgets() const
{
  QWidgetList wList;
  mdtSqlFieldHandler *fh;
  int i;

  for(i = 0; i < pvFieldHandlers.size(); ++i){
    fh = pvFieldHandlers.at(i);
    Q_ASSERT(fh != 0);
    if(fh->dataWidget() != 0){
      wList.append(fh->dataWidget());
    }
  }

  return wList;
}

int mdtSqlFormWidget::currentRow() const
{
  return pvWidgetMapper->currentIndex();
}

void mdtSqlFormWidget::toFirst()
{
  if(!allDataAreSaved()){
    return;
  }
  /**
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  */
  pvWidgetMapper->toFirst();
}

void mdtSqlFormWidget::toLast()
{
  if(!allDataAreSaved()){
    return;
  }
  /**
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  */
  pvWidgetMapper->toLast();
}

void mdtSqlFormWidget::toPrevious()
{
  if(!allDataAreSaved()){
    return;
  }
  /**
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  */
  pvWidgetMapper->toPrevious();
}

void mdtSqlFormWidget::toNext()
{
  if(!allDataAreSaved()){
    return;
  }
  /**
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  */
  pvWidgetMapper->toNext();
}

void mdtSqlFormWidget::setCurrentIndex(int row)
{
  if(!allDataAreSaved()){
    return;
  }
  /**
  if(!childWidgetsAreInVisaluzingState()){
    return;
  }
  if(currentState() != Visualizing){
    warnUserAboutUnsavedRow();
    return;
  }
  */
  pvWidgetMapper->setCurrentIndex(row);
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
      ///fieldHandler->updateFlags();
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

void mdtSqlFormWidget::onModelSelected()
{
  if(model() == 0){
    return;
  }
  if(model()->rowCount() > 0){
    pvWidgetMapper->toFirst();
  }else{
    onCurrentIndexChanged(-1);
  }
}

bool mdtSqlFormWidget::doSubmit()
{
  Q_ASSERT(model() != 0);

  int row;
  QSqlRecord initialRecord;
  QSqlError error;

  // Remember current row (will be lost during submit)
  row = pvWidgetMapper->currentIndex();
  // Remember current record - will help on primary key errors
  initialRecord = model()->record(row);

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
    if(!restorePrimaryKeyDataToModel(initialRecord)){
      error = model()->lastError();
      mdtError e(MDT_DATABASE_ERROR, "Unable to restor primary key after a submitAll error, table: " + model()->tableName(), mdtError::Error);
      e.setSystemError(error.number(), error.text());
      MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
      e.commit();
      return false;
    }
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
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!model()->submitAll()){
    model()->revertRow(row);
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

void mdtSqlFormWidget::warnUserAboutUnsavedRow()
{
  QMessageBox msgBox;

  ///msgBox.setText(tr("Current record was modified."));
  msgBox.setText(tr("Current record was modified in table '") + userFriendlyTableName() + "'");
  msgBox.setInformativeText(tr("Please save or cancel your modifications and try again."));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void mdtSqlFormWidget::buildWidgetsList(QWidget *w, const QString &prefix)
{
  Q_ASSERT(w != 0);
  Q_ASSERT(w->layout() != 0);

  int i;

  pvFoundWidgets.clear();
  for(i = 0; i < w->layout()->count(); ++i){
    searchWidgets(w->layout()->itemAt(i), prefix);
  }
}

void mdtSqlFormWidget::searchWidgets(QLayoutItem *item, const QString &prefix)
{
  Q_ASSERT(item != 0);

  int i;
  QWidget *widget;
  QLayout *layout;
  QString name;

  // If current item is a layout, and search in it if true
  layout = item->layout();
  if(layout != 0){
    for(i = 0; i < layout->count(); ++i){
      searchWidgets(layout->itemAt(i), prefix);
    }
  }
  widget = item->widget();
  if(widget != 0){
    name = widget->objectName();
    if(name.left(prefix.size()) == prefix){
      pvFoundWidgets.append(widget);
    }
    // If widget has a layout, serach in it
    layout = widget->layout();
    if(layout != 0){
      for(i = 0; i < layout->count(); ++i){
        searchWidgets(layout->itemAt(i), prefix);
      }
    }
  }
}
