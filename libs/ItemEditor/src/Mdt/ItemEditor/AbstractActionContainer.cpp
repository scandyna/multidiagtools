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
#include "AbstractActionContainer.h"
#include "ControllerStateMachine.h"

namespace Mdt{ namespace ItemEditor{

AbstractActionContainer::AbstractActionContainer(QObject* parent)
 : QObject(parent)
{
}

void AbstractActionContainer::setControllerStateMachine(const ControllerStateMachine*const stateMachine)
{
  Q_ASSERT(stateMachine != nullptr);

  mControllerStateMachine = stateMachine;
  disconnect(mControllerStateChangedConnection);
  mControllerStateChangedConnection = 
    connect(mControllerStateMachine, &ControllerStateMachine::currentStateChanged, this, &AbstractActionContainer::onControllerStateChanged);
  onControllerStateChanged();
}

// void AbstractActionContainer::setControllerStatePermission(const ControllerStatePermission & permission)
// {
// //   mControllerStatePermission = permission;
// }

ControllerState AbstractActionContainer::controllerState() const
{
  if(mControllerStateMachine.isNull()){
    return ControllerState::Visualizing;  /// \todo Fix which state should be returned
  }
  return mControllerStateMachine->currentState();
}

bool AbstractActionContainer::isChangeCurrentRowActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isChangeCurrentRowActionEnabled();
}

bool AbstractActionContainer::isInsertActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isInsertActionEnabled();
}

bool AbstractActionContainer::isSubmitActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isSubmitActionEnabled();
}

bool AbstractActionContainer::isRevertActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isRevertActionEnabled();
}

bool AbstractActionContainer::isRemoveActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isRemoveActionEnabled();
}

bool AbstractActionContainer::isSelectActionEnabled() const
{
  if(mControllerStateMachine.isNull()){
    return false;
  }
  return mControllerStateMachine->isSelectActionEnabled();
}

void AbstractActionContainer::setRowState(RowState rs)
{
  const bool changed = (rs != mRowState);
  mRowState = rs;
  if( changed && !mActionsDisabled ){
    updateEnableState();
  }
}

// void AbstractActionContainer::setControllerState(ControllerState state)
// {
// //   const bool changed = (state != mControllerState);
// // //   mControllerState = state;
// //   if( changed && !mActionsDisabled ){
// //     updateEnableState();
// //   }
// }

void AbstractActionContainer::setActionsDisabled(bool disable)
{
  mActionsDisabled = disable;
  if(disable){
    disableAllActions();
  }else{
    updateEnableState();
  }
}

void AbstractActionContainer::onControllerStateChanged()
{
  Q_ASSERT(!mControllerStateMachine.isNull());

  if(!mActionsDisabled){
    updateEnableState();
  }
}

}} // namespace Mdt{ namespace ItemEditor{
