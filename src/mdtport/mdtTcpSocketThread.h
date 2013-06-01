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
#ifndef MDT_TCP_SOCKET_THREAD_H
#define MDT_TCP_SOCKET_THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include "mdtPortThread.h"
#include "mdtFrame.h"

class mdtTcpSocketThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtTcpSocketThread(QObject *parent = 0);

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   */
  bool isReader() const;

  /*! \brief Returns true if this thread writes date
   *
   * Is used by mdtPortThread::stop() 
   *  to know if mdtAbstractPort::abortFrameToWriteWait() must be called.
   */
  bool isWriter() const;

  /*! \brief Returns true
   *
   * mdtPortManager uses this flag to know how to deal with timeouts.
   */
  bool handlesTimeout() const;

 private:

  void run();

  QTcpSocket *pvSocket;
};

#endif  // #ifndef MDT_TCP_SOCKET_THREAD_H
