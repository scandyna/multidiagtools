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
#include <QState>
#include <QStateMachine>
#include <QMessageBox>
#include <QSqlTableModel>

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

int mdtAbstractSqlWidget::rowCount() const
{
  Q_ASSERT(pvModel != 0);

  return pvModel->rowCount();
}

mdtAbstractSqlWidget::state_t mdtAbstractSqlWidget::currentState() const
{
  qDebug() << "Current state: " << pvCurrentState;
  return pvCurrentState;
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
  qDebug() << __FUNCTION__ << " emit insertTriggered() ...";
  emit insertTriggered();
}

void mdtAbstractSqlWidget::remove()
{
  emit removeTriggered();
}

void mdtAbstractSqlWidget::onStateVisualizingEntered()
{
  pvCurrentState = Visualizing;
  qDebug() << __FUNCTION__;
  emit insertEnabledStateChanged(true);
  emit removeEnabledStateChanged(true);
}

void mdtAbstractSqlWidget::onStateVisualizingExited()
{
  qDebug() << __FUNCTION__;
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
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

  int ret;
  QMessageBox msgBox;

  pvCurrentState = Removing;
  // If no row exists, we do nothing
  if(rowCount() < 1){
    emit operationSucceed();
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete some data."));
  msgBox.setInformativeText(tr("Do you really want to delete current record ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  switch(ret){
    case QMessageBox::Yes:
      if(doRemove()){
        emit operationSucceed();
      }
      break;
    default:
      emit operationSucceed();
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
  ///pvStateInserting->addTransition(this, SIGNAL(revertTriggered()), pvStateRevertingNewRow);
  ///pvStateInserting->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmittingNewRow);

  connect(pvStateEditingNewRow, SIGNAL(entered()), this, SLOT(onStateEditingNewRowEntered()));
  connect(pvStateEditingNewRow, SIGNAL(exited()), this, SLOT(onStateEditingNewRowExited()));
  pvStateEditingNewRow->addTransition(this, SIGNAL(revertTriggered()), pvStateRevertingNewRow);
  pvStateEditingNewRow->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmittingNewRow);

  connect(pvStateRevertingNewRow, SIGNAL(entered()), this, SLOT(onStateRevertingNewRowEntered()));
  pvStateRevertingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  ///pvStateRevertingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateInserting);
  pvStateRevertingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateEditingNewRow);
  
  connect(pvStateSubmittingNewRow, SIGNAL(entered()), this, SLOT(onStateSubmittingNewRowEntered()));
  pvStateSubmittingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  ///pvStateSubmittingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateInserting);
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
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
  emit toFirstEnabledStateChanged(false);
  emit toLastEnabledStateChanged(false);
  emit toNextEnabledStateChanged(false);
  emit toPreviousEnabledStateChanged(false);
  ///pvStateMachine->start();
}
