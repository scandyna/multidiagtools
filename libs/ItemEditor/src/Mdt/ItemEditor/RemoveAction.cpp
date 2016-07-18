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
#include "RemoveAction.h"
#include "ControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

RemoveAction::RemoveAction(QObject* parent)
 : QObject(parent)
{
  pvRemoveAction = new QAction(QIcon::fromTheme("edit-delete"), tr("Remove"), this);
  pvRemoveAction->setObjectName("RemoveAction");
  connect(pvRemoveAction, &QAction::triggered, this, &RemoveAction::removeTriggered);
  updateEnableStates();
}

void RemoveAction::setRowState(RowState rs)
{
  pvRowState = rs;
  updateEnableStates();
}

void RemoveAction::setControllerState(ControllerState state)
{
  pvControllerState = state;
  updateEnableStates();
}

void RemoveAction::updateEnableStates()
{
  if(pvRowState.isNull()){
    disableAllActions();
    return;
  }
  pvRemoveAction->setEnabled( ControllerStatePermission::canRemove(pvControllerState) );
}

void RemoveAction::disableAllActions()
{
  pvRemoveAction->setEnabled(false);
}


}} // namespace Mdt{ namespace ItemEditor{
