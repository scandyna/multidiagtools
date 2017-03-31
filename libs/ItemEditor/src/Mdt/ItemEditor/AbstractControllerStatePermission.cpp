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
#include "AbstractControllerStatePermission.h"
#include "AbstractControllerStateTable.h"
#include "ControllerEvent.h"

namespace Mdt{ namespace ItemEditor{

void AbstractControllerStatePermission::setModelHasCache(bool hasCache)
{
  mModelHasCache = hasCache;
}

void AbstractControllerStatePermission::setHasChildController(bool hasChild)
{
  mHasChildController = hasChild;
}

bool AbstractControllerStatePermission::canChangeCurrentRow(const AbstractControllerStateTable & st) const
{
  switch(st.currentState()){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      return false;
    case ControllerState::EditingItem:
      return false;
    case ControllerState::Inserting:
      return false;
//     case ControllerState::EditingNewItem:
//       return false;
    case ControllerState::ChildEditing:
      return false;
    case ControllerState::ParentEditing:
      return true;
  }
  return false;
}

bool AbstractControllerStatePermission::isChangeCurrentRowActionEnabled(const AbstractControllerStateTable & st) const
{
  return canChangeCurrentRow(st);
}

bool AbstractControllerStatePermission::canInsert(const AbstractControllerStateTable & st) const
{
  return st.canHandleEvent(ControllerEvent::InsertStarted);
}

bool AbstractControllerStatePermission::isInsertActionEnabled(const AbstractControllerStateTable & st) const
{
  return canInsert(st);
}

bool AbstractControllerStatePermission::canEdit(const AbstractControllerStateTable & st) const
{
  switch(st.currentState()){
    case ControllerState::Editing:
    case ControllerState::EditingItem:
    case ControllerState::Inserting:
//     case ControllerState::EditingNewItem:
      return true;
    case ControllerState::Visualizing:
    case ControllerState::ChildEditing:
    case ControllerState::ParentEditing:
      break;
  }
  return st.canHandleEvent(ControllerEvent::DataEditionStarted);
}

bool AbstractControllerStatePermission::canSubmit(const AbstractControllerStateTable & st) const
{
  return true;
//   switch(st.currentState()){
//     case ControllerState::Visualizing:
//       return true;
//     case ControllerState::Editing:
//     case ControllerState::Inserting:
//     case ControllerState::ChildEditing:
//     case ControllerState::ParentEditing:
//       break;
//   }
//   return st.canHandleEvent(ControllerEvent::SubmitDone);
}

bool AbstractControllerStatePermission::isSubmitActionEnabled(const AbstractControllerStateTable & st) const
{
  switch(st.currentState()){
    case ControllerState::ParentEditing:
      return false;
    case ControllerState::Visualizing:
      return false;
    case ControllerState::Editing:
    case ControllerState::Inserting:
    case ControllerState::ChildEditing:
      break;
  }
  return canSubmit(st);
}

bool AbstractControllerStatePermission::canRevert(const AbstractControllerStateTable & st) const
{
  return st.canHandleEvent(ControllerEvent::RevertDone);
}

bool AbstractControllerStatePermission::isRevertActionEnabled(const AbstractControllerStateTable& st) const
{
  switch(st.currentState()){
    case ControllerState::ParentEditing:
      return false;
    case ControllerState::Visualizing:
    case ControllerState::Editing:
    case ControllerState::Inserting:
    case ControllerState::ChildEditing:
      break;
  }
  return canRevert(st);
}

bool AbstractControllerStatePermission::canRemove(const AbstractControllerStateTable & st) const
{
  switch(st.currentState()){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
    case ControllerState::Inserting:
    case ControllerState::ChildEditing:
    case ControllerState::ParentEditing:
      break;
  }
  return st.canHandleEvent(ControllerEvent::RemoveDone);
}

bool AbstractControllerStatePermission::isRemoveActionEnabled(const AbstractControllerStateTable & st) const
{
  return canRemove(st);
}

bool AbstractControllerStatePermission::canSelect(const AbstractControllerStateTable & st) const
{
  switch(st.currentState()){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      return false;
    case ControllerState::Inserting:
      return false;
    case ControllerState::ChildEditing:
      return false;
    case ControllerState::ParentEditing:
      return false;
  }
  return false;
}

bool AbstractControllerStatePermission::isSelectActionEnabled(const AbstractControllerStateTable & st) const
{
  return canSelect(st);
}

}} // namespace Mdt{ namespace ItemEditor{
