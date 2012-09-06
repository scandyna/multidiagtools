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

#include "mdtPortThread.h"

#include <QDebug>

mdtPortManager::mdtPortManager(QObject *parent)
 : QThread(parent)
{
  pvPort = 0;
}

mdtPortManager::~mdtPortManager()
{
  qDebug() << "mdtPortManager::~mdtPortManager() ...";
  if(pvPort != 0){
    if(isRunning()){
      stop();
    }
    if(pvPort->isOpen()){
      pvPort->close();
    }
  }
  qDebug() << "mdtPortManager::~mdtPortManager() port closed";

  qDebug() << "mdtPortManager::~mdtPortManager() END";
}

QStringList mdtPortManager::scan()
{
  Q_ASSERT(!isRunning());

  return QStringList();
}

void mdtPortManager::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);
  Q_ASSERT(!isRunning());

  pvPort = port;
}

void mdtPortManager::detachPort(bool deletePort, bool deleteThreads)
{
  if(pvPort == 0){
    return;
  }

  if(isRunning()){
    stop();
  }
  // Detach each thread
  removeThreads(deleteThreads);
  // Close and detach port
  if(pvPort->isOpen()){
    pvPort->close();
  }
  if(deletePort){
    delete pvPort;
  }
  pvPort = 0;
}

void mdtPortManager::addThread(mdtPortThread *thread)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(!isRunning());
  Q_ASSERT(thread != 0);

  // Assign port to thread
  thread->setPort(pvPort);
  connect(thread, SIGNAL(newFrameReaden()), this, SLOT(newFrameReaden()));
  connect(thread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
  // Add thread to list
  pvThreads.append(thread);
}

void mdtPortManager::removeThreads(bool releaseMemory)
{
  Q_ASSERT(!isRunning());

  int i;
  mdtPortThread *thread;

  for(i=0; i<pvThreads.size(); i++){
    thread = pvThreads.at(i);
    disconnect(thread, SIGNAL(newFrameReaden()), this, SLOT(newFrameReaden()));
    disconnect(thread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
    if(releaseMemory){
      delete thread;
    }
  }
  pvThreads.clear();
}

bool mdtPortManager::start()
{
  Q_ASSERT(pvPort != 0);

  int i;

  for(i=0; i<pvThreads.size(); i++){
    if(!pvThreads.at(i)->start()){
      return false;
    }
  }

  return true;
}

bool mdtPortManager::isRunning()
{
  int i;

  if(pvPort == 0){
    return false;
  }

  for(i=0; i<pvThreads.size(); i++){
    if(pvThreads.at(i)->isRunning()){
      return true;
    }
  }

  return false;
}

void mdtPortManager::stop()
{
  int i;

  for(i=0; i<pvThreads.size(); i++){
    if(pvThreads.at(i)->isRunning()){
      pvThreads.at(i)->stop();
    }
  }
}

void mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);

  pvPort->setPortName(portName);
}

mdtPortConfig &mdtPortManager::config()
{
  Q_ASSERT(pvPort != 0);

  return pvPort->config();
}

bool mdtPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);

  if(pvPort->setup() != mdtAbstractPort::NoError){
    return false;
  }

  return true;
}

void mdtPortManager::closePort()
{
  // It can happen that port was never set
  if(pvPort == 0){
    return;
  }
  stop();
  // Close the port
  pvPort->close();
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
  frame->clearSub();
  // Store data and add frame to write queue
  frame->append(data);
  pvPort->unlockMutex();
  pvPort->addFrameToWrite(frame);

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
  if(error == MDT_PORT_QUEUE_EMPTY_ERROR){
    qDebug() << "Queue empty !";
  }
}
