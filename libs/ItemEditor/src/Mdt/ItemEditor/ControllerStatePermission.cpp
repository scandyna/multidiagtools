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
#include "ControllerStatePermission.h"
#include "AbstractControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

bool ControllerStatePermission::canChangeCurrentRow(ControllerState state) const
{
  if(mImpl){
    return mImpl->canChangeCurrentRow(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isChangeCurrentRowActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isChangeCurrentRowActionEnabled(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::canInsert(ControllerState state) const
{
  if(mImpl){
    return mImpl->canInsert(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isInsertActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isInsertActionEnabled(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::canSubmit(ControllerState state) const
{
  if(mImpl){
    return mImpl->canSubmit(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isSubmitActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isSubmitActionEnabled(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::canRevert(ControllerState state) const
{
  if(mImpl){
    return mImpl->canRevert(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isRevertActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isRevertActionEnabled(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::canRemove(ControllerState state) const
{
  if(mImpl){
    return mImpl->canRemove(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isRemoveActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isRemoveActionEnabled(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::canSelect(ControllerState state) const
{
  if(mImpl){
    return mImpl->canSelect(state);
  }else{
    return false;
  }
}

bool ControllerStatePermission::isSelectActionEnabled(ControllerState state) const
{
  if(mImpl){
    return mImpl->isSelectActionEnabled(state);
  }else{
    return false;
  }
}

}} // namespace Mdt{ namespace ItemEditor{
