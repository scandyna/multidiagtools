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
#include "AbstractTableViewWidget.h"
#include "AbstractController.h"
#include "AbstractActionContainer.h"
#include "TableViewController.h"
#include "ResizeToContentsAction.h"
#include <QTableView>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractTableViewWidget::AbstractTableViewWidget(QWidget* parent)
 : AbstractEditorWidget(parent),
   mView(new QTableView)
{
  setCentralWidget(mView);
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

  auto *controller = abstractController();
  Q_ASSERT(controller != nullptr);

  connect(controller, &AbstractController::rowStateChanged, actions, &AbstractActionContainer::setRowState);
}

void AbstractTableViewWidget::resizeViewToContents()
{
  Q_ASSERT(mView != nullptr);
  mView->resizeColumnsToContents();
  mView->resizeRowsToContents();
}

void AbstractTableViewWidget::createTopToolsBarIfNot()
{
  if(mTopToolsBar == nullptr){
    mTopToolsBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopToolsBar != nullptr);
  mTopToolsBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void AbstractTableViewWidget::createResizeToContentsActionIfNot()
{
  if(mResizeToContentsAction == nullptr){
    mResizeToContentsAction = new ResizeToContentsAction(this);
    Q_ASSERT(abstractController() != nullptr);
    mResizeToContentsAction->setControllerStateMachine(abstractController()->controllerStateMachine());
    connect(mResizeToContentsAction, &ResizeToContentsAction::resizeToContentsTriggered, this, &AbstractTableViewWidget::resizeViewToContents);
    registerActions(mResizeToContentsAction);
  }
  Q_ASSERT(mResizeToContentsAction != nullptr);
}

}} // namespace Mdt{ namespace ItemEditor{
