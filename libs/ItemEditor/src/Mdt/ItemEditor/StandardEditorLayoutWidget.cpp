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
#include "StandardEditorLayoutWidget.h"

#include <QVBoxLayout>
#include <QTabWidget>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

StandardEditorLayoutWidget::StandardEditorLayoutWidget(QWidget* parent)
 : QWidget(parent),
   mMainLayout(new QVBoxLayout),
   mMainWidget(nullptr),
   mChildsTabWidget(nullptr)
{
  setLayout(mMainLayout);
}

void StandardEditorLayoutWidget::setMainWidget(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  if(mMainWidget != nullptr){
    delete mMainWidget;
  }
  mMainWidget = widget;
  mMainLayout->insertWidget(0, widget);
}

void StandardEditorLayoutWidget::addChildWidget(QWidget* widget, const QString& label, const QIcon& icon)
{
  Q_ASSERT(widget != nullptr);

  // Setup tab widget if needed
  if(mChildsTabWidget == nullptr){
    mChildsTabWidget = new QTabWidget;
    mMainLayout->addWidget(mChildsTabWidget, 5);
  }
  Q_ASSERT(mChildsTabWidget != nullptr);
  // Add widget
  if(icon.isNull()){
    mChildsTabWidget->addTab(widget, label);
  }else{
    mChildsTabWidget->addTab(widget, icon, label);
  }
}

}} // namespace Mdt{ namespace ItemEditor{
