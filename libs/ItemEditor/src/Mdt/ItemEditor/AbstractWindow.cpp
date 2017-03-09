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
#include "AbstractWindow.h"
#include "AbstractEditorWidget.h"
#include "AbstractController.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractWindow::AbstractWindow(QWidget* parent)
 : QMainWindow(parent),
   mNavigationActions(nullptr),
   mInsertAction(nullptr),
   mEditionActions(nullptr),
   mRemoveAction(nullptr)
{
}

// void AbstractWindow::setMainEditorWidget(AbstractEditorWidget* widget)
// {
//   Q_ASSERT(widget != nullptr);
// 
//   auto *controller = widget->controller();
//   Q_ASSERT(controller != nullptr);
//   auto *oldController = mController.data();
//   mController = controller;
// 
//   // Connect navigation actions
//   if(pvNavigationActions != nullptr){
//     if(oldController != nullptr){
//       disconnect(pvNavigationActions, &NavigationActions::toFirstTriggered, oldController, &AbstractController::toFirst);
//       disconnect(pvNavigationActions, &NavigationActions::toPreviousTriggered, oldController, &AbstractController::toPrevious);
//       disconnect(pvNavigationActions, &NavigationActions::toNextTriggered, oldController, &AbstractController::toNext);
//       disconnect(pvNavigationActions, &NavigationActions::toLastTriggered, oldController, &AbstractController::toLast);
//       disconnect(oldController, &AbstractController::rowStateChanged, pvNavigationActions, &NavigationActions::setRowState);
//       disconnect(oldController, &AbstractController::controllerStateChanged, pvNavigationActions, &NavigationActions::setControllerState);
//     }
//     connect(pvNavigationActions, &NavigationActions::toFirstTriggered, controller, &AbstractController::toFirst);
//     connect(pvNavigationActions, &NavigationActions::toPreviousTriggered, controller, &AbstractController::toPrevious);
//     connect(pvNavigationActions, &NavigationActions::toNextTriggered, controller, &AbstractController::toNext);
//     connect(pvNavigationActions, &NavigationActions::toLastTriggered, controller, &AbstractController::toLast);
//     connect(controller, &AbstractController::rowStateChanged, pvNavigationActions, &NavigationActions::setRowState);
//     connect(controller, &AbstractController::controllerStateChanged, pvNavigationActions, &NavigationActions::setControllerState);
//   }
//   // Connect edition actions
//   if(pvEditionActions != nullptr){
//     if(oldController != nullptr){
//       disconnect(pvEditionActions, &EditionActions::submitTriggered, oldController, &AbstractController::submit);
//       disconnect(pvEditionActions, &EditionActions::revertTriggered, oldController, &AbstractController::revert);
//       disconnect(oldController, &AbstractController::rowStateChanged, pvEditionActions, &EditionActions::setRowState);
//       disconnect(oldController, &AbstractController::controllerStateChanged, pvEditionActions, &EditionActions::setControllerState);
//     }
//     connect(pvEditionActions, &EditionActions::submitTriggered, controller, &AbstractController::submit);
//     connect(pvEditionActions, &EditionActions::revertTriggered, controller, &AbstractController::revert);
//     connect(controller, &AbstractController::rowStateChanged, pvEditionActions, &EditionActions::setRowState);
//     connect(controller, &AbstractController::controllerStateChanged, pvEditionActions, &EditionActions::setControllerState);
//   }
//   // Connect insert action
//   if(pvInsertAction != nullptr){
//     if(oldController != nullptr){
//       disconnect(pvInsertAction, &InsertAction::insertTriggered, oldController, &AbstractController::insert);
//       disconnect(oldController, &AbstractController::controllerStateChanged, pvInsertAction, &InsertAction::setControllerState);
//     }
//     connect(pvInsertAction, &InsertAction::insertTriggered, controller, &AbstractController::insert);
//     connect(controller, &AbstractController::controllerStateChanged, pvInsertAction, &InsertAction::setControllerState);
//   }
//   // Connect remove actions
//   if(pvRemoveAction != nullptr){
//     if(oldController != nullptr){
//       disconnect(pvRemoveAction, &RemoveAction::removeTriggered, oldController, &AbstractController::remove);
//       disconnect(oldController, &AbstractController::rowStateChanged, pvRemoveAction, &RemoveAction::setRowState);
//       disconnect(oldController, &AbstractController::controllerStateChanged, pvRemoveAction, &RemoveAction::setControllerState);
//     }
//     connect(pvRemoveAction, &RemoveAction::removeTriggered, controller, &AbstractController::remove);
//     connect(controller, &AbstractController::rowStateChanged, pvRemoveAction, &RemoveAction::setRowState);
//     connect(controller, &AbstractController::controllerStateChanged, pvRemoveAction, &RemoveAction::setControllerState);
//   }
// }

