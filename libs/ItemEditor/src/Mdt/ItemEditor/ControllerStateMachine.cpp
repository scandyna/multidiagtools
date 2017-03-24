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
#include "AbstractControllerStateChain.h"
#include "AbstractControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

ControllerStateMachine::~ControllerStateMachine()
{
}

bool ControllerStateMachine::canChangeCurrentRow() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canChangeCurrentRow( mCurrentState );
}

bool ControllerStateMachine::canInsert() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canInsert( mCurrentState );
}

bool ControllerStateMachine::canEdit() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canEdit( mCurrentState );
}

bool ControllerStateMachine::canSubmit() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canSubmit( mCurrentState );
}

bool ControllerStateMachine::canRevert() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canRevert( mCurrentState );
}

bool ControllerStateMachine::canRemove() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canRemove( mCurrentState );
}

bool ControllerStateMachine::canSelect() const
{
  Q_ASSERT(mPermissionImpl);

  return mPermissionImpl->canSelect( mCurrentState );
}

void ControllerStateMachine::onDataEditionStarted()
{

}

void ControllerStateMachine::forceCurrentState(ControllerState state)
{
  setCurrentState(state);
}

void ControllerStateMachine::setCurrentState(ControllerState state)
{
  const bool changed = (state != mCurrentState);

  mCurrentState = state;
  if(changed){
    emit currentStateChanged();
  }
}

}} // namespace Mdt{ namespace ItemEditor{
