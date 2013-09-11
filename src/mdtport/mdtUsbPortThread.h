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
#ifndef MDT_USB_PORT_THREAD_H
#define MDT_USB_PORT_THREAD_H

#include <QObject>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QString>
#include "mdtPortThread.h"
#include "mdtFrame.h"

class mdtUsbPort;
class mdtFrameUsbControl;

class mdtUsbPortThread : public mdtPortThread
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtUsbPortThread(QObject *parent = 0);

  /*! \brief Set the port instance
   *
   * \pre port must be a valid pointer
   * \pre The thread must not running
   */
  void setPort(mdtUsbPort *port);

  /*! \brief Get USB port instance
   *
   * \return Port instance, or a null pointer if it was not set
   */
  mdtUsbPort *usbPort();

  /*! \brief Detach the port from thread
   *
   * \param releaseMemory If true, the port object that was set with setPort() will be deleted.
   * \pre The thread must not running
   */
  void detachPort(bool releaseMemory);

  /*! \brief Returns true
   */
  bool isReader() const;

  /*! \brief Returns true
   */
  bool isWriter() const;

  /*! \brief Retunrs true
   */
  bool handlesTimeout() const;

 signals:

  /*! \brief Emited when a new control response was readen
   */
  void controlResponseReaden();

  /*! \brief Emited when a message IN was readen (from additionnal interrupt IN endpoint)
   */
  void messageInReaden();

  /*! \brief Emitted when the read until a short packet process is finished
   */
  void readUntilShortPacketReceivedFinished();

 protected:

  /*! \brief Wait until a new control transfer is possible (i.e. when control frames pool is not empty)
   */
  mdtAbstractPort::error_t waitOnControlTransferPossible(bool notifyOnControlTransferComplete);

  /*! \brief Send a control query and wait until response is available
   */
  mdtAbstractPort::error_t sendControlQuery(mdtFrameUsbControl *ctlFrame);

  /*! \brief Read until a short frame is received
   *
   * \param maxReadTransfers Maximum of read transfers before return
   *                          (If device returns never a short frame,
   *                          this method will return after maxReadTransfers).
   * \return NoError or a error of type mdtAbstractPort::error_t.
   *          If maxReadTransfers is reached, ReadTimeout is returned.
   * \pre Port must be set with setPort() before using this method.
   */
  mdtAbstractPort::error_t readUntilShortPacketReceived(int maxReadTransfers);

 private:

  /*! \brief Working thread method
   */
  void run();

  Q_DISABLE_COPY(mdtUsbPortThread);

  mdtUsbPort *pvUsbPort;
};

#endif  // #ifndef MDT_USB_PORT_THREAD_H