void AbstractWindow::setMainController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  auto *oldController = mController.data();
  mController = controller;

  // Connect navigation actions
  if(mNavigationActions != nullptr){
    if(oldController != nullptr){
      disconnect(mNavigationActions, &NavigationActions::toFirstTriggered, oldController, &AbstractController::toFirst);
      disconnect(mNavigationActions, &NavigationActions::toPreviousTriggered, oldController, &AbstractController::toPrevious);
      disconnect(mNavigationActions, &NavigationActions::toNextTriggered, oldController, &AbstractController::toNext);
      disconnect(mNavigationActions, &NavigationActions::toLastTriggered, oldController, &AbstractController::toLast);
      disconnect(oldController, &AbstractController::rowStateChanged, mNavigationActions, &NavigationActions::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, mNavigationActions, &NavigationActions::setControllerState);
    }
    mNavigationActions->setControllerStatePermission(controller->controllerStatePermission());
    connect(mNavigationActions, &NavigationActions::toFirstTriggered, controller, &AbstractController::toFirst);
    connect(mNavigationActions, &NavigationActions::toPreviousTriggered, controller, &AbstractController::toPrevious);
    connect(mNavigationActions, &NavigationActions::toNextTriggered, controller, &AbstractController::toNext);
    connect(mNavigationActions, &NavigationActions::toLastTriggered, controller, &AbstractController::toLast);
    connect(controller, &AbstractController::rowStateChanged, mNavigationActions, &NavigationActions::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, mNavigationActions, &NavigationActions::setControllerState);
    mNavigationActions->setControllerState(controller->controllerState());
    mNavigationActions->setRowState(controller->rowState());
  }
  // Connect edition actions
  if(mEditionActions != nullptr){
    if(oldController != nullptr){
      disconnect(mEditionActions, &EditionActions::submitTriggered, oldController, &AbstractController::submit);
      disconnect(mEditionActions, &EditionActions::revertTriggered, oldController, &AbstractController::revert);
      disconnect(oldController, &AbstractController::rowStateChanged, mEditionActions, &EditionActions::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, mEditionActions, &EditionActions::setControllerState);
    }
    mEditionActions->setControllerStatePermission(controller->controllerStatePermission());
    connect(mEditionActions, &EditionActions::submitTriggered, controller, &AbstractController::submit);
    connect(mEditionActions, &EditionActions::revertTriggered, controller, &AbstractController::revert);
    connect(controller, &AbstractController::rowStateChanged, mEditionActions, &EditionActions::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, mEditionActions, &EditionActions::setControllerState);
    mEditionActions->setControllerState(controller->controllerState());
    mEditionActions->setRowState(controller->rowState());
  }
  // Connect insert action
  if(mInsertAction != nullptr){
    if(oldController != nullptr){
      disconnect(mInsertAction, &InsertAction::insertTriggered, oldController, &AbstractController::insert);
      disconnect(oldController, &AbstractController::controllerStateChanged, mInsertAction, &InsertAction::setControllerState);
    }
    mInsertAction->setControllerStatePermission(controller->controllerStatePermission());
    connect(mInsertAction, &InsertAction::insertTriggered, controller, &AbstractController::insert);
    connect(controller, &AbstractController::controllerStateChanged, mInsertAction, &InsertAction::setControllerState);
    mInsertAction->setControllerState(controller->controllerState());
    mInsertAction->setRowState(controller->rowState());
  }
  // Connect remove actions
  if(mRemoveAction != nullptr){
    if(oldController != nullptr){
      disconnect(mRemoveAction, &RemoveAction::removeTriggered, oldController, &AbstractController::remove);
      disconnect(oldController, &AbstractController::rowStateChanged, mRemoveAction, &RemoveAction::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, mRemoveAction, &RemoveAction::setControllerState);
    }
    mRemoveAction->setControllerStatePermission(controller->controllerStatePermission());
    connect(mRemoveAction, &RemoveAction::removeTriggered, controller, &AbstractController::remove);
    connect(controller, &AbstractController::rowStateChanged, mRemoveAction, &RemoveAction::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, mRemoveAction, &RemoveAction::setControllerState);
    mRemoveAction->setControllerState(controller->controllerState());
    mRemoveAction->setRowState(controller->rowState());
  }
}

NavigationActions* AbstractWindow::navigationActions()
{
  if(mNavigationActions == nullptr){
    mNavigationActions = new NavigationActions(this);
  }
  return mNavigationActions;
}

EditionActions* AbstractWindow::editionActions()
{
  if(mEditionActions == nullptr){
    mEditionActions = new EditionActions(this);
  }
  return mEditionActions;
}

InsertAction* AbstractWindow::insertAction()
{
  if(mInsertAction == nullptr){
    mInsertAction = new InsertAction(this);
  }
  return mInsertAction;
}

RemoveAction* AbstractWindow::removeAction()
{
  if(mRemoveAction == nullptr){
    mRemoveAction = new RemoveAction(this);
  }
  return mRemoveAction;
}

}} // namespace Mdt{ namespace ItemEditor{
