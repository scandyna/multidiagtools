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
#include "AbstractControllerStateTable.h"
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace ItemEditor{

void AbstractControllerStateTable::forceCurrentState(ControllerState state)
{
  mCurrentState = state;
}

bool AbstractControllerStateTable::canHandleEvent(ControllerEvent event) const noexcept
{
  return ( findTransitionFromCurrentState(event) != mTable.cend() );
}

void AbstractControllerStateTable::setEvent(ControllerEvent event) noexcept
{
//   const auto pred = [event, this](const ControllerStateTableRow & row){
//     return ( (row.event() == event) && (row.state() == mCurrentState) );
//   };
//   const auto it = std::find_if(mTable.cbegin(), mTable.cend(), pred);
  const auto it = findTransitionFromCurrentState(event);
  if(it != mTable.cend()){
    mCurrentState = it->targetState();
  }
}

void AbstractControllerStateTable::createTable()
{
  addTransition(ControllerState::Visualizing, ControllerEvent::DataEditionStarted, ControllerState::Editing);
  addTransition(ControllerState::Visualizing, ControllerEvent::InsertStarted, ControllerState::Inserting);
  addTransition(ControllerState::Editing, ControllerEvent::DataEditionDone, ControllerState::Visualizing);
  addTransition(ControllerState::Editing, ControllerEvent::SubmitDone, ControllerState::Visualizing);
  addTransition(ControllerState::Editing, ControllerEvent::RevertDone, ControllerState::Visualizing);
  addTransition(ControllerState::Inserting, ControllerEvent::SubmitDone, ControllerState::Visualizing);
  addTransition(ControllerState::Inserting, ControllerEvent::RemoveDone, ControllerState::Visualizing);
  
  mCurrentState = ControllerState::Visualizing;
}

void AbstractControllerStateTable::addTransition(ControllerState state, ControllerEvent event, ControllerState targetState)
{
  mTable.emplace_back(state, event, targetState);
}

AbstractControllerStateTable::const_iterator AbstractControllerStateTable::findTransitionFromCurrentState(ControllerEvent event) const
{
  const auto pred = [event, this](const ControllerStateTableRow & row){
    return ( (row.event() == event) && (row.state() == mCurrentState) );
  };
  return std::find_if(mTable.cbegin(), mTable.cend(), pred);
}

}} // namespace Mdt{ namespace ItemEditor{
