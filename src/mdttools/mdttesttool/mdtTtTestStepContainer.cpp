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

mdtTtTestStepContainer::mdtTtTestStepContainer(QObject* parent)
 : QObject(parent)
{
}

// std::shared_ptr<mdtTtTestStep> mdtTtTestStepContainer::addStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw)
// {
//   auto ts = createStep<mdtTtTestStep>(tnm, tsw);
// 
//   pvSteps.emplace_back(ts);
// 
//   return ts;
// }

// std::shared_ptr<mdtTtTestStep> mdtTtTestStepContainer::addStep(int index, const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw )
// {
//   auto ts = createStep<mdtTtTestStep>(tnm, tsw);
//   auto it = pvSteps.begin();
// 
//   pvSteps.insert(it + index, ts);
// 
//   return ts;
// }

// std::shared_ptr< mdtTtTestStep > mdtTtTestStepContainer::createStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw)
// {
//   std::shared_ptr<mdtTtTestStep> ts(new mdtTtTestStep(tnm, tsw));
// 
//   connect(ts.get(), SIGNAL(started(mdtTtTestStep*)), this, SLOT(disableRunAbortOfOtherSteps(mdtTtTestStep*)));
//   connect(ts.get(), SIGNAL(stopped(mdtTtTestStep*)), this, SLOT(enableRunAbortOfAllSteps()));
//   if(tsw != nullptr){
//     emit stepWidgetAdded(tsw);
//   }
// 
//   return ts;
// }

void mdtTtTestStepContainer::clear()
{
  pvSteps.clear();
}

mdtTtTestStep::State_t mdtTtTestStepContainer::state() const
{
  if(pvSteps.empty()){
    return mdtTtTestStep::State_t::Initial;
  }
  /*
   * We make it simple here:
   *  - Copy all states to a vector
   *  - Use std::find (using containsState helper function) in the correct order (priority) to determine global state
   */
  // Copy states of each stest step
  std::vector<mdtTtTestStep::State_t> allStates;
  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    allStates.emplace_back(ts->state());
  }
  // Determine global state
  if(containsState(allStates, mdtTtTestStep::State_t::Running)){
    return mdtTtTestStep::State_t::Running;
  }
  if(containsState(allStates, mdtTtTestStep::State_t::Fail)){
    return mdtTtTestStep::State_t::Fail;
  }
  if(containsState(allStates, mdtTtTestStep::State_t::Warn)){
    return mdtTtTestStep::State_t::Warn;
  }
  if(containsState(allStates, mdtTtTestStep::State_t::Initial)){
    return mdtTtTestStep::State_t::Initial;
  }
  return mdtTtTestStep::State_t::Success;
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
