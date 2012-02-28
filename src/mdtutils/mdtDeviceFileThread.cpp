
#include "mdtDeviceFileThread.h"
#include "mdtError.h"
#include <QApplication>

mdtDeviceFileThread::mdtDeviceFileThread(QObject *parent)
 : QThread(parent)
{
  pvDeviceFile = 0;
  pvRunning = false;
  pvReadMinWaitTime = 0;
  pvWriteMinWaitTime = 0;
}

mdtDeviceFileThread::~mdtDeviceFileThread()
{
  if(isRunning()){
    stop();
  }
}

void mdtDeviceFileThread::setDeviceFile(mdtDeviceFile *deviceFile)
{
  Q_ASSERT(deviceFile != 0);

  pvDeviceFile = deviceFile;
  pvReadMinWaitTime = pvDeviceFile->config().readMinWaitTime();
  pvWriteMinWaitTime = pvDeviceFile->config().writeMinWaitTime();
}

void mdtDeviceFileThread::start()
{
  Q_ASSERT(pvDeviceFile != 0);

  int i = 0;

  // Start..
  QThread::start();

  // Wait until the thread started
  while(!isRunning()){
    qApp->processEvents();
    msleep(50);
    i++;
    // When something goes wrong on starting, it happens sometimes that isRunning() ruturns false forever
    // We let the thrad 500 [ms] time to start, else generate an error
    if(i >= 10){
      mdtError e(MDT_UNDEFINED_ERROR, "Thread start timeout reached (500 [ms]) -> abort", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtDeviceFileThread");
      e.commit();
      return;
    }
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
