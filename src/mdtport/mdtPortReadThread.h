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
#ifndef MDT_PORT_READ_THREAD_H
#define MDT_PORT_READ_THREAD_H

#include "mdtPortThread.h"
#include "mdtFrame.h"
#include <QObject>

/*! \brief Reader thread for port I/O
 * 
 * It's possible to use the read timeout protocol.
 * An example of this is MODBUS (over serial line) in RTU mode.
 * 
 * \sa mdtPortConfig
 */
class mdtPortReadThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtPortReadThread(QObject *parent = 0);

 signals:

  /*! \brief Emited when a new frame is available
   * 
   * This signal is emited when a new frame is available.<br>
   * To get the frame, the simplest way is to use a mdtPortManager.<br>
   * It's also possible to use mdtPort, but this solution needs to handle
   * the mutex, verify the readen queue state, ...
   * \sa mdtPortManager
   * \sa mdtPort
   */
  void newFrameReaden();

 private:

  // Get a new frame from RX frames pool
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_PORT_READ_THREAD_H
