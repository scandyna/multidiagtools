
#include "mdtPortThread.h"
#include "mdtError.h"
#include <QApplication>

mdtPortThread::mdtPortThread(QObject *parent)
 : QThread(parent)
{
  pvPort = 0;
  pvRunning = false;
  pvReadMinWaitTime = 0;
  pvWriteMinWaitTime = 0;
}

mdtPortThread::~mdtPortThread()
{
  if(isRunning()){
    stop();
  }
}

void mdtPortThread::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);

  pvPort = port;
  pvReadMinWaitTime = pvPort->config().readMinWaitTime();
  pvWriteMinWaitTime = pvPort->config().writeMinWaitTime();
  pvBytePerByteWrite = pvPort->config().bytePerByteWrite();
}

bool mdtPortThread::start()
{
  Q_ASSERT(pvPort != 0);

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
      MDT_ERROR_SET_SRC(e, "mdtPortThread");
      e.commit();
      return false;
    }
  }

  return true;
}

void mdtPortThread::stop()
{
  Q_ASSERT(pvPort != 0);

  // Unset the running flag
  pvPort->lockMutex();
  pvRunning = false;
  pvPort->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}

bool mdtPortThread::isRunning() const
{
  if(!QThread::isRunning()){
    return false;
  }
  pvPort->lockMutex();
  if(pvRunning){
    pvPort->unlockMutex();
    return true;
  }
  pvPort->unlockMutex();
  return false;
}

bool mdtPortThread::isFinished() const
{
  pvPort->lockMutex();
  if(pvRunning){
    pvPort->unlockMutex();
    return false;
  }
  pvPort->unlockMutex();

  return QThread::isFinished();
}
