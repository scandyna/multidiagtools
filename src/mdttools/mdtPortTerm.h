/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_PORT_TERM_H
#define MDT_PORT_TERM_H

#include "ui_mdtPortTerm.h"
#include "mdtPortManager.h"
#include "mdtSerialPortSetupDialog.h"
#include "mdtSerialPortCtlWidget.h"
#include <QWidget>
#include <QMainWindow>
#include <QLocale>
#include <QEvent>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QActionGroup>

class QLabel;
class mdtPortStatusWidget;
class mdtSerialPortManager;
class mdtUsbtmcPortManager;

/*! \brief Mini port treminal
 */
class mdtPortTerm : public QMainWindow, public Ui::mdtPortTerm
{
 Q_OBJECT

 public:

  mdtPortTerm(QWidget *parent = 0);
  ~mdtPortTerm();

  /*! \brief Build the translations menu
   */
  void setAvailableTranslations(const QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey);

 public slots:

  /*! \brief Append incomming data to terminal
   */
  void appendReadenData(mdtPortTransaction *transaction);

  /*! \brief Send command to current port
   */
  void sendCmd();

  /*! \brief Abort command transmission
   */
  void on_pbSendCmdAbort_clicked();

  /*! \brief Clear terminal
   */
  void on_pbClearTerm_clicked();

  /*! \brief Retranslate
   *
   * This slot is called by mdtApplication
   *  when language has changed
   */
  void retranslate();

 private slots:

  // Set/unset port type to serial port
  void attachToSerialPort();
  void detachFromSerialPort();
  // Call port setup dialog
  void portSetup();

  // Set/unset port type to USBTMC port
  void attachToUsbtmcPort();
  void detachFromUsbtmcPort();

  // Select a port type
  void selectPortType(QAction*);

  // Detach all ports
  void detachFromPorts();

  /*! \brief Update current state
   */
  void setStateFromPortManager(int state);

 private:

  /*! \brief Set the disconnected state
   */
  void setStatePortClosed();

  /*! \brief Set the disconnected state
   */
  void setStateDisconnected();

  /*! \brief Set the connecting state
   */
  void setStateConnecting();

  /*! \brief Set the ready state
   */
  void setStateReady();

  /*! \brief Set the busy state
   */
  void setStateBusy();

  /*! \brief Set the error state
   */
  void setStateError();

  /*! \brief Used to show a message in status bar
   *
   * \param message Message to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showStatusMessage(const QString &message, int timeout = 0);

  /*! \brief Send command/query to serial port
   */
  bool sendCommandToSerialPort(const QString &command);

  /*! \brief Send command/query to USBTMC port
   */
  bool sendCommandToUsbtmcPort(const QString &command);

  // Diseable copy
  Q_DISABLE_COPY(mdtPortTerm);

  // Serial port
  mdtSerialPortManager *pvSerialPortManager;
  mdtSerialPortCtlWidget *pvSerialPortCtlWidget;
  // USBTMC port
  mdtUsbtmcPortManager *pvUsbtmcPortManager;
  // Current port manager
  mdtPortManager *pvCurrentPortManager;
  // Translations menu
  QActionGroup *pvLanguageActionGroup;
  // Port menu
  QActionGroup *pvPortSelectActionGroup;
  // Running flag (if true, command can be sent, ...)
  bool pvReady;
  // Status bar
  mdtPortStatusWidget *pvStatusWidget;
  // Term char or string
  QByteArray pvCmdTermSequence;
};

#endif  // #ifndef MDT_PORT_TERM_H
