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

namespace Mdt{ namespace ItemEditor{

bool AbstractControllerStatePermission::canChangeCurrentRow(ControllerState state) const
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      return false;
    case ControllerState::Inserting:
      return false;
    case ControllerState::ChildEditing:
      return false;
    case ControllerState::ParentEditing:
      return true;
  }
  return false;
}

bool AbstractControllerStatePermission::isChangeCurrentRowActionEnabled(ControllerState state) const
{
  return canChangeCurrentRow(state);
}

bool AbstractControllerStatePermission::canInsert(ControllerState state) const
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      return false;
    case ControllerState::Inserting:
      return true;
    case ControllerState::ChildEditing:
      return false;
    case ControllerState::ParentEditing:
      return true;
  }
  return false;
}

bool AbstractControllerStatePermission::isInsertActionEnabled(ControllerState state) const
{
  return canInsert(state);
}

bool AbstractControllerStatePermission::canSubmit(ControllerState state) const
{
  switch(state){
    case ControllerState::Visualizing:
      return false;
    case ControllerState::Editing:
      return true;
    case ControllerState::Inserting:
      return true;
    case ControllerState::ChildEditing:
      return true;
    case ControllerState::ParentEditing:
      return true;
  }
  return false;
}

bool AbstractControllerStatePermission::isSubmitActionEnabled(ControllerState state) const
{
  return canSubmit(state);
}

bool AbstractControllerStatePermission::canRevert(ControllerState state) const
{
  switch(state){
    case ControllerState::Visualizing:
      return false;
    case ControllerState::Editing:
      return true;
    case ControllerState::Inserting:
      return false;
    case ControllerState::ChildEditing:
      return true;
    case ControllerState::ParentEditing:
      return false;
  }
  return false;
}

bool AbstractControllerStatePermission::isRevertActionEnabled(ControllerState state) const
{
  return canRevert(state);
}

bool AbstractControllerStatePermission::canRemove(ControllerState state) const
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      return false;
    case ControllerState::Inserting:
      return true;
    case ControllerState::ChildEditing:
      return false;
    case ControllerState::ParentEditing:
      return true;
  }
  return false;
}

bool AbstractControllerStatePermission::isRemoveActionEnabled(ControllerState state) const
{
  return canRemove(state);
}

bool AbstractControllerStatePermission::canSelect(ControllerState state) const
{
  switch(state){
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

bool AbstractControllerStatePermission::isSelectActionEnabled(ControllerState state) const
{
  return canSelect(state);
}

}} // namespace Mdt{ namespace ItemEditor{
