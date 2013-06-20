/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtSqlParentChildWidget.h"
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlRelation.h"
#include <QTabWidget>
#include <QVBoxLayout>

mdtSqlParentChildWidget::mdtSqlParentChildWidget(QWidget *parent)
 : QWidget(parent)
{
  pvParentWidget = 0;
  pvChildsTabWidget = 0;
  // Create a layout
  QVBoxLayout *layout = new QVBoxLayout;
  setLayout(layout);
}

mdtSqlParentChildWidget::~mdtSqlParentChildWidget()
{
}

void mdtSqlParentChildWidget::setParentWidget(mdtAbstractSqlWidget *widget)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(layout() != 0);

  /// \todo layout, ...
  pvParentWidget = widget;
  layout()->addWidget(pvParentWidget);
}

mdtAbstractSqlWidget *mdtSqlParentChildWidget::parentWidget()
{
  return pvParentWidget;
}

void mdtSqlParentChildWidget::addChildWidget(mdtAbstractSqlWidget *widget, mdtSqlRelation *relation, const QString &label)
{
  Q_ASSERT(pvParentWidget != 0);
  Q_ASSERT(widget != 0);
  Q_ASSERT(relation != 0);
  Q_ASSERT(layout() != 0);

  // Add widget to list
  pvChildWidgets.append(widget);
  // We reparent relation, so it will be deleted at the right moment by Qt
  relation->setParent(this);
  // Setup tab widget if needed
  if(pvChildsTabWidget == 0){
    pvChildsTabWidget = new QTabWidget;
    layout()->addWidget(pvChildsTabWidget);
  }
  Q_ASSERT(pvChildsTabWidget != 0);
  pvChildsTabWidget->addTab(widget, label);
  ///widget->enableLocalNavigation();
  widget->enableLocalEdition();
  // Make needed connections
  connect(pvParentWidget, SIGNAL(currentRowChanged(int)), relation, SLOT(setParentCurrentIndex(int)));
  connect(pvParentWidget, SIGNAL(stateVisualizingExited()), this, SLOT(setChiledWidgetsDisabled()));
  connect(pvParentWidget, SIGNAL(stateVisualizingEntered()), this, SLOT(setChiledWidgetsEnabled()));
  // Force a update of relations
  relation->setParentCurrentIndex(pvParentWidget->currentRow());
}

/**
void mdtSqlParentChildWidget::setChiledWidgetsDisabled()
{
  int i;

  for(i=0; i<pvChildWidgets.size(); ++i){
    Q_ASSERT(pvChildWidgets.at(i) != 0);
    pvChildWidgets.at(i)->setEnabled(false);
  }
}

void mdtSqlParentChildWidget::setChiledWidgetsEnabled()
{
  int i;

  for(i=0; i<pvChildWidgets.size(); ++i){
    Q_ASSERT(pvChildWidgets.at(i) != 0);
    pvChildWidgets.at(i)->setEnabled(true);
  }
}
*/