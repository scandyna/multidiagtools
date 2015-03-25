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
#ifndef MDT_TT_TEST_STEP_H
#define MDT_TT_TEST_STEP_H

#include "mdtError.h"
#include "mdtTtTestNodeManager.h"
#include "mdtValue.h"
#include <QObject>
#include <QString>
#include <memory>
#include <limits>

class mdtTtTestStepWidget;

/*! \brief Test step class
 *
 * mdtTtTestStep provides some common functionnality
 *  for a test step.
 *
 * This class can be used together with mdtTtTestStepWidget,
 *  witch provides the GUI part.
 *
 * Used as is, state of the test staep can be stored,
 *  and GUI part is updatet.
 *
 * A way to create a test step is to subclass mdtTtTestStep,
 *  wich also offers some common functionnality, such as access
 *  to test node manager and its global device container.
 */
class mdtTtTestStep : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr, QObject *parent = nullptr);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 protected:

  /*! \brief Access test node manager
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager()
  {
    Q_ASSERT(pvTestNodeManager);
    return testNodeManager();
  }

  /*! \brief Get a device from test node manager's global device container
   */
  template<typename T>
  std::shared_ptr<T> device(const QString & alias)
  {
    Q_ASSERT(pvTestNodeManager);
    return pvTestNodeManager->device<T>(alias);
  }

  /*! \brief Check if x is in range
   *
   * Return true if value is in [min;max]
   *  If value is out of range, a descriptive text is stored to pvLastError
   *  You can use pvLastError.updateText() to set the main error text.
   *
   * Note: if value has -OL or +OL flag set, -infinity, respecively +infinity value is considered.
   */
  bool isInRange(const mdtValueDouble & x, double min, double max = std::numeric_limits<double>::infinity(), const QString & valueUnit = QString());

  /*! \brief Storage of last error
   */
  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtTtTestStep);

  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  mdtTtTestStepWidget *pvWidget;
};

#endif // #ifndef MDT_TT_TEST_STEP_H
