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
#include "mdtTtValueLimits.h"
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
 * Used as is, state of the test step can be stored,
 *  and GUI part is updatet.
 *
 * A way to create a test step is to subclass mdtTtTestStep,
 *  wich also offers some common functionnality, such as access
 *  to test node manager and its global device container.
 */
class mdtTtTestStep : public QObject
{
 Q_OBJECT

  friend class mdtTestToolTest; // For unit tests

 public:

  /*! \brief State
   */
  enum class State_t
  {
    Initial,  /*!< Initial is the state before test step was executed. */
    Running,  /*!< Test step is running. */
    Fail,     /*!< Test step was executed and failed. */
    Warn,     /*!< Test step was exectued and the result is not so good. */
    Success   /*!< Test step was exectued and the result is good. */
  };

  /*! \brief Constructor
   *
   * \param tnm Test node manager object.
   * \param tsw Test step widget. Note that tsw is not destroyed when this object is.
   * \param parent See Qt's QObject documentation.
   */
  mdtTtTestStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget *tsw = nullptr, QObject *parent = nullptr);

  /*! \brief Access test step widget
   *
   * Returns the widget that was passed in constructor,
   *  wich can also be a nullptr.
   */
  mdtTtTestStepWidget* widget()
  {
    return pvWidget;
  }

  /*! \brief Set title
   */
  void setTitle(const QString & text);

  /*! \brief Set message
   */
  void setMessage(const QString & msg);

  /*! \brief Set message
   */
  void setMessage(const mdtError & error);

  /*! \brief Clear message
   */
  void clearMessage();

  /*! \brief Set abortSupported flag
   *
   * Default is false.
   */
  void setAbortSupported(bool support);

  /*! \brief Get abortSupported flag
   */
  bool abortSupported() const
  {
    return pvAbortSupported;
  }

  /*! \brief Set Run/Abort function enabled
   *
   * See mdtTtTestStepWidget::setRunAbortButtonEnabled().
   */
  void setRunAbortEnabled(bool enable);

  /*! \brief Turn to Running state
   *
   * Note: if user clicked the run/abort button,
   *  this is done automatically.
   */
  void setRunning();

  /*! \brief Turn to Success state
   */
  void setFinishedSuccess();

  /*! \brief Turn to Warn state
   */
  void setFinishedWarn();

  /*! \brief Turn to Warn state and display a message (in message label)
   */
  void setFinishedWarn(const QString & msg)
  {
    setMessage(msg);
    setFinishedWarn();
  }

  /*! \brief Turn to Warn state and display a message (in message label)
   */
  void setFinishedWarn(const mdtError & msg)
  {
    setMessage(msg);
    setFinishedWarn();
  }

  /*! \brief Turn to Fail state
   */
  void setFinishedFail();

  /*! \brief Turn to Fail state and display a message (in message label)
   */
  void setFinishedFail(const QString & msg)
  {
    setMessage(msg);
    setFinishedFail();
  }

  /*! \brief Turn to Fail state and display a message (in message label)
   */
  void setFinishedFail(const mdtError & msg)
  {
    setMessage(msg);
    setFinishedFail();
  }

  /*! \brief Reset to Initial state
   */
  void reset();

  /*! \brief Get test step state
   */
  State_t state() const
  {
    return pvState;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 public slots:

  /*! \brief Run or abort test step
   *
   * Depending on state and abortSupported flag,
   *  runCalled() or abortCalled() will be emitted.
   *
   * Note: only signal is emitted, state is not updated.
   */
  void runAbort();

 signals:

  /*! \brief Emitted when run function was clicked
   */
  void runCalled();

  /*! \brief Emitted when abort function was clicked
   */
  void abortCalled();

  /*! \brief Emitted when test step has started
   *
   * \param ts this test step
   */
  void started(mdtTtTestStep* ts);

  /*! \brief Emitted when test step has stopped (normally finished or aborted)
   *
   * \param ts this test step
   */
  void stopped(mdtTtTestStep* ts);

 protected:

  /*! \brief Access test node manager
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager()
  {
    Q_ASSERT(pvTestNodeManager);
    return pvTestNodeManager;
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
   *
   * \deprecated Use isValueOk()
   */
  bool isInRange(const mdtValueDouble & x, double min, double max = std::numeric_limits<double>::infinity(), const QString & valueUnit = QString());

  /*! \brief Check if x is ok for given limits
   *
   * Return true if value is Ok (see mdtTtValueLimits::getResult() for details).
   *  If value is not Ok, a descriptive text is stored to pvLastError
   *  You can use pvLastError.updateText() to set the main error text.
   *
   * If x is null (wich can happen, f.ex., by communication error),
   *  a descriptive text is alos stored in pvLastError, and false is returned.
   *
   * Note: if value has -OL or +OL flag set, -infinity, respecively +infinity value is considered.
   *
   * \pre limits must be set (see mdtTtValueLimits::isSet()).
   */
  bool isValueOk(const mdtValueDouble & x, const mdtTtValueLimits & limits, const QString & valueUnit = QString());

  /*! \brief Storage of last error
   */
  mdtError pvLastError;

 private slots:

  /*! \brief Set widget pointer null when it is destroyed
   */
  void setWidgetNull();

 private:

  Q_DISABLE_COPY(mdtTtTestStep);

  State_t pvState;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  mdtTtTestStepWidget *pvWidget;
  bool pvAbortSupported;
};

#endif // #ifndef MDT_TT_TEST_STEP_H
