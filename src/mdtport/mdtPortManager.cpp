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
  qDebug() << "mdtPortManager::~mdtPortManager() END";
}

QList<mdtPortInfo*> mdtPortManager::scan()
{
  Q_ASSERT(!isRunning());

  return QList<mdtPortInfo*>();
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
  if(thread->isReader()){
    connect(thread, SIGNAL(newFrameReaden()), this, SLOT(fromThreadNewFrameReaden()));
  }
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
    if(thread->isReader()){
      disconnect(thread, SIGNAL(newFrameReaden()), this, SLOT(fromThreadNewFrameReaden()));
    }
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
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return true;
}

bool mdtPortManager::waitReadenFrame(int timeout)
{
  int maxIter = timeout / 50;

  while(pvReadenFrames.size() < 1){
    if(maxIter <= 0){
      return false;
    }
    qApp->processEvents();
    msleep(50);
    maxIter--;
  }

  return true;
}

QList<QByteArray> &mdtPortManager::readenFrames()
{
  int i;

  for(i=0; i<pvReadenFrames.size(); ++i){
    pvReadenFramesCopy.append(pvReadenFrames.takeFirst());
  }

  return pvReadenFramesCopy;
}

void mdtPortManager::wait(int msecs, int granularity)
{
  Q_ASSERT(granularity > 0);

  int i;
  int maxIter = msecs / granularity;

  for(i=0; i<maxIter; i++){
    msleep(granularity);
    qApp->processEvents();
  }
}

void mdtPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

  ///pvPort->flushIn();
  ///pvPort->flushOut();
  pvPort->flush();
}

void mdtPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = pvPort->readenFrames().dequeue();
    Q_ASSERT(frame != 0);
    // Copy data
    /// \todo Error on incomplete frame
    if(frame->isComplete()){
      QByteArray data;
      data.append(frame->data(), frame->size());
      pvReadenFrames.append(data);
    }
    // Restore frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  emit(newReadenFrame());
}

/// \todo Error handling (in general ...)
void mdtPortManager::onThreadsErrorOccured(int error)
{
  bool ok;
  int maxTry = 5;

  qDebug() << "mdtPortManager::onThreadsErrorOccured() , code: " << error;
  
  // Try to handle error
  if(error == mdtAbstractPort::Disconnected){
    qDebug() << "Disconnected , closing port ...";
    closePort();
    // Try re-open port
    ok = false;
    while(!ok){
      wait(5000);
      qDebug() << "Try to re-open ...";
      ok = openPort();
      maxTry--;
      if(maxTry <= 0){
        qDebug() << "Re-open failed";
        return;
      }
    }
    qDebug() << "Open OK, starting ...";
    if(!start()){
      qDebug() << "start failed :-(";
      return;
    }
  }
  
  // On IO error, we try to re-open the port
  /**
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
  */
}
