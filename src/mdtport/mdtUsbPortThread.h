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
#include "mdtPortThread.h"
#include "mdtFrame.h"

class mdtUsbPort;

class mdtUsbPortThread : public mdtPortThread
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtUsbPortThread(QObject *parent = 0);

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   */
  bool isReader() const;

 signals:

  /*! \brief Emited when a new control response was readen
   */
  void controlResponseReaden();

  /*! \brief Wmited when a message IN was readen (from additionnal interrupt IN endpoint)
   */
  void messageInReaden();

 private:

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

  void run();
};

#endif  // #ifndef MDT_USB_PORT_THREAD_H
