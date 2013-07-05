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
#include "mdtAbstractSqlWidget.h"
#include "mdtError.h"
#include "mdtSqlRelation.h"
#include <QState>
#include <QStateMachine>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlIndex>

#include <QDebug>

mdtAbstractSqlWidget::mdtAbstractSqlWidget(QWidget *parent)
 : QWidget(parent)
{
  pvModel = 0;
  buildStateMachine();
}

mdtAbstractSqlWidget::~mdtAbstractSqlWidget()
{
}

void mdtAbstractSqlWidget::setModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);
  Q_ASSERT(!pvStateMachine->isRunning());

  doSetModel(model);
  pvModel = model;
  pvStateMachine->start();
}

QSqlTableModel *mdtAbstractSqlWidget::model()
{
  return pvModel;
}

QString mdtAbstractSqlWidget::userFriendlyTableName() const
{
  Q_ASSERT(pvModel != 0);

  if(!pvUserFriendlyTableName.isEmpty()){
    return pvUserFriendlyTableName;
  }
  return pvModel->tableName();
}

void mdtAbstractSqlWidget::addChildWidget(mdtAbstractSqlWidget *widget, mdtSqlRelation *relation)
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(widget != 0);
  Q_ASSERT(relation != 0);

  // Add child to lists
  pvChildWidgets.append(widget);
  pvRelations.append(relation);
  // We reparent relation, so it will be deleted at the right moment by Qt
  relation->setParent(this);
  // Make needed connections
  connect(this, SIGNAL(currentRowChanged(int)), relation, SLOT(setParentCurrentIndex(int)));
  // Force a update of relations
  relation->setParentCurrentIndex(currentRow());
}

int mdtAbstractSqlWidget::rowCount() const
{
  Q_ASSERT(pvModel != 0);

  return pvModel->rowCount();
}

mdtAbstractSqlWidget::state_t mdtAbstractSqlWidget::currentState() const
{
  return pvCurrentState;
}

void mdtAbstractSqlWidget::enableLocalNavigation()
{
}

void mdtAbstractSqlWidget::enableLocalEdition()
{
}

