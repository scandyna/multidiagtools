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

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

ControllerStatePermission::ControllerStatePermission()
{
  qDebug() << "+ ControllerStatePermission: " << this;
}

ControllerStatePermission::~ControllerStatePermission()
{
  qDebug() << "- ControllerStatePermission: " << this;
}

ControllerStatePermission::ControllerStatePermission(const ControllerStatePermission & other)
 : mImpl(other.mImpl)
{
  qDebug() << "+ CPY ControllerStatePermission: " << this;
}

ControllerStatePermission & ControllerStatePermission::operator=(const ControllerStatePermission& other)
{
  qDebug() << "= & ControllerStatePermission: " << this;
  mImpl = other.mImpl;
  return *this;
}

ControllerStatePermission::ControllerStatePermission(ControllerStatePermission&& other)
 : mImpl(other.mImpl)
{
  qDebug() << "+ MOVE ControllerStatePermission: " << this;
}

ControllerStatePermission& ControllerStatePermission::operator=(ControllerStatePermission&& other)
{
  qDebug() << "= && ControllerStatePermission: " << this;
  mImpl = other.mImpl;
  return *this;
}

bool ControllerStatePermission::canChangeCurrentRow(ControllerState state) const
{
  if(mImpl){
    return mImpl->canChangeCurrentRow(state);
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

bool ControllerStatePermission::canSubmit(ControllerState state) const
{
  if(mImpl){
    return mImpl->canSubmit(state);
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

bool ControllerStatePermission::canRemove(ControllerState state) const
{
  if(mImpl){
    return mImpl->canRemove(state);
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

}} // namespace Mdt{ namespace ItemEditor{
