
#include "mdtSerialPortCtlThread.h"
#include "mdtSerialPort.h"
#include <QApplication>

#include <QDebug>

#ifdef Q_OS_UNIX
 #include <pthread.h>
#endif

mdtSerialPortCtlThread::mdtSerialPortCtlThread(QObject *parent)
 : mdtPortThread(parent)
{
}

#ifdef Q_OS_UNIX
void mdtSerialPortCtlThread::stop()
{
  Q_ASSERT(pvPort != 0);

  // We need a mdtSerialPort instance here
  mdtSerialPort *port = dynamic_cast<mdtSerialPort*>(pvPort);
  Q_ASSERT(port != 0);

  // Unset the running flag and kill ioctl() wait function
  port->lockMutex();
  pvRunning = false;
  port->abortWaitEventCtl();
  port->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}
#endif

void mdtSerialPortCtlThread::run()
{
  Q_ASSERT(pvPort != 0);

  // We need a mdtSerialPort instance here
  mdtSerialPort *port = dynamic_cast<mdtSerialPort*>(pvPort);
  Q_ASSERT(port != 0);

#ifdef Q_OS_UNIX
  port->defineCtlThread(pthread_self());
#endif

  // Set the running flag
  port->lockMutex();
  pvRunning = true;
  ///port->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    ///port->lockMutex();
    if(!pvRunning){
      ///port->unlockMutex();
      break;
    }
    ///port->unlockMutex();
    // Wait on ctl signal event
    if(!port->waitEventCtl()){
      ///port->lockMutex();
      pvRunning = false;
      ///port->unlockMutex();
      break;
    }
    // We have a event here, update the flags
    ///port->lockMutex();
    if(!port->getCtlStates()){
      pvRunning = false;
      ///port->unlockMutex();
      break;
    }
  }

  port->unlockMutex();
}
