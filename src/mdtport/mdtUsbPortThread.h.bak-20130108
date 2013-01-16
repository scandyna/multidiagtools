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

  mdtUsbPortThread(QObject *parent = 0);

  // Overload of mdtPortThread
  void stop();

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

  /*! \brief Get a new frame for writing data to port
   *
   * If no frame is available for write, this method
   *  will block the caller thread until one is avaliable,
   *  or another request happens (typical: control query).
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \return A pointer to a new frame, or Null on another request.
   *
   * \pre Port must be set with setPort() before using this method.
   */
  mdtFrame *getNewFrameWrite();

  /*! \brief Write data to port
   *
   * This is a helper method to write a frame to the USB port.
   *
   * Note about port mutex handling:<br>
   *  The port mutext must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param port A valid instance of mdtUsbPort
   * \param frame Data stored in thos frame will be written to port.
   * \param maxWriteTry Some port can return 0 byte or a timeout error if busy. If this happens,
   *                     this method will sleep some time an try a write call again until
   *                     write call works successfull or maxWriteTry is reached.
   *
   * \return True on successfull transfert, or false on error or by stop request.
   *          If this method returns false, the thread should be stopped.
   *
   * \pre port must be set with setPort() before using this method.
   * \pre frame must be a valid pointer (not Null).
   */
  mdtAbstractPort::error_t writeToPort(mdtUsbPort *port, mdtFrame *frame, int maxWriteTry = 10);

  void run();
};

#endif  // #ifndef MDT_USB_PORT_THREAD_H
