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

mdtTtTestStepContainerWidget::mdtTtTestStepContainerWidget(QWidget* parent)
 : QWidget(parent),
   pvLayout(new QVBoxLayout)
{
  pvLayout->addStretch(1);
  setLayout(pvLayout);
}

void mdtTtTestStepContainerWidget::setContainer(mdtTtTestStepContainer* c)
{
  Q_ASSERT(c != nullptr);

  connect(c, SIGNAL(stepWidgetAdded(mdtTtTestStepWidget*)), this, SLOT(addStepWidget(mdtTtTestStepWidget*)));
}



void mdtTtTestStepContainerWidget::addItem(mdtTtTestStepWidget* w)
{
  Q_ASSERT(w != nullptr);

  connect(w, SIGNAL(started(mdtTtTestStepWidget*)), this, SLOT(disableRunAbortOfOtherWidgets(mdtTtTestStepWidget*)));
  connect(w, SIGNAL(finished(mdtTtTestStepWidget*)), this, SLOT(enableRunAbortOfAllWidgets()));
  pvLayout->addWidget(w);
  pvStepWidgets.append(w);
}

// void mdtTtTestStepContainerWidget::addStretch()
// {
//   pvLayout->addStretch(1);
// }

void mdtTtTestStepContainerWidget::setRunAbortEnabled(bool enable)
{
  for(auto *w : pvStepWidgets){
    Q_ASSERT(w != nullptr);
    ///w->setRunAbortEnabled(enable);
  }
}

void mdtTtTestStepContainerWidget::addStepWidget(mdtTtTestStepWidget* tsw)
{
  Q_ASSERT(tsw != nullptr);
  Q_ASSERT(pvLayout->count() > 0);  // A stretch was added in constructor

  ///pvLayout->addWidget(tsw);
  pvLayout->insertWidget(pvLayout->count()-1, tsw);
}


void mdtTtTestStepContainerWidget::disableRunAbortOfOtherWidgets(mdtTtTestStepWidget* callerWidget)
{
  Q_ASSERT(callerWidget != nullptr);

  for(auto *w : pvStepWidgets){
    Q_ASSERT(w != nullptr);
    if(w != callerWidget){
      ///w->setRunAbortEnabled(false);
    }
  }
}

void mdtTtTestStepContainerWidget::enableRunAbortOfAllWidgets()
{
  setRunAbortEnabled(true);
}
