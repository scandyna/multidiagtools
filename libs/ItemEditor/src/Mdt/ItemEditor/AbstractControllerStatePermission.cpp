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
  }
  return false;
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
  }
  return false;
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
  }
  return false;
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
  }
  return false;
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
  }
  return false;
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
  }
  return false;
}

}} // namespace Mdt{ namespace ItemEditor{