bool mdtAbstractSqlWidget::allDataAreSaved()
{
  // Check main SQL widget state
  if(currentState() != mdtAbstractSqlWidget::Visualizing){
    QMessageBox msgBox;
    msgBox.setText(tr("Current record was modified in table '") + userFriendlyTableName() + "'");
    msgBox.setInformativeText(tr("Please save or cancel your modifications before closing the edition window."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    return false;
  }
  // Check child SQL widgets
  return childWidgetsAreInVisaluzingState();
}

void mdtAbstractSqlWidget::submit()
{
  emit submitTriggered();
}

void mdtAbstractSqlWidget::revert()
{
  emit revertTriggered();
}

void mdtAbstractSqlWidget::insert()
{
  emit insertTriggered();
}

void mdtAbstractSqlWidget::remove()
{
  emit removeTriggered();
}

void mdtAbstractSqlWidget::displayDatabaseError(QSqlError error)
{
  Q_ASSERT(pvModel != 0);

  QMessageBox msgBox;
  QString baseText;
  QString infoText;
  QString systemText;

  // Fill base text
  baseText = tr("A database error occured on table ");
  baseText += "'" + userFriendlyTableName() + "'";
  // Fill system text
  systemText = "Error number: " + QString::number(error.number()) + "\n ";
  systemText += "DB text: " + error.databaseText() + "\n ";
  systemText += "Driver text: " + error.driverText();
  // Fill info text
  if(pvModel->database().driverName() == "QSQLITE"){
    infoText = getUserReadableTextFromSqliteError(error);
  }else if(pvModel->database().driverName() == "QMYSQL"){
    infoText = getUserReadableTextFromMysqlError(error);
  }else{
    infoText = error.text();
  }

  // Register error
  mdtError e(MDT_DATABASE_ERROR, baseText + " Table: " + pvModel->tableName(), mdtError::Error);
  e.setSystemError(error.number(), systemText);
  MDT_ERROR_SET_SRC(e, "mdtAbstractSqlWidget");
  e.commit();
  // Display error
  msgBox.setText(baseText + "                      ");
  msgBox.setInformativeText(infoText);
  msgBox.setDetailedText(systemText);
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

QString mdtAbstractSqlWidget::getUserReadableTextFromSqliteError(const QSqlError &error)
{
  QString text;

  switch(error.number()){
    case 10:  // Disk I/O error
      text = tr("Cannot update database file.") + "\n";
      text += tr("If you are working on a removable device (USB key, USB hard drive, ...), please check that it is inserted and try again.") + "\n";
      text += tr("If your database file is on a remote server, please check network connection and try again.");
      break;
    case 19:  // Constraint violation
      text = tr("Please check that all required fields are entered and that you don't have a duplicated primary key.") + "\n";
      text += tr("For more precision, please see details to know what the system returned.");
      break;
    case 20:  // Data type error
      text = tr("A field contains invalid data type.") + "\n";
      text += tr("For example, check that you not typed a char in a numeric field.");
      break;
    default:
      text = error.text();
  }

  return text;
}

QString mdtAbstractSqlWidget::getUserReadableTextFromMysqlError(const QSqlError &error)
{
  QString text;

  switch(error.number()){
    default:
      text = error.text();
  }

  return text;
}

bool mdtAbstractSqlWidget::childWidgetsAreInVisaluzingState()
{
  int i;
  mdtAbstractSqlWidget *w;

  for(i=0; i<pvChildWidgets.size(); ++i){
    w = pvChildWidgets.at(i);
    Q_ASSERT(w != 0);
    if(w->currentState() != Visualizing){
      warnUserAboutUnsavedRow(w->userFriendlyTableName());
      return false;
    }
  }

  return true;
}

void mdtAbstractSqlWidget::warnUserAboutUnsavedRow(const QString &tableName)
{
  QMessageBox msgBox;

  msgBox.setText(tr("Some data are not saved in table '") + tableName + "'");
  msgBox.setInformativeText(tr("Please save or cancel your modifications and try again."));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool mdtAbstractSqlWidget::restorePrimaryKeyDataToModel(const QSqlRecord &previousRecord)
{
  Q_ASSERT(pvModel != 0);

  QSqlIndex primaryKey;
  int pkIndex;
  int indexInRecord;
  QSqlRecord currentRecord;
  bool pkUpdated = false;

  currentRecord = pvModel->record(currentRow());
  primaryKey = pvModel->primaryKey();
  for(pkIndex = 0; pkIndex < primaryKey.count(); ++pkIndex){
    indexInRecord = previousRecord.indexOf(primaryKey.fieldName(pkIndex));
    if(currentRecord.value(indexInRecord) != previousRecord.value(indexInRecord)){
      pkUpdated = true;
      currentRecord.setValue(indexInRecord, previousRecord.value(indexInRecord));
    }
  }
  if(pkUpdated){
    if(!pvModel->setRecord(currentRow(), currentRecord)){
      return false;
    }
  }

  return true;
}

void mdtAbstractSqlWidget::onStateVisualizingEntered()
{
  pvCurrentState = Visualizing;
  qDebug() << __FUNCTION__;

  enableChildWidgets();
  emit insertEnabledStateChanged(true);
  emit removeEnabledStateChanged(true);
  emit stateVisualizingEntered();
}

void mdtAbstractSqlWidget::onStateVisualizingExited()
{
  qDebug() << __FUNCTION__;
  
  disableChildWidgets();
  
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
  emit stateVisualizingExited();
}

void mdtAbstractSqlWidget::onStateEditingEntered()
{
  qDebug() << __FUNCTION__;
  pvCurrentState = Editing;
  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlWidget::onStateEditingExited()
{
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingExited();
}

void mdtAbstractSqlWidget::onStateSubmittingEntered()
{
  pvCurrentState = Submitting;
  qDebug() << __FUNCTION__;

  if(doSubmit()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }
}

void mdtAbstractSqlWidget::onStateRevertingEntered()
{
  int ret;
  QMessageBox msgBox;

  pvCurrentState = Reverting;
  qDebug() << __FUNCTION__;

  // We ask confirmation to the user
  msgBox.setText(tr("You choosed to not save modification. This will restore data from database."));
  msgBox.setInformativeText(tr("Do you really want to loose your modification ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  switch(ret){
    case QMessageBox::Yes:
      if(doRevert()){
        emit operationSucceed();
      }else{
        emit errorOccured();
      }
      break;
    default:
      emit errorOccured();
  }
}

void mdtAbstractSqlWidget::onStateInsertingEntered()
{
  pvCurrentState = Inserting;
  qDebug() << __FUNCTION__;

  if(!childWidgetsAreInVisaluzingState()){
    emit errorOccured();
    return;
  }
  if(doInsert()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }

}

void mdtAbstractSqlWidget::onStateEditingNewRowEntered()
{
  pvCurrentState = EditingNewRow;
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlWidget::onStateEditingNewRowExited()
{
  qDebug() << __FUNCTION__;
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingNewRowExited();
}

void mdtAbstractSqlWidget::onStateSubmittingNewRowEntered()
{
  pvCurrentState = SubmittingNewRow;
  qDebug() << __FUNCTION__;

  if(doSubmitNewRow()){
    emit operationSucceed();
  }else{
    emit errorOccured();
  }
}

void mdtAbstractSqlWidget::onStateRevertingNewRowEntered()
{
  int ret;
  QMessageBox msgBox;

  pvCurrentState = RevertingNewRow;
  qDebug() << __FUNCTION__;

  // We ask confirmation to the user
  msgBox.setText(tr("You choosed to not save data."));
  msgBox.setInformativeText(tr("Do you really want to loose new record ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  switch(ret){
    case QMessageBox::Yes:
      if(doRevertNewRow()){
        emit operationSucceed();
      }else{
        emit errorOccured();
      }
      break;
    default:
      emit errorOccured();
  }
}

void mdtAbstractSqlWidget::onStateRemovingEntered()
{
  Q_ASSERT(pvModel != 0);

  qDebug() << __FUNCTION__;

  pvCurrentState = Removing;
  // If no row exists, we do nothing
  if(rowCount() < 1){
    emit operationSucceed();
    return;
  }
  // Remove - Note: we ignore return value, because we return allways to Visualizing state
  doRemove();
  emit operationSucceed();
}

void mdtAbstractSqlWidget::disableChildWidgets()
{
  int i;

  for(i=0; i<pvChildWidgets.size(); ++i){
    Q_ASSERT(pvChildWidgets.at(i) != 0);
    pvChildWidgets.at(i)->setEnabled(false);
  }
}

void mdtAbstractSqlWidget::enableChildWidgets()
{
  int i;

  for(i=0; i<pvChildWidgets.size(); ++i){
    Q_ASSERT(pvChildWidgets.at(i) != 0);
    pvChildWidgets.at(i)->setEnabled(true);
  }
}

void mdtAbstractSqlWidget::buildStateMachine()
{
  // Create objects
  pvStateMachine = new QStateMachine(this);
  pvStateVisualizing = new QState;
  pvStateReverting = new QState;
  pvStateEditing = new QState;
  pvStateSubmitting = new QState;
  pvStateInserting = new QState;
  pvStateEditingNewRow = new QState;
  pvStateRevertingNewRow = new QState;
  pvStateSubmittingNewRow = new QState;
  pvStateRemoving = new QState;
  // Setup transitions
  connect(pvStateVisualizing, SIGNAL(entered()), this, SLOT(onStateVisualizingEntered()));
  connect(pvStateVisualizing, SIGNAL(exited()), this, SLOT(onStateVisualizingExited()));
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
  pvStateMachine->addState(pvStateVisualizing);
  pvStateMachine->addState(pvStateReverting);
  pvStateMachine->addState(pvStateEditing);
  pvStateMachine->addState(pvStateSubmitting);
  pvStateMachine->addState(pvStateInserting);
  pvStateMachine->addState(pvStateEditingNewRow);
  pvStateMachine->addState(pvStateRevertingNewRow);
  pvStateMachine->addState(pvStateSubmittingNewRow);
  pvStateMachine->addState(pvStateRemoving);
  pvStateMachine->setInitialState(pvStateVisualizing);
  pvCurrentState = Visualizing;
  /**
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
  emit toFirstEnabledStateChanged(false);
  emit toLastEnabledStateChanged(false);
  emit toNextEnabledStateChanged(false);
  emit toPreviousEnabledStateChanged(false);
  */
}
