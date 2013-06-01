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
 *
 * \todo suspendTransmission() call is no longer in getNewFrameRead(), must be handled in run() now.
 */
class mdtPortReadThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtPortReadThread(QObject *parent = 0);

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   */
  bool isReader() const;

  /*! \brief Retunrs false
   */
  bool handlesTimeout() const;

 private:

  // Thread implementation
  void run();

  bool pvTransmissionSuspended; // Flag used to remeber if a transmission suspend was request.
  int pvMinPoolSizeBeforeReadSuspend;

  Q_DISABLE_COPY(mdtPortReadThread);
};

#endif  // #ifndef MDT_PORT_READ_THREAD_H
