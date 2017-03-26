/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "ControllerStateMachine.h"
#include "AbstractControllerStateTable.h"
#include "AbstractControllerStatePermission.h"
#include "ControllerEvent.h"

#include "AbstractControllerStateChain.h"

namespace Mdt{ namespace ItemEditor{

ControllerStateMachine::~ControllerStateMachine()
{
}

bool ControllerStateMachine::canChangeCurrentRow() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canChangeCurrentRow( *mTableImpl );
}

bool ControllerStateMachine::canInsert() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canInsert( *mTableImpl );
}

bool ControllerStateMachine::canEdit() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canEdit( *mTableImpl );
}

bool ControllerStateMachine::canSubmit() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canSubmit( *mTableImpl );
}

bool ControllerStateMachine::canRevert() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canRevert( *mTableImpl );
}

bool ControllerStateMachine::canRemove() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->canRemove( *mTableImpl );
}

bool ControllerStateMachine::canSelect() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  ///return mPermissionImpl->canSelect( *mTableImpl );
  return false;
}

bool ControllerStateMachine::isChangeCurrentRowActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->isChangeCurrentRowActionEnabled(*mTableImpl);
}

bool ControllerStateMachine::isInsertActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->isInsertActionEnabled(*mTableImpl);
}

bool ControllerStateMachine::isSubmitActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->isSubmitActionEnabled(*mTableImpl);
}

bool ControllerStateMachine::isRevertActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->isRevertActionEnabled(*mTableImpl);
}

bool ControllerStateMachine::isRemoveActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  return mPermissionImpl->isRemoveActionEnabled(*mTableImpl);
}

bool ControllerStateMachine::isSelectActionEnabled() const
{
  Q_ASSERT(mPermissionImpl);
  Q_ASSERT(mTableImpl);

  ///return mPermissionImpl->isSelectActionEnabled(*mTableImpl);
  return false;
}

void ControllerStateMachine::dataEditionStarted()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->dataEditionStartedState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::DataEditionStarted);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::dataEditionDone()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->dataEditionDoneState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::DataEditionDone);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::submitDone()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->submitDoneState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::SubmitDone);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::revertDone()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->revertDoneState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::RevertDone);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::insertStarted()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->insertStartedState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::InsertStarted);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::removeDone()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->removeDoneState(mCurrentState) );
  const auto previousState = currentState();
  mTableImpl->setEvent(ControllerEvent::RemoveDone);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

void ControllerStateMachine::dataEditionStartedFromParent()
{
  Q_ASSERT(mTableImpl);

//   setCurrentState( mTableImpl->dataEditionStartedFromParentState(mCurrentState) );
  const auto previousState = currentState();
  ///mTableImpl->setEvent(ControllerEvent::Da);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
}

ControllerState ControllerStateMachine::currentState() const
{
  Q_ASSERT(mTableImpl);

  return mTableImpl->currentState();
}

void ControllerStateMachine::forceCurrentState(ControllerState state)
{
  Q_ASSERT(mTableImpl);

  const auto previousState = currentState();

  mTableImpl->forceCurrentState(state);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
//   setCurrentState(state);
}

void ControllerStateMachine::createTransitionTable()
{
  Q_ASSERT(mTableImpl);

  mTableImpl->createTable();
}

// void ControllerStateMachine::setCurrentState(ControllerState state)
// {
//   const bool changed = (state != mCurrentState);
// 
//   mCurrentState = state;
//   if(changed){
//     emit currentStateChanged();
//   }
// }

}} // namespace Mdt{ namespace ItemEditor{
