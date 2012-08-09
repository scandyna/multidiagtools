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
#include "mdtSerialPortCtlThread.h"
#include "mdtSerialPort.h"
#include "mdtAbstractPort.h"
#include <QApplication>

#include <QDebug>

#ifdef Q_OS_UNIX
 #include <pthread.h>
#endif

mdtSerialPortCtlThread::mdtSerialPortCtlThread(QObject *parent)
 : mdtPortThread(parent)
{
}

void mdtSerialPortCtlThread::run()
{
  mdtAbstractPort::error_t portError;

  qDebug() << "CTLTHD: starting ...";
  Q_ASSERT(pvPort != 0);

  // We need a mdtSerialPort instance here
  mdtSerialPort *port = dynamic_cast<mdtSerialPort*>(pvPort);
  Q_ASSERT(port != 0);

#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif

  // Set the running flag
  port->lockMutex();
  pvRunning = true;

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait on ctl signal event
    portError = port->waitEventCtl();
    if(portError == mdtAbstractPort::WaitingCanceled){
      // Stopping
      break;
    }else if(portError == mdtAbstractPort::UnknownError){
      // Unhandled error. Signal this and stop
      emit(errorOccured(MDT_PORT_IO_ERROR));
      break;
    }
    // We have a event here, update the flags
    if(!port->getCtlStates()){
      break;
    }
  }
  qDebug() << "CTLTHD: Cleanup ...";

  port->unlockMutex();
  qDebug() << "CTLTHD: END";
}
