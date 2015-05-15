/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtTtTestStepContainerWidget.h"
#include "mdtTtTestStepContainer.h"

#include <QLabel>
#include <QFrame>

#include <QDebug>

mdtTtTestStepContainerWidget::mdtTtTestStepContainerWidget(QWidget* parent)
 : QWidget(parent),
   pvLayout(new QVBoxLayout),
   pvStepWidgetsCount(0),
   pvSeparationLinesVisible(true)
{
  pvLayout->addStretch(1);
  setLayout(pvLayout);
}

void mdtTtTestStepContainerWidget::setContainer(mdtTtTestStepContainer* c)
{
  Q_ASSERT(c != nullptr);

  connect(c, SIGNAL(stepWidgetAdded(mdtTtTestStepWidget*)), this, SLOT(addStepWidget(mdtTtTestStepWidget*)));
  connect(c, SIGNAL(stepWidgetRemoved(mdtTtTestStepWidget*)), this, SLOT(removeStepWidget(mdtTtTestStepWidget*)));
}

void mdtTtTestStepContainerWidget::showSeparationLines()
{
  pvSeparationLinesVisible = true;
  updateSeparationLinesVisibility();
}

void mdtTtTestStepContainerWidget::hideSeparationLines()
{
  pvSeparationLinesVisible = false;
  updateSeparationLinesVisibility();
}

void mdtTtTestStepContainerWidget::addStepWidget(mdtTtTestStepWidget* tsw)
{
  Q_ASSERT(tsw != nullptr);
  Q_ASSERT(pvLayout->count() > 0);  // A stretch was added in constructor

  pvLayout->insertWidget(pvLayout->count()-1, tsw);
  ++pvStepWidgetsCount;
  addSeparationLine();
}

void mdtTtTestStepContainerWidget::removeStepWidget(mdtTtTestStepWidget* tsw)
{
  Q_ASSERT(tsw != nullptr);

  pvLayout->removeWidget(tsw);
  delete tsw;
  --pvStepWidgetsCount;
  removeSeparationLine();
}

void mdtTtTestStepContainerWidget::addSeparationLine()
{
  Q_ASSERT(pvLayout->count() > 0);

  QFrame *line = new QFrame;
  line->setFrameStyle(QFrame::HLine | QFrame::Plain);
  pvLayout->insertWidget(pvLayout->count()-1, line);
  pvSeparationLines.emplace_back(line);
  line->setVisible(pvSeparationLinesVisible);
}

void mdtTtTestStepContainerWidget::removeSeparationLine()
{
  if(pvSeparationLines.empty()){
    return;
  }
  QFrame *line = pvSeparationLines.back();
  Q_ASSERT(line != nullptr);
  qDebug() << "Remocing line...";
  pvLayout->removeWidget(line);
  delete line;
  pvSeparationLines.pop_back();
}

void mdtTtTestStepContainerWidget::updateSeparationLinesVisibility()
{
  for(auto *line : pvSeparationLines){
    Q_ASSERT(line != nullptr);
    line->setVisible(pvSeparationLinesVisible);
  }
}
