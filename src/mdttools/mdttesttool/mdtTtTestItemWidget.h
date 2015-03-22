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
#ifndef MDT_TT_TEST_ITEM_WIDGET_H
#define MDT_TT_TEST_ITEM_WIDGET_H

#include "mdtError.h"
#include <QWidget>
#include <QString>

class QPushButton;
class QLabel;

class QGridLayout;
class QHBoxLayout;

/*! \brief Test item widget
 */
class mdtTtTestItemWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief State
   */
  enum class State_t
  {
    Initial,  /*!< Initial is the state before test item was executed. */
    Running,  /*!< Test item is running. */
    Fail,     /*!< Test item was executed and failed. */
    Warn,     /*!< Test item was exectued and the result is not so good. */
    Success   /*!< Test item was exectued and the result good. */
  };

  /*! \brief Constructor
   */
  mdtTtTestItemWidget(QWidget *parent = 0);

  /*! \brief Set item title
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

  /*! \brief Turn to Running state
   *
   * Note: if user clicked the run/abort button,
   *  this is done automatically.
   */
  void setRunning();

  /*! \brief Turn to Success state
   */
  void setFinishedSuccess();

  /*! \brief Turn to Success state
   */
  void setFinishedWarn();

  /*! \brief Turn to Success state
   */
  void setFinishedFail();

  /*! \brief Reset to Initial state
   */
  void reset();

  /*! \brief Enable or disable Run/Abort function
   */
  void setRunAbortEnabled(bool enable);

 signals:

  /*! \brief Emitted when run function was clicked
   */
  void runCalled();

  /*! \brief Emitted when abort function was clicked
   */
  void abortCalled();

  /*! \brief Emitted when test item has started
   *
   * \param w this pointer
   */
  void started(mdtTtTestItemWidget *w);

  /*! \brief Emitted when test item has finished (or was aborted)
   *
   * \param w this pointer
   */
  void finished(mdtTtTestItemWidget *w);

 private slots:

  /*! \brief
   */
  void runAbort();

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

  Q_DISABLE_COPY(mdtTtTestItemWidget);

  State_t pvState;
  ///QGridLayout *pvLayout;
  QHBoxLayout *pvLayout;
  QPushButton *pbRunAbort;
  QLabel *lbTitle;
  QLabel *lbState;
  QLabel *lbMessage;
};

#endif // #ifndef MDT_TT_TEST_ITEM_WIDGET_H
