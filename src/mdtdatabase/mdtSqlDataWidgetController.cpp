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
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include "mdtSqlFormWidgetDataValidator.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSortFilterProxyModel.h"
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QLayout>
#include <QLayoutItem>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>

#include <QDebug>

mdtSqlDataWidgetController::mdtSqlDataWidgetController(QObject* parent)
 : mdtAbstractSqlTableController(parent)
{
  pvFirstDataWidget = 0;
  pvWidgetMapper.setModel(proxyModel().get());
}

mdtSqlDataWidgetController::~mdtSqlDataWidgetController()
{
  qDeleteAll(pvFieldHandlers);
}

bool mdtSqlDataWidgetController::mapFormWidgets(QWidget* widget, const QString& firstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->layout() != 0);
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Stopped);

  int i;
  QString fieldName;
  QWidget *w;
  mdtSqlSchemaTable st;
  bool isFirstWidgetInTabOrder;

  // Clear previous mapping
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper.clearMapping();
  pvFirstDataWidget = 0;
  // Search widgets with fld_ as prefix in they objectName
  buildWidgetsList(widget, "fld_");
  // Fetch table information - record returned by QSqlDatabase does not return Date and DateTime field infomration
  if(!st.setupFromTable(model()->tableName(), model()->database())){
    pvLastError = st.lastError();
    return false;
  }
  // Map found widgets
  for(i = 0; i < pvFoundWidgets.size(); ++i){
    w = pvFoundWidgets.at(i);
    Q_ASSERT(w != 0);
    fieldName = w->objectName();
    fieldName = fieldName.right(fieldName.length()-4);
    isFirstWidgetInTabOrder = (w->objectName() == firstWidgetInTabOrder);
    if(!addMapping(w, fieldName, st, isFirstWidgetInTabOrder)){
      qDeleteAll(pvFieldHandlers);
      pvFieldHandlers.clear();
      pvWidgetMapper.clearMapping();
      pvFirstDataWidget = 0;
      return false;
    }
  }
  // Update UI
  updateMappedWidgets();
  updateNavigationControls();
  // Add data validator
  addDataValidator(std::shared_ptr<mdtSqlFormWidgetDataValidator>(new mdtSqlFormWidgetDataValidator(0, pvFieldHandlers)));
  // Found widgets are not needed anymore
  pvFoundWidgets.clear();

  return true;
}

bool mdtSqlDataWidgetController::addMapping(QWidget* widget, const QString& fieldName, bool isFirstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Stopped);

  mdtSqlSchemaTable st;

  if(!st.setupFromTable(model()->tableName(), model()->database())){
    pvLastError = st.lastError();
    return false;
  }

  return addMapping(widget, fieldName, st, isFirstWidgetInTabOrder);
}

