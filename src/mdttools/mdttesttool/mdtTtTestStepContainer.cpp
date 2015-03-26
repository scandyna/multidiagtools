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
#include "mdtTtTestStepContainer.h"
#include "mdtTtTestStep.h"

mdtTtTestStepContainer::mdtTtTestStepContainer(QObject* parent)
 : QObject(parent)
{
}

std::shared_ptr<mdtTtTestStep> mdtTtTestStepContainer::addStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw)
{
  std::shared_ptr<mdtTtTestStep> ts(new mdtTtTestStep(tnm, tsw));
  connect(ts.get(), SIGNAL(started(mdtTtTestStep*)), this, SLOT(disableRunAbortOfOtherSteps(mdtTtTestStep*)));
  connect(ts.get(), SIGNAL(stopped(mdtTtTestStep*)), this, SLOT(enableRunAbortOfAllSteps()));
  pvSteps.emplace_back(ts);

  return ts;
}

void mdtTtTestStepContainer::clear()
{
  pvSteps.clear();
}

void mdtTtTestStepContainer::setRunAbortEnabled(bool enable)
{
  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    ts->setRunAbortEnabled(enable);
  }
}

void mdtTtTestStepContainer::disableRunAbortOfOtherSteps(mdtTtTestStep* caller)
{
  Q_ASSERT(caller);

  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    if(ts.get() != caller){
      ts->setRunAbortEnabled(false);
    }
  }
}

void mdtTtTestStepContainer::enableRunAbortOfAllSteps()
{
  setRunAbortEnabled(true);
}
