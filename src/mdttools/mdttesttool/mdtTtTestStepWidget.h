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
#ifndef MDT_TT_TEST_STEP_WIDGET_H
#define MDT_TT_TEST_STEP_WIDGET_H

#include "mdtError.h"
#include "mdtTtTestStep.h"
#include <QWidget>
#include <QString>

class QPushButton;
class QLabel;

class QGridLayout;

/*! \brief Test step widget
 *
 * This class should be used with mdtTtTestStep
 */
class mdtTtTestStepWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestStepWidget(QWidget *parent = 0);

  /*! \brief Set title
   */
  void setTitle(const QString & text);

  /*! \brief Set message
   */
  void setMessage(const QString & msg);

  /*! \brief Set message
   */
//   void setMessage(const mdtError & error);

  /*! \brief Clear message
   */
  void clearMessage();

  /*! \brief Set abortSupported flag
   *
   * Default is false.
   */
  void setAbortSupported(bool support);

  /*! \brief Set Run/Abort button enabled
   *
   * When button is disabled by this function,
   *  it will still disabled until it is re-enabled.
   *  When button is enabled by this function,
   *  it is handled regarding state set with setState().
   */
  void setRunAbortButtonEnabled(bool enable);

  /*! \brief Turn to Running state
   *
   * Note: if user clicked the run/abort button,
   *  this is done automatically.
   */
//   void setRunning();

  /*! \brief Turn to Success state
   */
//   void setFinishedSuccess();

  /*! \brief Turn to Warn state
   */
//   void setFinishedWarn();

  /*! \brief Turn to Warn state and display a message (in message label)
   */
//   void setFinishedWarn(const QString & msg);

  /*! \brief Turn to Warn state and display a message (in message label)
   */
//   void setFinishedWarn(const mdtError & msg);

  /*! \brief Turn to Fail state
   */
//   void setFinishedFail();
// 
//   /*! \brief Turn to Fail state and display a message (in message label)
//    */
//   void setFinishedFail(const QString & msg);
// 
//   /*! \brief Turn to Fail state and display a message (in message label)
//    */
//   void setFinishedFail(const mdtError & msg);

  /*! \brief Reset to Initial state
   */
//   void reset();

  /*! \brief Enable or disable Run/Abort function
   */
  //void setRunAbortEnabled(bool enable);

  /*! \brief Set state
   */
  void setState(mdtTtTestStep::State_t state);

 signals:

  /*! \brief Emitted when Run/Abort button was clicked
   */
  void runAbortButtonClicked();

//   /*! \brief Emitted when run function was clicked
//    */
//   void runCalled();
// 
//   /*! \brief Emitted when abort function was clicked
//    */
//   void abortCalled();

  /*! \brief Emitted when test item has started
   *
   * \param w this pointer
   */
  void started(mdtTtTestStepWidget *w);

  /*! \brief Emitted when test item has finished (or was aborted)
   *
   * \param w this pointer
   */
  void finished(mdtTtTestStepWidget *w);

//  private slots:
// 
//   /*! \brief
//    */
//   void runAbort();

 private:

  /*! \brief Set initial state
   */
  void setStateInitial();

  /*! \brief Set running state
   */
  void setStateRunning();

  /*! \brief Set test fail state
   */
  void setStateFail();

  /*! \brief Set warn state
   */
  void setStateWarn();

  /*! \brief Set success state
   */
  void setStateSuccess();

  /*! \brief Update Run/Abort button enabled state
   *
   * Enabled state is set regarding pvRunAbortButtonEnabled, pvState and pvAbortSupported
   */
  void updateRunAbortButtonEnabledState();

  Q_DISABLE_COPY(mdtTtTestStepWidget);

  mdtTtTestStep::State_t pvState;
  QGridLayout *pvLayout;
  QPushButton *pbRunAbort;
  bool pvRunAbortButtonEnabled;
  bool pvAbortSupported;
  QLabel *lbTitle;
  QLabel *lbState;
  QLabel *lbMessage;
};

#endif // #ifndef MDT_TT_TEST_STEP_WIDGET_H
