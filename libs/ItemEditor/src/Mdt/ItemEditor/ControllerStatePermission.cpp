/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

bool ControllerStatePermission::canChangeCurrentRow(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      break;
  }
  return false;
}

bool ControllerStatePermission::canInsert(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      break;
  }
  return false;
}

bool ControllerStatePermission::canSubmit(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return false;
    case ControllerState::Editing:
      return true;
  }
  return false;
}

bool ControllerStatePermission::canRevert(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return false;
    case ControllerState::Editing:
      return true;
  }
  return false;
}

bool ControllerStatePermission::canRemove(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      break;
  }
  return false;
}

bool ControllerStatePermission::canSelect(ControllerState state)
{
  switch(state){
    case ControllerState::Visualizing:
      return true;
    case ControllerState::Editing:
      break;
  }
  return false;
}

}} // namespace Mdt{ namespace ItemEditor{
