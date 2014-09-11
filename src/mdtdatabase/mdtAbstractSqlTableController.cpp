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
#include "mdtAbstractSqlTableController.h"
#include "mdtState.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QCoreApplication>

#include <QDebug>

using namespace std;

mdtAbstractSqlTableController::mdtAbstractSqlTableController(QObject* parent)
 : QObject(parent)
{
  buildStateMachine();
}

mdtAbstractSqlTableController::~mdtAbstractSqlTableController()
{
}

void mdtAbstractSqlTableController::setMessageHandler(shared_ptr< mdtUiMessageHandler > handler)
{
  Q_ASSERT(handler);

  int i;

  pvMessageHandler = handler;
  for(i = 0; i < pvDataValidators.size(); ++i){
    Q_ASSERT(pvDataValidators.at(i));
    pvDataValidators.at(i)->setMessageHandler(pvMessageHandler);
  }
}

void mdtAbstractSqlTableController::setTableName(const QString& tableName, QSqlDatabase db, const QString& userFriendlyTableName)
{
  shared_ptr<QSqlTableModel> m(new QSqlTableModel(0, db));
  m->setTable(tableName);
  setModel(m, userFriendlyTableName);
}

void mdtAbstractSqlTableController::setModel(shared_ptr< QSqlTableModel > m, const QString& userFriendlyTableName)
{
  Q_ASSERT(m);

  pvModel = m;
  if(userFriendlyTableName.isEmpty()){
    pvUserFriendlyTableName = m->tableName();
  }else{
    pvUserFriendlyTableName = userFriendlyTableName;
  }
  modelSetEvent();
}

bool mdtAbstractSqlTableController::select()
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  bool ok;

  emit selectTriggered();
  pvStateMachine.waitOnState(Selecting);

  // Call select on model
  ok = pvModel->select();
  if(!ok){
    QSqlError sqlError = pvModel->lastError();
    pvLastError.setError(tr("Select failed for") + "'" + pvUserFriendlyTableName + "' (" + pvModel->tableName() + ")" , mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
  }
  // Go to first row
  if(pvModel->rowCount() > 0){
    setCurrentRow(0);
  }else{
    setCurrentRow(-1);
  }
  if(ok){
    emit operationSucceed();
    pvStateMachine.waitOnState(Visualizing);
  }

  return ok;
}

void mdtAbstractSqlTableController::start()
{
  Q_ASSERT(pvModel);

  if(pvStateMachine.isRunning()){
    stop();
  }
  pvStateMachine.start(true);
  emit globalWidgetEnableStateChanged(true);
}

void mdtAbstractSqlTableController::stop()
{
  Q_ASSERT(pvModel);

  if(!pvStateMachine.isRunning()){
    return;
  }
  emit globalWidgetEnableStateChanged(false);
  pvStateMachine.stop(true);
}

void mdtAbstractSqlTableController::addDataValidator(shared_ptr< mdtSqlDataValidator > validator, bool putAtTopPriority, bool setInternalMessageHandler)
{
  Q_ASSERT(validator);

  if(setInternalMessageHandler && pvMessageHandler){
    validator->setMessageHandler(pvMessageHandler);
  }
  if(putAtTopPriority){
    pvDataValidators.prepend(validator);
  }else{
    pvDataValidators.append(validator);
  }
}

QVariant mdtAbstractSqlTableController::currentData(const QString& fieldName)
{
  Q_ASSERT(pvModel);

  return data(currentRow(), fieldName);
}

QVariant mdtAbstractSqlTableController::currentData(const QString& fieldName, bool & ok)
{
  Q_ASSERT(pvModel);

  return data(currentRow(), fieldName, ok);
}

QVariant mdtAbstractSqlTableController::data(int row, const QString& fieldName)
{
  Q_ASSERT(pvModel);

  bool ok;

  return data(row, fieldName, ok);
}

