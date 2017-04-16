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
#include "AbstractControllerRelation.h"
#include "AbstractController.h"
#include "ControllerStateMachine.h"
#include "ControllerEvent.h"

// #include "Debug.h"

namespace Mdt{ namespace ItemEditor{

AbstractControllerRelation::AbstractControllerRelation(QObject* parent)
 : QObject(parent)
{
}

void AbstractControllerRelation::setParentController(AbstractController* controller)
{
  if(controller == mParentController){
    return;
  }

  if(!mParentController.isNull()){
    parentControllerAboutToChangeEvent(mParentController);
  }

  mParentController = controller;
  Q_ASSERT(!mParentController.isNull());

  if(mParentController->modelForView() != nullptr){
    parentControllerModelChangedEvent(mParentController->modelForView());
  }
  disconnect(mParentControllerModelChangedConnection);
  mParentControllerModelChangedConnection = 
    connect( mParentController, &AbstractController::modelForViewChanged, this, &AbstractControllerRelation::parentControllerModelChangedEvent );

  Q_ASSERT(mParentController->controllerStateMachine() != nullptr);
  disconnect(mParentControllerEventCompletedConnection);
  mParentControllerEventCompletedConnection =
    connect( mParentController->controllerStateMachine(), &ControllerStateMachine::eventCompleted, this, &AbstractControllerRelation::onParentControllerEventCompleted );

  parentControllerChangedEvent(mParentController);
}

void AbstractControllerRelation::setChildController(AbstractController* controller)
{
  if(controller == mChildController){
    return;
  }

  if(!mChildController.isNull()){
    childControllerAboutToChangeEvent(mChildController);
  }

  mChildController = controller;
  Q_ASSERT(!mChildController.isNull());

  if(mChildController->modelForView() != nullptr){
    childControllerModelChangedEvent(mChildController->modelForView());
  }
  disconnect(mChildControllerModelChangedConnection);
  mChildControllerModelChangedConnection = 
    connect( mChildController, &AbstractController::modelForViewChanged, this, &AbstractControllerRelation::childControllerModelChangedEvent );

  Q_ASSERT(mChildController->controllerStateMachine() != nullptr);
  disconnect(mChildControllerEventCompletedConnection);
  mChildControllerEventCompletedConnection =
    connect( mChildController->controllerStateMachine(), &ControllerStateMachine::eventCompleted, this, &AbstractControllerRelation::onChildControllerEventCompleted );

  childControllerChangedEvent(mChildController);
}

AbstractController* AbstractControllerRelation::parentController() const
{
  return mParentController;
}

AbstractController* AbstractControllerRelation::childController() const
{
  return mChildController;
}

QAbstractItemModel* AbstractControllerRelation::parentControllerModel() const
{
  return mParentController->modelForView();
}

QAbstractItemModel* AbstractControllerRelation::childControllerModel() const
{
  if(mChildController.isNull()){
    return nullptr;
  }
  return mChildController->modelForView();
}

void AbstractControllerRelation::parentControllerAboutToChangeEvent(AbstractController*)
{
}

void AbstractControllerRelation::parentControllerChangedEvent(AbstractController*)
{
}

void AbstractControllerRelation::childControllerAboutToChangeEvent(AbstractController*)
{
}

void AbstractControllerRelation::childControllerChangedEvent(AbstractController*)
{
}

void AbstractControllerRelation::parentControllerModelChangedEvent(QAbstractItemModel*)
{
}

void AbstractControllerRelation::childControllerModelChangedEvent(QAbstractItemModel*)
{
}

void AbstractControllerRelation::parentControllerStateChangedEvent(ControllerState)
{
}

void AbstractControllerRelation::childControllerStateChangedEvent(ControllerState)
{
}

void AbstractControllerRelation::onParentControllerEventCompleted(ControllerEvent event)
{
  Q_ASSERT(!mChildController.isNull());
  Q_ASSERT(mChildController->controllerStateMachine() != nullptr);

  switch(event){
    case ControllerEvent::DataEditionStarted:
    case ControllerEvent::EditionStartedFromParent:
    case ControllerEvent::InsertStarted:
      mChildController->controllerStateMachine()->setEvent(ControllerEvent::EditionStartedFromParent);
      break;
    case ControllerEvent::SubmitDone:
    case ControllerEvent::RevertDone:
    case ControllerEvent::EditionDoneFromParent:
      mChildController->controllerStateMachine()->setEvent(ControllerEvent::EditionDoneFromParent);
      break;
    case ControllerEvent::DataEditionDone:
    case ControllerEvent::RemoveDone:
    case ControllerEvent::EditionStartedFromChild:
    case ControllerEvent::EditionDoneFromChild:
    case ControllerEvent::DisableController:
    case ControllerEvent::EnableController:
      break;
  }
}

void AbstractControllerRelation::onChildControllerEventCompleted(ControllerEvent event)
{
  Q_ASSERT(!mParentController.isNull());
  Q_ASSERT(mParentController->controllerStateMachine() != nullptr);

  switch(event){
    case ControllerEvent::DataEditionStarted:
    case ControllerEvent::EditionStartedFromChild:
    case ControllerEvent::InsertStarted:
      mParentController->controllerStateMachine()->setEvent(ControllerEvent::EditionStartedFromChild);
      break;
    case ControllerEvent::SubmitDone:
    case ControllerEvent::RevertDone:
    case ControllerEvent::EditionDoneFromChild:
      mParentController->controllerStateMachine()->setEvent(ControllerEvent::EditionDoneFromChild);
      break;
    case ControllerEvent::DataEditionDone:
    case ControllerEvent::RemoveDone:
    case ControllerEvent::EditionStartedFromParent:
    case ControllerEvent::EditionDoneFromParent:
    case ControllerEvent::DisableController:
    case ControllerEvent::EnableController:
      break;
  }
}

}} // namespace Mdt{ namespace ItemEditor{
