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
#include "TableViewControllerStatePermission.h"
#include "AbstractControllerStateTable.h"

namespace Mdt{ namespace ItemEditor{

bool TableViewControllerStatePermission::canInsert(const AbstractControllerStateTable & st) const
{
  if(st.currentState() == ControllerState::Inserting){
    return !hasChildController();
  }
  return AbstractControllerStatePermission::canInsert(st);
}

bool TableViewControllerStatePermission::canChangeCurrentRow(const AbstractControllerStateTable & st) const
{
  if(st.currentState() == ControllerState::Editing){
    return !hasChildController();
  }
  if(st.currentState() == ControllerState::Inserting){
    return !hasChildController();
  }
  return AbstractControllerStatePermission::canChangeCurrentRow(st);
}

bool TableViewControllerStatePermission::isSubmitActionEnabled(const AbstractControllerStateTable & st) const
{
  if(st.currentState() == ControllerState::EditingItem){
    return true;
  }
  return AbstractControllerStatePermission::isSubmitActionEnabled(st);
}

bool TableViewControllerStatePermission::canRevert(const AbstractControllerStateTable& st) const
{
  if(st.currentState() == ControllerState::Editing){
    return modelHasCache();
  }
  return AbstractControllerStatePermission::canRevert(st);
}

bool TableViewControllerStatePermission::isRevertActionEnabled(const AbstractControllerStateTable & st) const
{
  if(st.currentState() == ControllerState::EditingItem){
    return true;
  }
  if(st.currentState() == ControllerState::Editing){
    return modelHasCache();
  }
  return AbstractControllerStatePermission::isRevertActionEnabled(st);
}

}} // namespace Mdt{ namespace ItemEditor{