QVariant mdtAbstractSqlTableController::data(int row, const QString& fieldName, bool & ok)
{
  Q_ASSERT(pvModel);

  int column;
  QModelIndex index;

  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  index = pvModel->index(row, column);
  if(!index.isValid()){
    ok = false;
    pvLastError.setError(tr("Index with row") + " " + QString::number(row)\
               + " " + tr(", and column") + " " + QString::number(column) + " " + tr("is invalid, table:") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  ok = true;

  return pvModel->data(index);
}

int mdtAbstractSqlTableController::rowCount(bool fetchAll) const
{
  if(!pvModel){
    return 0;
  }
  if(pvModel->rowCount() < 1){
    return 0;
  }
  if(fetchAll){
    while(pvModel->canFetchMore()){
      pvModel->fetchMore();
    }
  }
  return pvModel->rowCount();
}

bool mdtAbstractSqlTableController::setCurrentRow(int row)
{
  Q_ASSERT(pvModel);

  // Check that state machine runs
  if(!pvStateMachine.isRunning()){
    pvLastError.setError(tr("Cannot change current row because state machine is stopped."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Check about unsaved data
  if(!allDataAreSaved()){
    return false;
  }
  // If we are allready at requested row, we do nothing (prevent cyclic calls)
  if(row == currentRow()){
    return true;
  }
  // With empty model, we have a special case, baucause fetchMore will allways return true (infinite loop)
  if(pvModel->rowCount() < 1){
    if(row >= 0){
      return false;
    }
    // Send event
    currentRowChangedEvent(-1);
    return true;
  }
  // Fetch rows in database if needed
  Q_ASSERT(pvModel->rowCount() > 0);
  while((row >= pvModel->rowCount())&&(pvModel->canFetchMore())){
    pvModel->fetchMore();
  }
  // Check range and send event if OK
  if(row >= pvModel->rowCount()){
    return false;
  }
  currentRowChangedEvent(row);

  return true;
}

bool mdtAbstractSqlTableController::allDataAreSaved()
{
  if((currentState() != Visualizing)&&(currentState() != Selecting)&&(currentState() != Stopped)){
    if(pvMessageHandler){
      QString msg;
      pvMessageHandler->setType(mdtUiMessageHandler::Warning);
      msg = tr("For table");
      msg += " '" + userFriendlyTableName() + "' ";
      msg += tr(": some data are not saved.");
      pvMessageHandler->setText(msg);
      msg = tr("Please save or cancel modifications and try again.");
      pvMessageHandler->displayToUser();
    }
    return false;
  }
  /// \todo Check state of all child controllers
  return true;
}

bool mdtAbstractSqlTableController::submitAndWait()
{
  Q_ASSERT(pvModel);

  pvLastError.clear();
  pvOperationComplete = false;
  emit submitTriggered();
  waitOperationComplete();
  if(pvLastError.level() != mdtError::NoError){
    return false;
  }

  return true;
}

void mdtAbstractSqlTableController::submit()
{
  emit submitTriggered();
}

void mdtAbstractSqlTableController::revert()
{
  emit revertTriggered();
}

void mdtAbstractSqlTableController::insert()
{
  if(!allDataAreSaved()){
    return;
  }
  emit insertTriggered();
}

void mdtAbstractSqlTableController::remove()
{
  emit removeTriggered();
}

void mdtAbstractSqlTableController::onStateSelectingEntered()
{
  qDebug() << pvModel->tableName() <<  __FUNCTION__;

  emit globalWidgetEnableStateChanged(false);
}

void mdtAbstractSqlTableController::onStateVisualizingEntered()
{
  qDebug() << pvModel->tableName() <<  __FUNCTION__;

  emit globalWidgetEnableStateChanged(true);
  ///enableChildWidgets();
  emit insertEnabledStateChanged(true);
  emit removeEnabledStateChanged(true);
  pvOperationComplete = true;
}

void mdtAbstractSqlTableController::onStateVisualizingExited()
{
  qDebug() << __FUNCTION__;
  
  ///disableChildWidgets();
  
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
}

void mdtAbstractSqlTableController::onStateEditingEntered()
{
  qDebug() << __FUNCTION__;

  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlTableController::onStateEditingExited()
{
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingExited();
}

void mdtAbstractSqlTableController::onStateSubmittingEntered()
{
  qDebug() << __FUNCTION__;

  if(!checkBeforeSubmit()){
    emit errorOccured();
    return;
  }
  if(doSubmit()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }
}

void mdtAbstractSqlTableController::onStateRevertingEntered()
{
  bool doIt;

  qDebug() << __FUNCTION__;

  // We ask confirmation to the user
  if(pvMessageHandler){
    pvMessageHandler->setText(tr("You choosed to not save modification. This will restore data from database."));
    pvMessageHandler->setInformativeText(tr("Do you really want to loose your modification ?"));
    pvMessageHandler->setType(mdtUiMessageHandler::Warning);
    doIt = pvMessageHandler->askUser();
  }else{
    doIt = true;
  }
  // Do revert
  if(doIt){
    if(doRevert()){
      emit operationSucceed();
    }else{
      emit errorOccured();
    }
  }else{
    emit errorOccured();  // To exit reverting state
  }
}

void mdtAbstractSqlTableController::onStateInsertingEntered()
{
  qDebug() << __FUNCTION__;

  if(doInsert()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }
}

void mdtAbstractSqlTableController::onStateEditingNewRowEntered()
{
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlTableController::onStateEditingNewRowExited()
{
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingNewRowExited();
}

void mdtAbstractSqlTableController::onStateSubmittingNewRowEntered()
{
  qDebug() << __FUNCTION__;

  if(!checkBeforeSubmit()){
    emit errorOccured();
    return;
  }
  if(doSubmitNewRow()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }
}

void mdtAbstractSqlTableController::onStateRevertingNewRowEntered()
{
  bool doIt;

  qDebug() << __FUNCTION__;

  // We ask confirmation to the user
  if(pvMessageHandler){
    pvMessageHandler->setText(tr("You choosed to not save data."));
    pvMessageHandler->setInformativeText(tr("Do you really want to loose new record ?"));
    pvMessageHandler->setType(mdtUiMessageHandler::Warning);
    doIt = pvMessageHandler->askUser();
  }else{
    doIt = true;
  }
  // Do revert
  if(doIt){
    if(doRevert()){
      emit operationSucceed();
    }else{
      emit errorOccured();
    }
  }else{
    emit errorOccured();  // To exit reverting state
  }
}

void mdtAbstractSqlTableController::onStateRemovingEntered()
{
  Q_ASSERT(pvModel);

  qDebug() << __FUNCTION__;

  // If no row exists, we do nothing
  if(rowCount() < 1){
    emit operationSucceed();
    return;
  }
  // Remove - Note: we ignore return value, because we return allways to Visualizing state
  ///doRemove();
  emit operationSucceed();
}

bool mdtAbstractSqlTableController::checkBeforeSubmit()
{
  int i;

  for(i = 0; i < pvDataValidators.size(); ++i){
    Q_ASSERT(pvDataValidators.at(i));
    if(!pvDataValidators.at(i)->checkBeforeSubmit()){
      return false;
    }
  }

  return true;
}

void mdtAbstractSqlTableController::waitOperationComplete()
{
  while(!pvOperationComplete){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
}

void mdtAbstractSqlTableController::buildStateMachine()
{
  pvStateSelecting = new mdtState(Selecting);
  pvStateVisualizing = new mdtState(Visualizing);
  pvStateReverting = new mdtState(Reverting);
  pvStateEditing = new mdtState(Editing);
  pvStateSubmitting = new mdtState(Submitting);
  pvStateInserting = new mdtState(Inserting);
  pvStateEditingNewRow = new mdtState(EditingNewRow);
  pvStateRevertingNewRow = new mdtState(RevertingNewRow);
  pvStateSubmittingNewRow = new mdtState(SubmittingNewRow);
  pvStateRemoving = new mdtState(Removing);
  // Setup transitions
  connect(pvStateSelecting, SIGNAL(entered()), this, SLOT(onStateSelectingEntered()));
  pvStateSelecting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  connect(pvStateVisualizing, SIGNAL(entered()), this, SLOT(onStateVisualizingEntered()));
  connect(pvStateVisualizing, SIGNAL(exited()), this, SLOT(onStateVisualizingExited()));
  pvStateVisualizing->addTransition(this, SIGNAL(selectTriggered()), pvStateSelecting);
  pvStateVisualizing->addTransition(this, SIGNAL(dataEdited()), pvStateEditing);
  pvStateVisualizing->addTransition(this, SIGNAL(insertTriggered()), pvStateInserting);
  pvStateVisualizing->addTransition(this, SIGNAL(removeTriggered()), pvStateRemoving);
  connect(pvStateReverting, SIGNAL(entered()), this, SLOT(onStateRevertingEntered()));
  pvStateReverting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateReverting->addTransition(this, SIGNAL(errorOccured()), pvStateEditing);
  connect(pvStateEditing, SIGNAL(entered()), this, SLOT(onStateEditingEntered()));
  connect(pvStateEditing, SIGNAL(exited()), this, SLOT(onStateEditingExited()));
  pvStateEditing->addTransition(this, SIGNAL(revertTriggered()), pvStateReverting);
  pvStateEditing->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmitting);
  connect(pvStateSubmitting, SIGNAL(entered()), this, SLOT(onStateSubmittingEntered()));
  pvStateSubmitting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateSubmitting->addTransition(this, SIGNAL(errorOccured()), pvStateEditing);
  connect(pvStateInserting, SIGNAL(entered()), this, SLOT(onStateInsertingEntered()));
  pvStateInserting->addTransition(this, SIGNAL(operationSucceed()), pvStateEditingNewRow);
  pvStateInserting->addTransition(this, SIGNAL(errorOccured()), pvStateVisualizing);
  connect(pvStateEditingNewRow, SIGNAL(entered()), this, SLOT(onStateEditingNewRowEntered()));
  connect(pvStateEditingNewRow, SIGNAL(exited()), this, SLOT(onStateEditingNewRowExited()));
  pvStateEditingNewRow->addTransition(this, SIGNAL(revertTriggered()), pvStateRevertingNewRow);
  pvStateEditingNewRow->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmittingNewRow);
  connect(pvStateRevertingNewRow, SIGNAL(entered()), this, SLOT(onStateRevertingNewRowEntered()));
  pvStateRevertingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateRevertingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateEditingNewRow);
  connect(pvStateSubmittingNewRow, SIGNAL(entered()), this, SLOT(onStateSubmittingNewRowEntered()));
  pvStateSubmittingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateSubmittingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateEditingNewRow);
  connect(pvStateRemoving, SIGNAL(entered()), this, SLOT(onStateRemovingEntered()));
  pvStateRemoving->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  // Add states to state machine and start
  pvStateMachine.addState(pvStateSelecting);
  pvStateMachine.addState(pvStateVisualizing);
  pvStateMachine.addState(pvStateReverting);
  pvStateMachine.addState(pvStateEditing);
  pvStateMachine.addState(pvStateSubmitting);
  pvStateMachine.addState(pvStateInserting);
  pvStateMachine.addState(pvStateEditingNewRow);
  pvStateMachine.addState(pvStateRevertingNewRow);
  pvStateMachine.addState(pvStateSubmittingNewRow);
  pvStateMachine.addState(pvStateRemoving);
  pvStateMachine.setInitialState(pvStateVisualizing);
}
