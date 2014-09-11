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
}

mdtSqlDataWidgetController::~mdtSqlDataWidgetController()
{
}

bool mdtSqlDataWidgetController::mapFormWidgets(QWidget* widget, const QString& firstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->layout() != 0);
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Stopped);

  int i, fieldIndex;
  QString fieldName;
  QWidget *w;
  mdtSqlFieldHandler *fieldHandler;
  QSqlRecord record;
  mdtSqlSchemaTable st;

  // Clear previous mapping
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper.clearMapping();
  pvFirstDataWidget = 0;
  // Search widgets with fld_ as prefix in they objectName
  buildWidgetsList(widget, "fld_");
  // If we want informations about fields, we must get record from database instance
  record = model()->database().record(model()->tableName());
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
    // Search matching field in model
    fieldName = fieldName.right(fieldName.length()-4);
    fieldIndex = model()->record().indexOf(fieldName);
    // If field was found, map it
    if(fieldIndex >= 0){
      fieldHandler = new mdtSqlFieldHandler;
      fieldHandler->setField(st.field(fieldName));
      fieldHandler->setDataWidget(w);
      ///connect(fieldHandler, SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
      pvFieldHandlers.append(fieldHandler);
      // If this widget is the first in focus chain, ref it
      if(w->objectName() == firstWidgetInTabOrder){
        pvFirstDataWidget = w;
      }
      // Add to widget mapper
      pvWidgetMapper.addMapping(fieldHandler, fieldIndex, "data");
    }else{
      w->setEnabled(false);
      mdtError e(tr("Cannot find field for widget '") + w->objectName() + tr("'"), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
      e.commit();
    }
  }
  // Update UI
  updateMappedWidgets();
  updateNavigationControls();

  /*
   * When calling select() , setQuery() , setFilter() or something similar on model,
   * Widget mapper will not update. This problem is not in QTableView
   *  (witch connects model's rowsInserted() , rowsRemoved(), etc signals to internal slots to handle this, I think).
   * As workaround, we will connect a signal to Widget mapper.
   */
  ///connect(this, SIGNAL(modelSelected()), pvWidgetMapper, SLOT(toFirst()));
  /**
  connect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  if(model()->rowCount() < 1){
    onCurrentIndexChanged(-1);
  }else{
    pvWidgetMapper->toFirst();
  }
  */
  // Add data validator
  addDataValidator(std::shared_ptr<mdtSqlFormWidgetDataValidator>(new mdtSqlFormWidgetDataValidator(0, pvFieldHandlers)));

  return true;
}

int mdtSqlDataWidgetController::currentRow() const
{
  if(!model()){
    return -1;
  }
  return pvWidgetMapper.currentIndex();
}

void mdtSqlDataWidgetController::toFirst()
{
  if((currentState() != Visualizing)||(!model())){
    return;
  }
  setCurrentRow(0);
}

void mdtSqlDataWidgetController::toLast()
{
  if((currentState() != Visualizing)||(!model())){
    return;
  }
  setCurrentRow(rowCount(true) - 1);
}

void mdtSqlDataWidgetController::toNext()
{
  if((currentState() != Visualizing)||(!model())){
    return;
  }
  setCurrentRow(currentRow() + 1);
}

void mdtSqlDataWidgetController::toPrevious()
{
  if((currentState() != Visualizing)||(!model())){
    return;
  }
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
  updateNavigationControls();
}

void mdtSqlDataWidgetController::modelSetEvent()
{
  Q_ASSERT(model());

  model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvWidgetMapper.setModel(model().get());
}

void mdtSqlDataWidgetController::currentRowChangedEvent(int row)
{
  Q_ASSERT(model());

  pvWidgetMapper.setCurrentIndex(row);
  updateMappedWidgets();
  updateNavigationControls();
}

bool mdtSqlDataWidgetController::doSubmit()
{
  Q_ASSERT(model());

  int row;
  QSqlRecord initialRecord;
  QSqlError sqlError;

  // Remember current row (will be lost during submit)
  row = pvWidgetMapper.currentIndex();
  // Remember current record - will help on primary key errors
  initialRecord = model()->record(row);

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
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  if(model()->rowCount() > 0){
    while((row >= model()->rowCount())&&(model()->canFetchMore())){
      model()->fetchMore();
    }
  }
  pvWidgetMapper.setCurrentIndex(row);

  return true;
}

bool mdtSqlDataWidgetController::doRevert()
{
  pvWidgetMapper.revert();

  return true;
}

bool mdtSqlDataWidgetController::doInsert()
{
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Inserting);

  int row;

  // Insert new row at end
  row = rowCount(true);
  model()->insertRow(row);
  pvWidgetMapper.setCurrentIndex(row);
  clearMappedWidgets();
  /// \todo Set focus on first widget...
  
  return true;
}

bool mdtSqlDataWidgetController::doSubmitNewRow()
{
  Q_ASSERT(model());

  return doSubmit();
}

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
  // Data was never submit to model, we simply go to last row
  /**
  clearMappedWidgets();
  toLast();
  */
  setCurrentRow(row);

  return true;
}

void mdtSqlDataWidgetController::updateMappedWidgets()
{
  bool haveData;
  mdtSqlFieldHandler *fieldHandler;
  int i;

  // We have valid data only if some conditions are met
  if((model())&&(currentState() != Stopped)&&(currentRow() > -1)){
    haveData = true;
  }else{
    haveData = false;
  }
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
