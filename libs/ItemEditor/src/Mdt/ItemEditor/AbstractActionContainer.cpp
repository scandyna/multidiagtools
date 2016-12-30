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
#include "AbstractActionContainer.h"

namespace Mdt{ namespace ItemEditor{

AbstractActionContainer::AbstractActionContainer(QObject* parent)
 : QObject(parent)
{
}

void AbstractActionContainer::setRowState(RowState rs)
{
  const bool changed = (rs != mRowState);
  mRowState = rs;
  if( changed && !mActionsDisabled ){
    updateEnableState();
  }
}

void AbstractActionContainer::setControllerState(ControllerState state)
{
  const bool changed = (state != mControllerState);
  mControllerState = state;
  if( changed && !mActionsDisabled ){
    updateEnableState();
  }
}

void AbstractActionContainer::setActionsDisabled(bool disable)
{
  mActionsDisabled = disable;
  if(disable){
    disableAllActions();
  }else{
    updateEnableState();
  }
}

}} // namespace Mdt{ namespace ItemEditor{