bool mdtSqlDataWidgetController::addMapping(QWidget* widget, const QString& fieldName, const mdtSqlSchemaTable& st, bool isFirstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Stopped);

  int fieldIndex;
  mdtSqlFieldHandler *fieldHandler;

  // Search matching field in model
  fieldIndex = model()->record().indexOf(fieldName);
  // If field was found, map it
  if(fieldIndex >= 0){
    fieldHandler = new mdtSqlFieldHandler;
    fieldHandler->setField(st.field(fieldName));
    fieldHandler->setDataWidget(widget);
    connect(fieldHandler, SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
    pvFieldHandlers.append(fieldHandler);
    // If this widget is the first in focus chain, ref it
    if(isFirstWidgetInTabOrder){
      pvFirstDataWidget = widget;
    }
    // Add to widget mapper
    pvWidgetMapper.addMapping(fieldHandler, fieldIndex, "data");
  }else{
    widget->setEnabled(false);
    mdtError e(tr("For table '") + tableName() + tr("': cannot find field for widget '") + widget->objectName() + tr("'"), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
    e.commit();
  }

  return true;
}

/**
int mdtSqlDataWidgetController::currentRow() const
{
  if(!model()){
    return -1;
  }
  return pvWidgetMapper.currentIndex();
}
*/

void mdtSqlDataWidgetController::toFirst()
{
  setCurrentRow(0);
}

void mdtSqlDataWidgetController::toLast()
{
  setCurrentRow(rowCount(true) - 1);
}

void mdtSqlDataWidgetController::toNext()
{
  setCurrentRow(currentRow() + 1);
}

void mdtSqlDataWidgetController::toPrevious()
{
  setCurrentRow(currentRow() - 1);
}

void mdtSqlDataWidgetController::onStateVisualizingEntered()
{
  mdtAbstractSqlTableController::onStateVisualizingEntered();
  updateNavigationControls();
}

void mdtSqlDataWidgetController::onStateVisualizingExited()
{
  mdtAbstractSqlTableController::onStateVisualizingExited();
  /*
   * Because currentState() will return Visualizing untile next state is enterred,
   *  we cannot use updateNavigationControls() here
   */
  emit toFirstEnabledStateChanged(false);
  emit toLastEnabledStateChanged(false);
  emit toPreviousEnabledStateChanged(false);
  emit toNextEnabledStateChanged(false);
}

void mdtSqlDataWidgetController::modelSetEvent()
{
  Q_ASSERT(model());

  model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void mdtSqlDataWidgetController::currentRowChangedEvent(int row)
{
  Q_ASSERT(model());

  qDebug() << tableName() << ": currentRowChangedEvent(" << row << ")";
  pvWidgetMapper.setCurrentIndex(row);
  updateMappedWidgets();
  updateNavigationControls();
}

void mdtSqlDataWidgetController::insertDoneEvent(int row)
{
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Inserting);

  qDebug() << tableName() << ": insertDoneEvent(" << row << ")";
  pvWidgetMapper.setCurrentIndex(row);
  clearMappedWidgets();
  updateMappedWidgets();
  setFocusOnFirstDataWidget();
}

bool mdtSqlDataWidgetController::doSubmit()
{
  Q_ASSERT(model());

  ///int row;
  ///QSqlRecord initialRecord;
  QSqlError sqlError;

  // Remember current row (will be lost during submit)
  ///row = pvWidgetMapper.currentIndex();
  // Remember current record - will help on primary key errors
  ///initialRecord = model()->record(row);

  qDebug() << tableName() << ": doSubmit() - 1) - row count: " << model()->rowCount();
  
  // Call widget mapper submit() (will commit data from widgets to model)
  if(!pvWidgetMapper.submit()){
    sqlError = model()->lastError();
    pvLastError.setError(tr("Submitting data to model failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDataWidgetController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  
  qDebug() << tableName() << ": doSubmit() - 2) - row count: " << model()->rowCount();
  
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!model()->submitAll()){
    /** \todo Implement restorePrimaryKeyDataToModel()
    if(!restorePrimaryKeyDataToModel(initialRecord)){
      error = model()->lastError();
      mdtError e(MDT_DATABASE_ERROR, "Unable to restor primary key after a submitAll error, table: " + model()->tableName(), mdtError::Error);
      e.setSystemError(error.number(), error.text());
      MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
      e.commit();
      return false;
    }
    */
    sqlError = model()->lastError();
    pvLastError.setError(tr("Submitting data to database failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDataWidgetController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  
  qDebug() << tableName() << ": doSubmit() - 3) - row count: " << model()->rowCount();
  
  qDebug() << tableName() << ": filter: " << model()->filter();
  
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  /**
  if(model()->rowCount() > 0){
    while((row >= model()->rowCount())&&(model()->canFetchMore())){
      model()->fetchMore();
    }
  }
  pvWidgetMapper.setCurrentIndex(row);
  */

  return true;
}

bool mdtSqlDataWidgetController::doRevert()
{
  Q_ASSERT(model());

  pvWidgetMapper.revert();

  return true;
}

/**
bool mdtSqlDataWidgetController::doInsert()
{
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Inserting);

  int row;

  // Remember current row, on case user wants to revert
  pvBeforeInsertCurrentRow = currentRow();
  // Insert new row at end
  row = rowCount(true);
  model()->insertRow(row);
  pvWidgetMapper.setCurrentIndex(row);
  clearMappedWidgets();
  setFocusOnFirstDataWidget();

  return true;
}
*/

bool mdtSqlDataWidgetController::doSubmitNewRow()
{
  Q_ASSERT(model());

  return doSubmit();
}

/**
bool mdtSqlDataWidgetController::doRevertNewRow()
{
  Q_ASSERT(model());

  int row;
  QSqlError sqlError;

  // Remeber current row and remove it
  row = pvWidgetMapper.currentIndex();
  if(!model()->removeRow(row)){
    sqlError = model()->lastError();
    pvLastError.setError(tr("Reverting data failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDataWidgetController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  ///pvWidgetMapper.setCurrentIndex(pvBeforeInsertCurrentRow);

  return true;
}
*/

bool mdtSqlDataWidgetController::doRemove()
{
  Q_ASSERT(model());

  ///int row;
  QSqlError sqlError;

  // Remeber current row (will be lost during submit)
  ///row = pvWidgetMapper.currentIndex();
  // If we are not on a row, we do nothing
  /**
  if(row < 0){
    return true;
  }
  */
  // Remove current row
  if(!model()->removeRow(currentRow())){
    sqlError = model()->lastError();
    pvLastError.setError(tr("Removing data failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDataWidgetController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  /*
   * We use QDataWidgetMapper::ManualSubmit submit policy and QSqlTableModel::OnManualSubmit edit strategy.
   * Widget mapper calls submit() on model, but this has no effect with OnManualSubmit edit strategy,
   * so we have to call submitAll() on model.
   */
  if(!model()->submitAll()){
    model()->revertRow(currentRow());
    sqlError = model()->lastError();
    pvLastError.setError(tr("Removing data failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDataWidgetController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  ///row = qMin(row, rowCount(true) - 1);
  ///setCurrentRowPv(row, false);

  return true;
}

void mdtSqlDataWidgetController::updateMappedWidgets()
{
  bool haveData;
  mdtSqlFieldHandler *fieldHandler;
  int i;

  // We have valid data only if some conditions are met
  if((model())&&(currentState() != Stopped)&&(currentRow() >= 0)){
    haveData = true;
  }else{
    haveData = false;
  }
  
  qDebug() << tableName() << ": updateMappedWidgets() - haveData: " << haveData;
  
  // Update widgets
  for(i = 0; i < pvFieldHandlers.size(); ++i){
    fieldHandler = pvFieldHandlers.at(i);
    Q_ASSERT(fieldHandler != 0);
    if(fieldHandler->dataWidget() != 0){
      fieldHandler->dataWidget()->setEnabled(haveData);
      if(!haveData){
        fieldHandler->clearWidgetData();
      }
    }
  }
}

void mdtSqlDataWidgetController::setFocusOnFirstDataWidget()
{
  if(pvFirstDataWidget != 0){
    pvFirstDataWidget->setFocus();
  }
}

void mdtSqlDataWidgetController::clearMappedWidgets()
{
  int i;

  for(i=0; i<pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    pvFieldHandlers.at(i)->clearWidgetData();
  }
}

void mdtSqlDataWidgetController::updateNavigationControls()
{
  int row;

  if((!model())||(currentState() != Visualizing)){
    emit toFirstEnabledStateChanged(false);
    emit toLastEnabledStateChanged(false);
    emit toPreviousEnabledStateChanged(false);
    emit toNextEnabledStateChanged(false);
    return;
  }
  Q_ASSERT(model());
  row = currentRow();
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
}


void mdtSqlDataWidgetController::buildWidgetsList(QWidget *w, const QString &prefix)
{
  Q_ASSERT(w != 0);
  Q_ASSERT(w->layout() != 0);

  int i;

  pvFoundWidgets.clear();
  for(i = 0; i < w->layout()->count(); ++i){
    searchWidgets(w->layout()->itemAt(i), prefix);
  }
}

void mdtSqlDataWidgetController::searchWidgets(QLayoutItem *item, const QString &prefix)
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
