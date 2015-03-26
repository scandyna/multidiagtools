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

#include <QObject>
#include <vector>
#include <memory>

class mdtTtTestStep;
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
   */
  std::shared_ptr<mdtTtTestStep> addStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr);

  /*! \brief Get test step at given index
   *
   * \pre index must be valid (0 <= index < count)
   */
  std::shared_ptr<mdtTtTestStep> step(int index)
  {
    Q_ASSERT( (index >= 0)&&(index < count()) );
    return pvSteps[index];
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

  /*! \brief Set Run/Abort function enabled for all test steps
   *
   * See mdtTtTestStepWidget::setRunAbortButtonEnabled().
   */
  void setRunAbortEnabled(bool enable);

 private slots:

  /*! \brief Disable run/abort function of all test steps, expect the one that called this slot
   */
  void disableRunAbortOfOtherSteps(mdtTtTestStep* caller);

  /*! \brief Enable run/abort function of all test steps
   */
  void enableRunAbortOfAllSteps();

 private:

  Q_DISABLE_COPY(mdtTtTestStepContainer);

  std::vector<std::shared_ptr<mdtTtTestStep> > pvSteps;
};

#endif // #ifndef MDT_TT_TEST_STEP_CONTAINER_H
