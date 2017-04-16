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
#include "ControllerEvent.h"
#include <QByteArray>

namespace Mdt{ namespace ItemEditor{

QByteArray controllerEventText(ControllerEvent event)
{
  switch(event){
    case ControllerEvent::DataEditionStarted:
      return QByteArray("DataEditionStarted");
    case ControllerEvent::DataEditionDone:
      return QByteArray("DataEditionDone");
    case ControllerEvent::SubmitDone:
      return QByteArray("SubmitDone");
    case ControllerEvent::RevertDone:
      return QByteArray("RevertDone");
    case ControllerEvent::InsertStarted:
      return QByteArray("InsertStarted");
    case ControllerEvent::RemoveDone:
      return QByteArray("RemoveDone");
    case ControllerEvent::EditionStartedFromParent:
      return QByteArray("EditionStartedFromParent");
    case ControllerEvent::EditionDoneFromParent:
      return QByteArray("EditionDoneFromParent");
    case ControllerEvent::EditionStartedFromChild:
      return QByteArray("EditionStartedFromChild");
    case ControllerEvent::EditionDoneFromChild:
      return QByteArray("EditionDoneFromChild");
    case ControllerEvent::DisableController:
      return QByteArray("DisableController");
    case ControllerEvent::EnableController:
      return QByteArray("EnableController");
  }
  return QByteArray();
}

}} // namespace Mdt{ namespace ItemEditor{
