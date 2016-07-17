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
#include "EditionActions.h"
#include "ControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

EditionActions::EditionActions(QObject* parent)
 : QObject(parent)
{
  pvSubmitAction = new QAction(QIcon::fromTheme("document-save"), tr("Submit"), this);
  pvRevertAction = new QAction(QIcon::fromTheme("document-revert"), tr("Revert"), this);
  pvSubmitAction->setObjectName("SubmitAction");
  pvRevertAction->setObjectName("RevertAction");
  connect(pvSubmitAction, &QAction::triggered, this, &EditionActions::submitTriggered);
  connect(pvRevertAction, &QAction::triggered, this, &EditionActions::revertTriggered);
  updateEnableStates();
}

void EditionActions::setRowState(RowState rs)
{
  pvRowState = rs;
  updateEnableStates();
}

void EditionActions::setControllerState(ControllerState state)
{
  pvControllerState = state;
  updateEnableStates();
}

void EditionActions::updateEnableStates()
{
  if(pvRowState.isNull()){
    disableAllActions();
    return;
  }
  pvSubmitAction->setEnabled( ControllerStatePermission::canSubmit(pvControllerState) );
  pvRevertAction->setEnabled( ControllerStatePermission::canRevert(pvControllerState) );
}

void EditionActions::disableAllActions()
{
  pvSubmitAction->setEnabled(false);
  pvRevertAction->setEnabled(false);
}

}} // namespace Mdt{ namespace ItemEditor{
