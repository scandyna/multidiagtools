
#include "mdtSerialPortThread.h"
#include <QDebug>
#include <QApplication>

mdtSerialPortThread::mdtSerialPortThread(QObject *parent)
 : QThread(parent)
{
  pvSerialPort = 0;
  pvRunning = false;
}

void mdtSerialPortThread::setSerialPort(mdtSerialPort *serialPort)
{
  Q_ASSERT(serialPort != 0);

  pvSerialPort = serialPort;
}

void mdtSerialPortThread::start()
{
  Q_ASSERT(pvSerialPort != 0);

  // Start..
  QThread::start();
  // Wait until the thread started
  while(!isRunning()){
    qApp->processEvents();
    msleep(50);
  }
}

void mdtSerialPortThread::stop()
{
  Q_ASSERT(pvSerialPort != 0);

  // Unset the running flag
  pvSerialPort->lockMutex();
  pvRunning = false;
  pvSerialPort->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}

bool mdtSerialPortThread::isRunning() const
{
  if(!QThread::isRunning()){
    return false;
  }
  pvSerialPort->lockMutex();
  if(pvRunning){
    pvSerialPort->unlockMutex();
    return true;
  }
  pvSerialPort->unlockMutex();
  return false;
}

bool mdtSerialPortThread::isFinished() const
{
  pvSerialPort->lockMutex();
  if(pvRunning){
    pvSerialPort->unlockMutex();
    return false;
  }
  pvSerialPort->unlockMutex();

  return QThread::isFinished();
}
