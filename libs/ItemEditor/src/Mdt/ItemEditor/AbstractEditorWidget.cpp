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
#include "AbstractEditorWidget.h"
#include "AbstractEditorLayout.h"
#include "AbstractActionContainer.h"
#include "InsertAction.h"
#include "RemoveAction.h"
#include "EditionActions.h"
#include "NavigationActions.h"
#include <QToolBar>
#include <QAction>

namespace Mdt{ namespace ItemEditor{

AbstractEditorWidget::AbstractEditorWidget(QWidget* parent)
 : QWidget(parent),
   mMainLayout(new AbstractEditorLayout)
{
  setLayout(mMainLayout);
}

void AbstractEditorWidget::setCentralWidget(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  mMainLayout->setCentralWidget(widget);
}

QToolBar* AbstractEditorWidget::addToolBarToTopArea()
{
  return mMainLayout->addToolBarToTopArea();
}

QToolBar* AbstractEditorWidget::addToolBarToBottomArea()
{
  return mMainLayout->addToolBarToBottomArea();
}

// void AbstractEditorWidget::addActionToTopBar(QAction* action, AbstractEditorWidget::AddSeparatorInToolBar addSeparator)
// {
//   Q_ASSERT(action != nullptr);
// 
//   
// }

void AbstractEditorWidget::addWidgetToTopArea(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  createTopCustomWidgetsBarIfNot();
  mTopCustomWidgetsBar->addWidget(widget);
}

void AbstractEditorWidget::addInsertActionToTopArea()
{
  createInsertActionIfNot();
  createTopEditBarIfNot();
  mTopEditBar->addAction(mInsertAction->insertAction());
}

void AbstractEditorWidget::addInsertActionToBottomArea()
{
  createInsertActionIfNot();
  createBottomEditBarIfNot();
  mBottomEditBar->addAction(mInsertAction->insertAction());
}

void AbstractEditorWidget::setInsertActionText(const QString& text)
{
  createInsertActionIfNot();
  mInsertAction->insertAction()->setText(text);
}

void AbstractEditorWidget::addRemoveActionToTopArea()
{
  createRemoveActionIfNot();
  createTopEditBarIfNot();
  mTopEditBar->addAction(mRemoveAction->removeAction());
}

void AbstractEditorWidget::addRemoveActionToBottomArea()
{
  createRemoveActionIfNot();
  createBottomEditBarIfNot();
  mBottomEditBar->addAction(mRemoveAction->removeAction());
}

void AbstractEditorWidget::setRemoveActionText(const QString& text)
{
  createRemoveActionIfNot();
  mRemoveAction->removeAction()->setText(text);
}

void AbstractEditorWidget::addEditionActionsToTopArea()
{
  createEditionActionsIfNot();
  createTopEditBarIfNot();
  mTopEditBar->addAction(mEditionActions->submitAction());
  mTopEditBar->addAction(mEditionActions->revertAction());
}

void AbstractEditorWidget::addEditionActionsToBottomArea()
{
  createEditionActionsIfNot();
  createBottomEditBarIfNot();
  mBottomEditBar->addAction(mEditionActions->submitAction());
  mBottomEditBar->addAction(mEditionActions->revertAction());
}

void AbstractEditorWidget::addNavigationActionsToTopArea()
{
  createNavigationActionsIfNot();
  createTopNavigationBarIfNot();
  mTopNavigationBar->addAction(mNavigationActions->toFirst());
  mTopNavigationBar->addAction(mNavigationActions->toPrevious());
  mTopNavigationBar->addAction(mNavigationActions->toNext());
  mTopNavigationBar->addAction(mNavigationActions->toLast());
}

void AbstractEditorWidget::addNavigationActionsToBottomArea()
{
  createNavigationActionsIfNot();
  createBottomNavigationBarIfNot();
  mBottomNavigationBar->addAction(mNavigationActions->toFirst());
  mBottomNavigationBar->addAction(mNavigationActions->toPrevious());
  mBottomNavigationBar->addAction(mNavigationActions->toNext());
  mBottomNavigationBar->addAction(mNavigationActions->toLast());
}

void AbstractEditorWidget::registerActions(AbstractActionContainer* actions)
{
  Q_ASSERT(actions != nullptr);

  auto *controller = abstractController();
  Q_ASSERT(controller != nullptr);

  connect(controller, &AbstractController::rowStateChanged, actions, &AbstractActionContainer::setRowState);
  actions->setRowState(controller->rowState());
//   connect(controller, &AbstractController::controllerStateChanged, actions, &AbstractActionContainer::setControllerState);
}

void AbstractEditorWidget::prependProxyModel(QAbstractProxyModel* proxyModel)
{
  Q_ASSERT(abstractController() != nullptr);

  abstractController()->prependProxyModel(proxyModel);
}

void AbstractEditorWidget::appendProxyModel(QAbstractProxyModel* proxyModel)
{
  Q_ASSERT(abstractController() != nullptr);

  abstractController()->appendProxyModel(proxyModel);
}

// void AbstractEditorWidget::setController(AbstractController* controller)
// {
//   Q_ASSERT(controller != nullptr);
// 
//   pvController = controller;
// }

void AbstractEditorWidget::createTopEditBarIfNot()
{
  if(mTopEditBar == nullptr){
    mTopEditBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopEditBar != nullptr);
  mTopEditBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void AbstractEditorWidget::createBottomEditBarIfNot()
{
  if(mBottomEditBar == nullptr){
    mBottomEditBar = addToolBarToBottomArea();
  }
  Q_ASSERT(mBottomEditBar != nullptr);
  mBottomEditBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void AbstractEditorWidget::createTopNavigationBarIfNot()
{
  if(mTopNavigationBar == nullptr){
    mTopNavigationBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopNavigationBar != nullptr);
}

void AbstractEditorWidget::createBottomNavigationBarIfNot()
{
  if(mBottomNavigationBar == nullptr){
    mBottomNavigationBar = addToolBarToBottomArea();
  }
  Q_ASSERT(mBottomNavigationBar != nullptr);
}

void AbstractEditorWidget::createTopCustomWidgetsBarIfNot()
{
  if(mTopCustomWidgetsBar == nullptr){
    mTopCustomWidgetsBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopCustomWidgetsBar != nullptr);
}

void AbstractEditorWidget::createInsertActionIfNot()
{
  if(mInsertAction == nullptr){
    mInsertAction = new InsertAction(this);
    Q_ASSERT(abstractController() != nullptr);
//     mInsertAction->setControllerStatePermission(abstractController()->controllerStatePermission());
    mInsertAction->setControllerStateMachine(abstractController()->controllerStateMachine());
    connect(mInsertAction, &InsertAction::insertTriggered, abstractController(), &AbstractController::insert);
    registerActions(mInsertAction);
  }
  Q_ASSERT(mInsertAction != nullptr);
}

void AbstractEditorWidget::createRemoveActionIfNot()
{
  if(mRemoveAction == nullptr){
    mRemoveAction = new RemoveAction(this);
    Q_ASSERT(abstractController() != nullptr);
//     mRemoveAction->setControllerStatePermission(abstractController()->controllerStatePermission());
    mRemoveAction->setControllerStateMachine(abstractController()->controllerStateMachine());
    connect(mRemoveAction, &RemoveAction::removeTriggered, abstractController(), &AbstractController::remove);
    registerActions(mRemoveAction);
  }
  Q_ASSERT(mRemoveAction != nullptr);
}

void AbstractEditorWidget::createEditionActionsIfNot()
{
  if(mEditionActions == nullptr){
    mEditionActions = new EditionActions(this);
    Q_ASSERT(abstractController() != nullptr);
//     mEditionActions->setControllerStatePermission(abstractController()->controllerStatePermission());
    mEditionActions->setControllerStateMachine(abstractController()->controllerStateMachine());
    connect(mEditionActions, &EditionActions::submitTriggered, abstractController(), &AbstractController::submit);
    connect(mEditionActions, &EditionActions::revertTriggered, abstractController(), &AbstractController::revert);
    registerActions(mEditionActions);
  }
  Q_ASSERT(mEditionActions != nullptr);
}

void AbstractEditorWidget::createNavigationActionsIfNot()
{
  if(mNavigationActions == nullptr){
    mNavigationActions = new NavigationActions(this);
    Q_ASSERT(abstractController() != nullptr);
//     mNavigationActions->setControllerStatePermission(abstractController()->controllerStatePermission());
    mNavigationActions->setControllerStateMachine(abstractController()->controllerStateMachine());
    connect(mNavigationActions, &NavigationActions::toFirstTriggered, abstractController(), &AbstractController::toFirst);
    connect(mNavigationActions, &NavigationActions::toPreviousTriggered, abstractController(), &AbstractController::toPrevious);
    connect(mNavigationActions, &NavigationActions::toNextTriggered, abstractController(), &AbstractController::toNext);
    connect(mNavigationActions, &NavigationActions::toLastTriggered, abstractController(), &AbstractController::toLast);
    registerActions(mNavigationActions);
  }
  Q_ASSERT(mNavigationActions != nullptr);
}

}} // namespace Mdt{ namespace ItemEditor{
