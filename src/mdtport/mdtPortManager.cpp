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
#include "mdtPortManager.h"
#include "mdtError.h"
#include <QTimer>
#include <QApplication>

#include <QDebug>

mdtPortManager::mdtPortManager(QObject *parent)
 : QThread(parent)
{
  pvReadThread = 0;
  pvWriteThread = 0;
  pvConfig = 0;
  pvPort = 0;
}

mdtPortManager::~mdtPortManager()
{
  // Stop threads and close the port
  closePort();
  // Release memory
  if(pvReadThread != 0){
    delete pvReadThread;
    pvReadThread = 0;
  }
  if(pvWriteThread != 0){
    delete pvWriteThread;
    pvWriteThread = 0;
  }
}

void mdtPortManager::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);

  pvPort = port;

  // If thread not exists, create it first
  if(pvReadThread == 0){
    pvReadThread = new mdtPortReadThread;
  }
  if(pvWriteThread == 0){
    pvWriteThread = new mdtPortWriteThread;
  }
  // Assign port to threads
  Q_ASSERT(pvReadThread != 0);
  pvReadThread->setPort(pvPort);
  connect(pvReadThread, SIGNAL(newFrameReaden()), this, SLOT(newFrameReaden()));
  Q_ASSERT(pvWriteThread != 0);
  pvWriteThread->setPort(pvPort);
  // Connect thraed's error signals
  connect(pvReadThread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
  connect(pvWriteThread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
}

bool mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);

  return pvPort->setAttributes(portName);
}

void mdtPortManager::setConfig(mdtPortConfig *config)
{
  Q_ASSERT(config != 0);

  pvConfig = config;
}

mdtPortConfig &mdtPortManager::config()
{
  Q_ASSERT(pvConfig != 0);

  return *pvConfig;
}

bool mdtPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvConfig != 0);

  return pvPort->open(*pvConfig);
}

void mdtPortManager::closePort()
{
  // It can happen that port was never set
  if(pvPort == 0){
    return;
  }
  // If threads exists, call stop methods
  if(pvReadThread != 0){
    stopReading();
  }
  if(pvWriteThread != 0){
    stopWriting();
  }
  stop();
  // Close the port
  pvPort->close();
}

bool mdtPortManager::startReading()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvReadThread != 0);

  return pvReadThread->start();
}

void mdtPortManager::stopReading()
{
  Q_ASSERT(pvReadThread != 0);
  Q_ASSERT(pvPort != 0);

  if(pvReadThread->isRunning()){
    pvReadThread->stop();
  }
}

bool mdtPortManager::startWriting()
{
  Q_ASSERT(pvWriteThread != 0);
  Q_ASSERT(pvPort != 0);

  // Calling this method on read only port is a error
  if(config().readOnly()){
    mdtError e(MDT_PORT_IO_ERROR, "start writing called on a read only port", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    return false;
  }
  return pvWriteThread->start();
}

void mdtPortManager::stopWriting()
{
  Q_ASSERT(pvWriteThread != 0);
  Q_ASSERT(pvPort != 0);

  if(pvWriteThread->isRunning()){
    pvWriteThread->stop();
  }
}

bool mdtPortManager::start()
{
  if(!config().readOnly()){
    if(!startWriting()){
      return false;
    }
  }
  if(!startReading()){
    stopWriting();
    return false;
  }
  return true;
}

bool mdtPortManager::isRunning()
{
  if(pvPort == 0){
    return false;
  }
  if(pvReadThread != 0){
    if(pvReadThread->isRunning()){
      return true;
    }
  }
  if(pvWriteThread != 0){
    if(pvWriteThread->isRunning()){
      return true;
    }
  }
  return false;
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
  pvLastReadenData = pvLastReadenFrame;
  pvLastReadenFrame.clear();

  return pvLastReadenData;
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

mdtPortReadThread *mdtPortManager::readThread()
{
  Q_ASSERT(pvReadThread != 0);

  return pvReadThread;
}

mdtPortWriteThread *mdtPortManager::writeThread()
{
  Q_ASSERT(pvWriteThread != 0);

  return pvWriteThread;
}

void mdtPortManager::newFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = pvPort->readenFrames().dequeue();
    Q_ASSERT(frame != 0);
    // Copy data and put frame back to pool
    pvLastReadenFrame.append(frame->data(), frame->size());
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  emit(newDataReaden());
}

void mdtPortManager::onThreadsErrorOccured(int error)
{
  // On IO error, we try to re-open the port
  if(error == MDT_PORT_IO_ERROR){
    qDebug() << "I/O error !";
    closePort();
    if(!openPort()){
      return;
    }
    start();
  }
  if(error == MDT_PORT_QUEUE_EMPY_ERROR){
    qDebug() << "Queue empty !";
  }
}
