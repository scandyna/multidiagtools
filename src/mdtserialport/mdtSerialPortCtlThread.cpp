
#include "mdtSerialPortCtlThread.h"
#include <QDebug>
#include <QApplication>

#ifdef Q_OS_UNIX
 #include <pthread.h>
#endif


mdtSerialPortCtlThread::mdtSerialPortCtlThread(QObject *parent)
{
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

  // Run...
  while(pvRunning){
    // Wait on ctl signal event
    if(!pvSerialPort->waitEventCtl()){
      break;
    }
  }
}
