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
#include "AbstractTableViewWidget.h"
#include "AbstractController.h"
#include "AbstractActionContainer.h"
#include "TableViewController.h"
#include "InsertAction.h"
#include "RemoveAction.h"
#include "ResizeToContentsAction.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractTableViewWidget::AbstractTableViewWidget(QWidget* parent)
 : QWidget(parent),
   mView(new QTableView),
   mMainLayout(new QVBoxLayout)
{
  Q_ASSERT(mTopBarLayout == nullptr);
  Q_ASSERT(mBottomBarLayout == nullptr);
  Q_ASSERT(mTopToolsBar == nullptr);
  Q_ASSERT(mTopEditBar == nullptr);
  Q_ASSERT(mBottomEditBar == nullptr);
  Q_ASSERT(mInsertAction == nullptr);
  Q_ASSERT(mRemoveAction == nullptr);
  Q_ASSERT(mResizeToContentsAction == nullptr);

  // Layout widgets
  mMainLayout->addWidget(mView);
  setLayout(mMainLayout);
}

QToolBar* AbstractTableViewWidget::addToolBarToTopArea()
{
  auto *bar = new QToolBar;

  createTopBarLayoutIfNot();
  mTopBarLayout->addWidget(bar);

  return bar;
}

QToolBar* AbstractTableViewWidget::addToolBarToBottomArea()
{
  auto *bar = new QToolBar;

  createBottomBarLayoutIfNot();
  mBottomBarLayout->addWidget(bar);

  return bar;
}

void AbstractTableViewWidget::addInsertActionToTopArea()
{
  createInsertActionIfNot();
  createTopEditBarIfNot();
  mTopEditBar->addAction(mInsertAction->insertAction());
}

void AbstractTableViewWidget::addInsertActionToBottomArea()
{
  createInsertActionIfNot();
  createBottomEditBarIfNot();
  mBottomEditBar->addAction(mInsertAction->insertAction());
}

void AbstractTableViewWidget::setInsertActionText(const QString& text)
{
  createInsertActionIfNot();
  mInsertAction->insertAction()->setText(text);
}

void AbstractTableViewWidget::addRemoveActionToBottomBar()
{
  createRemoveActionIfNot();
  createBottomEditBarIfNot();
  mBottomEditBar->addAction(mRemoveAction->removeAction());
}

void AbstractTableViewWidget::setRemoveActionText(const QString& text)
{
  createRemoveActionIfNot();
  mRemoveAction->removeAction()->setText(text);
}

void AbstractTableViewWidget::addResizeToContentsActionToTopBar()
{
  createResizeToContentsActionIfNot();
  createTopToolsBarIfNot();
  mTopToolsBar->addAction(mResizeToContentsAction->resizeToContentsAction());
}

void AbstractTableViewWidget::registerActions(AbstractActionContainer* actions)
{
  Q_ASSERT(actions != nullptr);

  auto *controller = refController();
  Q_ASSERT(controller != nullptr);

  connect(controller, &AbstractController::rowStateChanged, actions, &AbstractActionContainer::setRowState);
  connect(controller, &AbstractController::controllerStateChanged, actions, &AbstractActionContainer::setControllerState);
}

void AbstractTableViewWidget::resizeViewToContents()
{
  Q_ASSERT(mView != nullptr);
  mView->resizeColumnsToContents();
  mView->resizeRowsToContents();
}

void AbstractTableViewWidget::createTopBarLayoutIfNot()
{
  if(mTopBarLayout == nullptr){
    mTopBarLayout = new QHBoxLayout;
    mMainLayout->insertLayout(0, mTopBarLayout);
  }
  Q_ASSERT(mTopBarLayout != nullptr);
}

void AbstractTableViewWidget::createBottomBarLayoutIfNot()
{
  if(mBottomBarLayout == nullptr){
    mBottomBarLayout = new QHBoxLayout;
    mMainLayout->addLayout(mBottomBarLayout);
  }
  Q_ASSERT(mBottomBarLayout != nullptr);
}

void AbstractTableViewWidget::createTopToolsBarIfNot()
{
  if(mTopToolsBar == nullptr){
    mTopToolsBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopToolsBar != nullptr);
  mTopToolsBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void AbstractTableViewWidget::createTopEditBarIfNot()
{
  if(mTopEditBar == nullptr){
    mTopEditBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopEditBar != nullptr);
  mTopEditBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void AbstractTableViewWidget::createBottomEditBarIfNot()
{
  if(mBottomEditBar == nullptr){
    mBottomEditBar = addToolBarToBottomArea();
  }
  Q_ASSERT(mBottomEditBar != nullptr);
  mBottomEditBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void AbstractTableViewWidget::createInsertActionIfNot()
{
  if(mInsertAction == nullptr){
    mInsertAction = new InsertAction(this);
    Q_ASSERT(refController() != nullptr);
    connect(mInsertAction, &InsertAction::insertTriggered, refController(), &AbstractController::insert);
  }
  Q_ASSERT(mInsertAction != nullptr);
  registerActions(mInsertAction);
}

void AbstractTableViewWidget::createRemoveActionIfNot()
{
  if(mRemoveAction == nullptr){
    mRemoveAction = new RemoveAction(this);
    Q_ASSERT(refController() != nullptr);
    connect(mRemoveAction, &RemoveAction::removeTriggered, refController(), &AbstractController::remove);
  }
  Q_ASSERT(mRemoveAction != nullptr);
  registerActions(mRemoveAction);
}

void AbstractTableViewWidget::createResizeToContentsActionIfNot()
{
  if(mResizeToContentsAction == nullptr){
    mResizeToContentsAction = new ResizeToContentsAction(this);
    connect(mResizeToContentsAction, &ResizeToContentsAction::resizeToContentsTriggered, this, &AbstractTableViewWidget::resizeViewToContents);
  }
  Q_ASSERT(mResizeToContentsAction != nullptr);
  registerActions(mResizeToContentsAction);
}


}} // namespace Mdt{ namespace ItemEditor{
