
#include "mdtDeviceFileThread.h"
#include <QApplication>

mdtDeviceFileThread::mdtDeviceFileThread(QObject *parent)
 : QThread(parent)
{
  pvDeviceFile = 0;
  pvRunning = false;
}

void mdtDeviceFileThread::setDeviceFile(mdtDeviceFile *deviceFile)
{
  Q_ASSERT(deviceFile != 0);

  pvDeviceFile = deviceFile;
}

void mdtDeviceFileThread::start()
{
  Q_ASSERT(pvDeviceFile != 0);

  // Start..
  QThread::start();
  // Wait until the thread started
  while(!isRunning()){
    qApp->processEvents();
    msleep(50);
  }
}

void mdtDeviceFileThread::stop()
{
  Q_ASSERT(pvDeviceFile != 0);

  // Unset the running flag
  pvDeviceFile->lockMutex();
  pvRunning = false;
  pvDeviceFile->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}

bool mdtDeviceFileThread::isRunning() const
{
  if(!QThread::isRunning()){
    return false;
  }
  pvDeviceFile->lockMutex();
  if(pvRunning){
    pvDeviceFile->unlockMutex();
    return true;
  }
  pvDeviceFile->unlockMutex();
  return false;
}

bool mdtDeviceFileThread::isFinished() const
{
  pvDeviceFile->lockMutex();
  if(pvRunning){
    pvDeviceFile->unlockMutex();
    return false;
  }
  pvDeviceFile->unlockMutex();

  return QThread::isFinished();
}
