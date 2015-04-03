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
#ifndef MDT_TT_TEST_STEP_CONTAINER_H
#define MDT_TT_TEST_STEP_CONTAINER_H

#include "mdtTtTestStep.h"
#include <QObject>
#include <vector>
#include <memory>
#include <algorithm>

class mdtTtTestStepWidget;
class mdtTtTestNodeManager;

/*! \brief Container of test steps
 */
class mdtTtTestStepContainer : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestStepContainer(QObject *parent = 0);

  /*! \brief Create and add a test step
   *
   * The newly created test step is added at the end of the container.
   *  Be carefull when using the variant that accepts index argument.
   */
  std::shared_ptr<mdtTtTestStep> addStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr)
  {
    return addStep<mdtTtTestStep>(tnm, tsw);
  }

  /*! \brief Create and add a user defined test step
   *
   * The newly created test step is added at the end of the container.
   *  Be carefull when using the variant that accepts index argument.
   */
  template<typename T>
  std::shared_ptr<T> addStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr)
  {
    auto ts = createStep<T>(tnm, tsw);

    pvSteps.emplace_back(ts);

    return ts;
  }

  /*! \brief Create and add a test step
   *
   * This version is usefull if, f.ex., a enum is used to find test steps.
   *  When using this function, be carefull to not let some indices with no test step.
   *  This function does not care about this.
   */
  std::shared_ptr<mdtTtTestStep> addStep(int index, const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr)
  {
    return addStep<mdtTtTestStep>(index, tnm, tsw);
  }

  /*! \brief Create and add a user defined test step
   *
   * This version is usefull if, f.ex., a enum is used to find test steps.
   *  When using this function, be carefull to not let some indices with no test step.
   *  This function does not care about this.
   */
  template<typename T>
  std::shared_ptr<T> addStep(int index, const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr)
  {
    auto ts = createStep<T>(tnm, tsw);
    auto it = pvSteps.begin();

    pvSteps.insert(it + index, ts);

    return ts;
  }

  /*! \brief Get test step at given index
   *
   * \pre index must be valid (0 <= index < count)
   */
  std::shared_ptr<mdtTtTestStep> step(int index)
  {
    Q_ASSERT( (index >= 0)&&(index < count()) );
    Q_ASSERT(pvSteps[index]);
    return pvSteps[index];
  }

  /*! \brief Get user defined test step at given index
   *
   * \pre index must be valid (0 <= index < count)
   */
  template<typename T>
  std::shared_ptr<T> step(int index)
  {
    return std::dynamic_pointer_cast<T>(step(index));
  }

  /*! \brief Get count of test steps
   */
  int count() const
  {
    return pvSteps.size();
  }

  /*! \brief Clear container
   */
  void clear();

  /*! \brief Check global state reagrding all test steps
   *
   * Will check state of all test steps, and return a global state.
   *  To determine the global state, following rules are applied:
   *  - 1) If one step has Running state, global state is Running
   *  - 2) If one step has Fail state, global state is Fail
   *  - 3) If one step has Warn state, global state is Warn
   *  - 4) If one step has Initial state, global state is Initial
   *  - 5) Global state is Success
   */
  mdtTtTestStep::State_t state() const;

  /*! \brief Reset state of all steps to Initial
   */
  void reset();

  /*! \brief Clear message of all test steps
   */
  void clearStepsMessage();

  /*! \brief Set Run/Abort function enabled for all test steps
   *
   * See mdtTtTestStepWidget::setRunAbortButtonEnabled().
   */
  void setRunAbortEnabled(bool enable);

 signals:

  /*! \brief Emitted when a test step with a widget was added to container
   */
  void stepWidgetAdded(mdtTtTestStepWidget *tsw);

  /*! \brief Emitted when a test step with a widget was removed from container
   */
//   void stepWidgetRemoved(mdtTtTestStepWidget *tsw);

 private slots:

  /*! \brief Disable run/abort function of all test steps, expect the one that called this slot
   */
  void disableRunAbortOfOtherSteps(mdtTtTestStep* caller);

  /*! \brief Enable run/abort function of all test steps
   */
  void enableRunAbortOfAllSteps();

 private:

  /*! \brief Create a test step
   */
  template<typename T>
  std::shared_ptr<T> createStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr)
  {
    std::shared_ptr<T> ts(new T(tnm, tsw));

    connect(ts.get(), SIGNAL(started(mdtTtTestStep*)), this, SLOT(disableRunAbortOfOtherSteps(mdtTtTestStep*)));
    connect(ts.get(), SIGNAL(stopped(mdtTtTestStep*)), this, SLOT(enableRunAbortOfAllSteps()));
    if(tsw != nullptr){
      emit stepWidgetAdded(tsw);
    }

    return ts;
  }

  /*! \brief Check if state exists in vector
   */
  inline bool containsState(const std::vector<mdtTtTestStep::State_t> & v, mdtTtTestStep::State_t s) const
  {
    return std::find(v.cbegin(), v.cend(), s) != v.cend();
  }

  Q_DISABLE_COPY(mdtTtTestStepContainer);

  std::vector<std::shared_ptr<mdtTtTestStep> > pvSteps;
};

#endif // #ifndef MDT_TT_TEST_STEP_CONTAINER_H
