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
#include "mdtTtTestItemContainerWidget.h"

#include <QLabel>

mdtTtTestItemContainerWidget::mdtTtTestItemContainerWidget(QWidget* parent)
 : QWidget(parent),
   pvLayout(new QVBoxLayout)
{
  setLayout(pvLayout);
  ///setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

void mdtTtTestItemContainerWidget::addItem(mdtTtTestItemWidget* w)
{
  Q_ASSERT(w != nullptr);

  connect(w, SIGNAL(started(mdtTtTestItemWidget*)), this, SLOT(disableRunAbortOfOtherWidgets(mdtTtTestItemWidget*)));
  connect(w, SIGNAL(finished(mdtTtTestItemWidget*)), this, SLOT(enableRunAbortOfAllWidgets()));
  ///w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  pvLayout->addWidget(w);
  pvItemWidgets.append(w);
}

void mdtTtTestItemContainerWidget::addStretch()
{
  pvLayout->addStretch(1);
}

void mdtTtTestItemContainerWidget::setRunAbortEnabled(bool enable)
{
  for(auto *w : pvItemWidgets){
    Q_ASSERT(w != nullptr);
    w->setRunAbortEnabled(enable);
  }
}

void mdtTtTestItemContainerWidget::disableRunAbortOfOtherWidgets(mdtTtTestItemWidget* callerWidget)
{
  Q_ASSERT(callerWidget != nullptr);

  for(auto *w : pvItemWidgets){
    Q_ASSERT(w != nullptr);
    if(w != callerWidget){
      w->setRunAbortEnabled(false);
    }
  }
}

void mdtTtTestItemContainerWidget::enableRunAbortOfAllWidgets()
{
  setRunAbortEnabled(true);
//   for(auto *w : pvItemWidgets){
//     Q_ASSERT(w != nullptr);
//     w->setRunAbortEnabled(true);
//   }
}
