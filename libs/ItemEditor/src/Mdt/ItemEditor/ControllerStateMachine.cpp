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

// #include "Debug.h"

namespace Mdt{ namespace ItemEditor{

ControllerStateMachine::~ControllerStateMachine()
{
}

void ControllerStateMachine::setModelHasCache(bool hasCache)
{
  Q_ASSERT(mPermissionImpl);

  mPermissionImpl->setModelHasCache(hasCache);
}

void ControllerStateMachine::setHasChildController(bool hasChild)
{
  Q_ASSERT(mPermissionImpl);

  mPermissionImpl->setHasChildController(hasChild);
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

  return mPermissionImpl->canSelect( *mTableImpl );
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

  return mPermissionImpl->isSelectActionEnabled(*mTableImpl);
  return false;
}

void ControllerStateMachine::dataEditionStarted()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::DataEditionStarted);
}

void ControllerStateMachine::dataEditionDone()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::DataEditionDone);
}

void ControllerStateMachine::submitDone()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::SubmitDone);
}

void ControllerStateMachine::revertDone()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::RevertDone);
}

void ControllerStateMachine::insertStarted()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::InsertStarted);
}

void ControllerStateMachine::removeDone()
{
  Q_ASSERT(mTableImpl);

  setEvent(ControllerEvent::RemoveDone);
}

void ControllerStateMachine::setEvent(ControllerEvent event)
{
  Q_ASSERT(mTableImpl);

  const auto previousState = currentState();
  mTableImpl->setEvent(event);
  if(currentState() != previousState){
    emit currentStateChanged();
  }
  emit eventCompleted(event);
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
}

void ControllerStateMachine::createTransitionTable()
{
  Q_ASSERT(mTableImpl);

  mTableImpl->createTable();
}

}} // namespace Mdt{ namespace ItemEditor{
