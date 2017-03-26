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
#include "AbstractControllerStateChain.h"

namespace Mdt{ namespace ItemEditor{

// AbstractControllerStateChain::AbstractControllerStateChain(QObject* parent)
// {
// }

ControllerState AbstractControllerStateChain::dataEditionStartedState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Visualizing:
      return ControllerState::Editing;
    case ControllerState::Editing:
    case ControllerState::Inserting:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::dataEditionDoneState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Editing:
      return ControllerState::Visualizing;
    case ControllerState::Visualizing:
    case ControllerState::Inserting:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::submitDoneState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Editing:
    case ControllerState::Inserting:
      return ControllerState::Visualizing;
    case ControllerState::Visualizing:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::revertDoneState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Editing:
      return ControllerState::Visualizing;
    case ControllerState::Inserting:
    case ControllerState::Visualizing:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::insertStartedState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Visualizing:
      return ControllerState::Inserting;
    case ControllerState::Editing:
    case ControllerState::Inserting:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::removeDoneState(ControllerState currentState) const
{
  switch(currentState){
    case ControllerState::Editing:
    case ControllerState::Inserting:
      return ControllerState::Visualizing;
    case ControllerState::Visualizing:
    case ControllerState::ParentEditing:
    case ControllerState::ChildEditing:
      break;
  }
  return currentState;
}

ControllerState AbstractControllerStateChain::dataEditionStartedFromParentState(ControllerState currentState) const
{

}

}} // namespace Mdt{ namespace ItemEditor{
