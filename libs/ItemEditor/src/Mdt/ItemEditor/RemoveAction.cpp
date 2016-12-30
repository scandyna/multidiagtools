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
 : AbstractActionContainer(parent)
{
  mRemoveAction = new QAction(QIcon::fromTheme("edit-delete"), tr("Remove"), this);
  mRemoveAction->setObjectName("RemoveAction");
  connect(mRemoveAction, &QAction::triggered, this, &RemoveAction::removeTriggered);
  mRemoveAction->setEnabled(false);
}

void RemoveAction::updateEnableState()
{
  if(rowStateIsNull()){
    disableAllActions();
    return;
  }
  mRemoveAction->setEnabled( ControllerStatePermission::canRemove(controllerState()) );
}

void RemoveAction::disableAllActions()
{
  mRemoveAction->setEnabled(false);
}

}} // namespace Mdt{ namespace ItemEditor{
