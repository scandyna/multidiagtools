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
   pvNavigationActions(nullptr),
   pvInsertAction(nullptr),
   pvEditionActions(nullptr),
   pvRemoveAction(nullptr)
{
}

void AbstractWindow::setMainEditorWidget(AbstractEditorWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  auto *controller = widget->controller();
  Q_ASSERT(controller != nullptr);
  auto *oldController = pvController.data();
  pvController = controller;

  // Connect navigation actions
  if(pvNavigationActions != nullptr){
    if(oldController != nullptr){
      disconnect(pvNavigationActions, &NavigationActions::toFirstTriggered, oldController, &AbstractController::toFirst);
      disconnect(pvNavigationActions, &NavigationActions::toPreviousTriggered, oldController, &AbstractController::toPrevious);
      disconnect(pvNavigationActions, &NavigationActions::toNextTriggered, oldController, &AbstractController::toNext);
      disconnect(pvNavigationActions, &NavigationActions::toLastTriggered, oldController, &AbstractController::toLast);
      disconnect(oldController, &AbstractController::rowStateChanged, pvNavigationActions, &NavigationActions::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, pvNavigationActions, &NavigationActions::setControllerState);
    }
    connect(pvNavigationActions, &NavigationActions::toFirstTriggered, controller, &AbstractController::toFirst);
    connect(pvNavigationActions, &NavigationActions::toPreviousTriggered, controller, &AbstractController::toPrevious);
    connect(pvNavigationActions, &NavigationActions::toNextTriggered, controller, &AbstractController::toNext);
    connect(pvNavigationActions, &NavigationActions::toLastTriggered, controller, &AbstractController::toLast);
    connect(controller, &AbstractController::rowStateChanged, pvNavigationActions, &NavigationActions::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, pvNavigationActions, &NavigationActions::setControllerState);
  }
  // Connect edition actions
  if(pvEditionActions != nullptr){
    if(oldController != nullptr){
      disconnect(pvEditionActions, &EditionActions::submitTriggered, oldController, &AbstractController::submit);
      disconnect(pvEditionActions, &EditionActions::revertTriggered, oldController, &AbstractController::revert);
      disconnect(oldController, &AbstractController::rowStateChanged, pvEditionActions, &EditionActions::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, pvEditionActions, &EditionActions::setControllerState);
    }
    connect(pvEditionActions, &EditionActions::submitTriggered, controller, &AbstractController::submit);
    connect(pvEditionActions, &EditionActions::revertTriggered, controller, &AbstractController::revert);
    connect(controller, &AbstractController::rowStateChanged, pvEditionActions, &EditionActions::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, pvEditionActions, &EditionActions::setControllerState);
  }
  // Connect insert action
  if(pvInsertAction != nullptr){
    if(oldController != nullptr){
      disconnect(pvInsertAction, &InsertAction::insertTriggered, oldController, &AbstractController::insert);
      disconnect(oldController, &AbstractController::controllerStateChanged, pvInsertAction, &InsertAction::setControllerState);
    }
    connect(pvInsertAction, &InsertAction::insertTriggered, controller, &AbstractController::insert);
    connect(controller, &AbstractController::controllerStateChanged, pvInsertAction, &InsertAction::setControllerState);
  }
  // Connect remove actions
  if(pvRemoveAction != nullptr){
    if(oldController != nullptr){
      disconnect(pvRemoveAction, &RemoveAction::removeTriggered, oldController, &AbstractController::remove);
      disconnect(oldController, &AbstractController::rowStateChanged, pvRemoveAction, &RemoveAction::setRowState);
      disconnect(oldController, &AbstractController::controllerStateChanged, pvRemoveAction, &RemoveAction::setControllerState);
    }
    connect(pvRemoveAction, &RemoveAction::removeTriggered, controller, &AbstractController::remove);
    connect(controller, &AbstractController::rowStateChanged, pvRemoveAction, &RemoveAction::setRowState);
    connect(controller, &AbstractController::controllerStateChanged, pvRemoveAction, &RemoveAction::setControllerState);
  }
}

NavigationActions* AbstractWindow::navigationActions()
{
  if(pvNavigationActions == nullptr){
    pvNavigationActions = new NavigationActions(this);
  }
  return pvNavigationActions;
}

EditionActions* AbstractWindow::editionActions()
{
  if(pvEditionActions == nullptr){
    pvEditionActions = new EditionActions(this);
  }
  return pvEditionActions;
}

InsertAction* AbstractWindow::insertAction()
{
  if(pvInsertAction == nullptr){
    pvInsertAction = new InsertAction(this);
  }
  return pvInsertAction;
}

RemoveAction* AbstractWindow::removeAction()
{
  if(pvRemoveAction == nullptr){
    pvRemoveAction = new RemoveAction(this);
  }
  return pvRemoveAction;
}

}} // namespace Mdt{ namespace ItemEditor{
