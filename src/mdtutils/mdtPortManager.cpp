
#include "mdtPortManager.h"
#include "mdtError.h"
#include <QTimer>
#include <QApplication>

#include <QDebug>

mdtPortManager::mdtPortManager(QObject *parent)
 : QThread(parent)
{
  // Create port and threads
  pvPort = new mdtPort;
  Q_ASSERT(pvPort != 0);
  // Create threads and associate port
  pvReadThread = new mdtPortReadThread;
  Q_ASSERT(pvReadThread != 0);
  pvReadThread->setPort(pvPort);
  connect(pvReadThread, SIGNAL(newFrameReaden()), this, SLOT(newFrameReaden()));
  pvWriteThread = new mdtPortWriteThread;
  Q_ASSERT(pvWriteThread != 0);
  pvWriteThread->setPort(pvPort);
}

mdtPortManager::~mdtPortManager()
{
  // Stop threads and close the port
  closePort();
  // Release memory
  if(pvPort != 0){
    delete pvPort;
    pvPort = 0;
  }
  if(pvReadThread != 0){
    delete pvReadThread;
    pvReadThread = 0;
  }
  if(pvWriteThread != 0){
    delete pvWriteThread;
    pvWriteThread = 0;
  }
}

bool mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);

  return pvPort->setAttributes(portName);
}

bool mdtPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);

  return pvPort->open(pvConfig);
}

void mdtPortManager::closePort()
{
  Q_ASSERT(pvPort != 0);

  stop();
  pvPort->close();
}

bool mdtPortManager::startReading()
{
  Q_ASSERT(pvReadThread != 0);
  Q_ASSERT(pvPort != 0);

  return pvReadThread->start();
}

void mdtPortManager::stopReading()
{
  Q_ASSERT(pvReadThread != 0);

  if(pvReadThread->isRunning()){
    pvReadThread->stop();
  }
}

bool mdtPortManager::startWriting()
{
  Q_ASSERT(pvWriteThread != 0);
  Q_ASSERT(pvPort != 0);

  return pvWriteThread->start();
}

void mdtPortManager::stopWriting()
{
  Q_ASSERT(pvWriteThread != 0);

  if(pvWriteThread->isRunning()){
    pvWriteThread->stop();
  }
}

bool mdtPortManager::start()
{
  if(!startWriting()){
    return false;
  }
  if(!startReading()){
    stopWriting();
    return false;
  }
  return true;
}

void mdtPortManager::stop()
{
  stopWriting();
  stopReading();
}

bool mdtPortManager::writeData(QByteArray data)
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Clear previous readen frame
  pvLastReadenFrame.clear();
  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    return false;
  }
  frame = pvPort->writeFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  // Store data and add frame to write queue
  frame->append(data);
  pvPort->writeFrames().enqueue(frame);
  pvPort->unlockMutex();

  return true;
}

QByteArray &mdtPortManager::lastReadenFrame()
{
  return pvLastReadenFrame;
}

bool mdtPortManager::waitReadenFrame(int timeout)
{
  int maxIter = timeout / 50;

  while(pvLastReadenFrame.size() < 1){
    if(maxIter <= 0){
      return false;
    }
    qApp->processEvents();
    msleep(50);
    maxIter--;
  }

  return true;
}

void mdtPortManager::newFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Get a frame in readen queue
  pvPort->lockMutex();
  Q_ASSERT(pvPort->readenFrames().size() > 0);
  frame = pvPort->readenFrames().dequeue();
  Q_ASSERT(frame != 0);
  // Copy data and put frame back to pool
  pvLastReadenFrame.append(frame->data(), frame->size());
  pvPort->readFramesPool().enqueue(frame);
  pvPort->unlockMutex();
}
