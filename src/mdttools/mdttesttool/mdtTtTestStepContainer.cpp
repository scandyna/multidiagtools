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

void mdtTtTestStepContainer::clear()
{
  /*
   * Signal the we want to remove a test step widget.
   * Note: no memory management is sone here.
   * Some container, such as mdtTtTestStepContainerWidget will delete widget itself.
   */
  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    if(ts->widget() != nullptr){
      emit stepWidgetRemoved(ts->widget());
    }
  }
  // Remove test steps
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

void mdtTtTestStepContainer::reset()
{
  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    ts->reset();
  }
}

void mdtTtTestStepContainer::clearStepsMessage()
{
  for(auto & ts : pvSteps){
    Q_ASSERT(ts);
    ts->clearMessage();
  }
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
