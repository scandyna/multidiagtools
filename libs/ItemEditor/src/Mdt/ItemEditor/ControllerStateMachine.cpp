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

namespace Mdt{ namespace ItemEditor{

bool ControllerStateMachine::canChangeCurrentRow() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canChangeCurrentRow( currentState() );
}

bool ControllerStateMachine::canInsert() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canInsert( currentState() );
}

bool ControllerStateMachine::canEdit() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canEdit( currentState() );
}

bool ControllerStateMachine::canSubmit() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canSubmit( currentState() );
}

bool ControllerStateMachine::canRevert() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canRevert( currentState() );
}

bool ControllerStateMachine::canRemove() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canRemove( currentState() );
}

bool ControllerStateMachine::canSelect() const
{
  if(!mChainImpl){
    return false;
  }
  return mPermission.canSelect( currentState() );
}

void ControllerStateMachine::onDataEditionStarted()
{

}

ControllerState ControllerStateMachine::currentState() const
{
  Q_ASSERT(mChainImpl);

  return mChainImpl->currentState();
}

}} // namespace Mdt{ namespace ItemEditor{
