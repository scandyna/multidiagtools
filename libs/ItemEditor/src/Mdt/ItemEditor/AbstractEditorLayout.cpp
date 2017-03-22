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
#include "AbstractEditorLayout.h"
#include <QLayoutItem>
#include <QHBoxLayout>
#include <QWidget>
#include <QToolBar>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

void AbstractEditorLayout::setCentralWidget(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  if(widget == mCentralWidget){
    return;
  }
  if(mCentralWidget.isNull()){
    mCentralWidget = widget;
    insertWidget( count()-1, mCentralWidget );
  }else{
    auto *item = replaceWidget(mCentralWidget, widget);
    delete item;
    delete mCentralWidget;
    mCentralWidget = widget;
  }
}

QWidget* AbstractEditorLayout::centralWidget() const
{
  return mCentralWidget;
}

QToolBar* AbstractEditorLayout::addToolBarToTopArea()
{
  auto *bar = new QToolBar;

  createTopBarLayoutIfNot();
  mTopBarLayout->insertWidget( mTopBarLayout->count()-1 , bar );

  return bar;
}

QToolBar* AbstractEditorLayout::addToolBarToBottomArea()
{
  auto *bar = new QToolBar;

  createBottomBarLayoutIfNot();
  mBottomBarLayout->insertWidget( mBottomBarLayout->count()-1 , bar );

  return bar;
}

void AbstractEditorLayout::createTopBarLayoutIfNot()
{
  if(mTopBarLayout == nullptr){
    mTopBarLayout = new QHBoxLayout;
    mTopBarLayout->addStretch();
    insertLayout(0, mTopBarLayout);
  }
  Q_ASSERT(mTopBarLayout != nullptr);
}

void AbstractEditorLayout::createBottomBarLayoutIfNot()
{
  if(mBottomBarLayout == nullptr){
    mBottomBarLayout = new QHBoxLayout;
    mBottomBarLayout->addStretch();
    addLayout(mBottomBarLayout);
  }
  Q_ASSERT(mBottomBarLayout != nullptr);
}

}} // namespace Mdt{ namespace ItemEditor{
