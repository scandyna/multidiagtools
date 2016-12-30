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
#include "InsertAction.h"
#include "ControllerStatePermission.h"
#include <QIcon>

namespace Mdt{ namespace ItemEditor{

InsertAction::InsertAction(QObject* parent)
 : AbstractActionContainer(parent)
{
  mAction = new QAction(QIcon::fromTheme("document-new"), tr("Insert"), this);
  mAction->setObjectName("Insert");
  connect(mAction, &QAction::triggered, this, &InsertAction::insertTriggered);
}

void InsertAction::updateEnableState()
{
  mAction->setEnabled( ControllerStatePermission::canInsert(controllerState()) );
}

void InsertAction::disableAllActions()
{
  mAction->setEnabled(false);
}

}} // namespace Mdt{ namespace ItemEditor{
