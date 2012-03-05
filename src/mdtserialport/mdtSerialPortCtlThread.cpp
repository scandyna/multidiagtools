
#include "mdtSerialPortCtlThread.h"
#include <QApplication>

#ifdef Q_OS_UNIX
 #include <pthread.h>
#endif

mdtSerialPortCtlThread::mdtSerialPortCtlThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvSerialPort = 0;
}

void mdtSerialPortCtlThread::setPort(mdtSerialPort *serialPort)
{
  Q_ASSERT(serialPort != 0);

  mdtPortThread::setPort(serialPort);
  pvSerialPort = serialPort;
}

#ifdef Q_OS_UNIX
void mdtSerialPortCtlThread::stop()
{
  Q_ASSERT(pvSerialPort != 0);

  // Unset the running flag and kill ioctl() wait function
  pvSerialPort->lockMutex();
  pvRunning = false;
  pvSerialPort->abortWaitEventCtl();
  pvSerialPort->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}
#endif

void mdtSerialPortCtlThread::run()
{
  Q_ASSERT(pvSerialPort != 0);

#ifdef Q_OS_UNIX
  pvSerialPort->defineCtlThread(pthread_self());
#endif

  // Set the running flag
  pvSerialPort->lockMutex();
  pvRunning = true;
  pvSerialPort->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    pvSerialPort->lockMutex();
    if(!pvRunning){
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();
    // Wait on ctl signal event
    if(!pvSerialPort->waitEventCtl()){
      pvSerialPort->lockMutex();
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    // We have a event here, update the flags
    pvSerialPort->lockMutex();
    if(!pvSerialPort->getCtlStates()){
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();
  }
}
