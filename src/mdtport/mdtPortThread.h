/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_PORT_THREAD_H
#define MDT_PORT_THREAD_H

#include "mdtAbstractPort.h"
#include "mdtError.h"
#include <QThread>
#include <QObject>

class mdtPortThread : public QThread
{
 Q_OBJECT

 public:

  mdtPortThread(QObject *parent = 0);
  ~mdtPortThread();

  /*! \brief Set the port instance
   * \pre port must be a valid pointer
   * \pre The thread must not running
   */
  void setPort(mdtAbstractPort *port);

  /*! \brief Start the thread
   *  \return True on sucsessfull start, or flase on start timeout
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  bool start();

  /*! \brief Stop the running thread
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  void stop();

  /*! \brief Returns true if the thread is running
   *  This function overloads the QThread::isRunning() function.
   *  Note for subclass: when the thread is started and ready, the private member pvRunning
   *  must be set to true.
   */
  bool isRunning() const;

  /*! \brief Returns false if the thread is running
   *  \see isRunning()
   */
  bool isFinished() const;

 signals:

  /*! \brief Emitted on error
   * 
   * When a error occurs, this signal is emited.<br>
   * This can happen, for example, when a USB device becomes not present.<br>
   * mdtPortManager uses this signal.
   * \sa mdtPortManager
   */
  void errorOccured(int error);

 protected:

  volatile bool pvRunning;
  mdtAbstractPort *pvPort;
  bool pvUseReadTimeoutProtocol;  // For example: MODBUS RTU mode
  ///int pvReadMinWaitTime;
  int pvWriteMinWaitTime;
  bool pvBytePerByteWrite;
};

#endif  // #ifndef MDT_PORT_THREAD_H
