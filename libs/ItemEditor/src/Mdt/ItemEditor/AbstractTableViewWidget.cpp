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
#include "TableViewController.h"
#include "InsertAction.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractTableViewWidget::AbstractTableViewWidget(QWidget* parent)
 : QWidget(parent),
   mView(new QTableView),
   mMainLayout(new QVBoxLayout)
{
  Q_ASSERT(mTopBarLayout == nullptr);
  Q_ASSERT(mBottomBarLayout == nullptr);
  Q_ASSERT(mTopEditBar == nullptr);
  Q_ASSERT(mInsertAction == nullptr);

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

void AbstractTableViewWidget::addInsertActionToTopArea(const QString & toolTip)
{
  if(mInsertAction == nullptr){
    createTopEditBarIfNot();
    mInsertAction = new InsertAction(this);
    mTopEditBar->addAction(mInsertAction->insertAction());
    Q_ASSERT(refController() != nullptr);
    connect(mInsertAction, &InsertAction::insertTriggered, refController(), &AbstractController::insert);
  }
  Q_ASSERT(mInsertAction != nullptr);
  mInsertAction->insertAction()->setToolTip(toolTip);
}

void AbstractTableViewWidget::createTopBarLayoutIfNot()
{
  if(mTopBarLayout == nullptr){
    mTopBarLayout = new QHBoxLayout;
    mMainLayout->insertLayout(0, mTopBarLayout);
  }
  Q_ASSERT(mTopBarLayout != nullptr);
}

void AbstractTableViewWidget::createTopEditBarIfNot()
{
  if(mTopEditBar == nullptr){
    mTopEditBar = addToolBarToTopArea();
  }
  Q_ASSERT(mTopEditBar != nullptr);
}

}} // namespace Mdt{ namespace ItemEditor{
