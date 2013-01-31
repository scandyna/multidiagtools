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
  qRegisterMetaType<mdtPortTransaction>("mdtPortTransaction");

  pvPort = 0;
  pvReadThread = 0;
  pvWriteThread = 0;
  // At default, transactions support and blocking mode are OFF
  setTransactionsDisabled(false);
}

mdtPortManager::~mdtPortManager()
{
  if(pvPort != 0){
    if(isRunning()){
      stop();
    }
    if(pvPort->isOpen()){
      pvPort->close();
    }
  }
  qDeleteAll(pvTransactionsPool);
  pvTransactionsPool.clear();
  qDeleteAll(pvTransactionsPending);
  pvTransactionsPending.clear();
  qDeleteAll(pvTransactionsDone);
  pvTransactionsDone.clear();
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
  qDebug() << "mdtPortManager::detachPort() ...";
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
  qDebug() << "mdtPortManager::detachPort() DONE";
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
    pvReadThread = thread;
  }
  if(thread->isWriter()){
    pvWriteThread = thread;
  }
  connect(thread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
  // Add thread to list
  pvThreads.append(thread);
}

mdtPortThread *mdtPortManager::readThread()
{
  return pvReadThread;
}

mdtPortThread *mdtPortManager::writeThread()
{
  return pvWriteThread;
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
  pvReadThread = 0;
  pvWriteThread = 0;
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

  qDebug() << "mdtPortManager::stop() ...";
  for(i=0; i<pvThreads.size(); i++){
    if(pvThreads.at(i)->isRunning()){
      pvThreads.at(i)->stop();
    }
  }
  qDebug() << "mdtPortManager::stop() DONE";
}

void mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);

  pvPort->setPortName(portName);
}

void mdtPortManager::setPortInfo(mdtPortInfo info)
{
  pvPortInfo = info;
  setPortName(pvPortInfo.portName());
}

mdtPortInfo mdtPortManager::portInfo()
{
  return pvPortInfo;
}

mdtPortConfig &mdtPortManager::config()
{
  Q_ASSERT(pvPort != 0);

  return pvPort->config();
}

mdtPortConfig &mdtPortManager::config() const
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
  // Initialize flags
  pvCancelReadWait = false;

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
  // Remove pending transactions
  qDeleteAll(pvTransactionsPending);
  pvTransactionsPending.clear();
}

void mdtPortManager::setTransactionsEnabled()
{
  Q_ASSERT(!isRunning());

  pvEnqueueAllReadenFrames = false;
  pvTransactionsEnabled = true;
}

void mdtPortManager::setTransactionsDisabled(bool enqueueIncommingFrames)
{
  Q_ASSERT(!isRunning());

  pvEnqueueAllReadenFrames = enqueueIncommingFrames;
  pvTransactionsEnabled = false;
}

mdtPortTransaction *mdtPortManager::getNewTransaction()
{
  mdtPortTransaction *transaction;

  if(pvTransactionsPool.size() < 1){
    transaction = new mdtPortTransaction;
  }else{
    transaction = pvTransactionsPool.dequeue();
  }
  Q_ASSERT(transaction != 0);
  transaction->clear();

  return transaction;
}

bool mdtPortManager::waitOnWriteReady(int timeout, int granularity)
{
  Q_ASSERT(granularity > 0);
  Q_ASSERT(pvPort != 0);

  int i;
  int maxIter;

  if(timeout == 0){
    timeout = adjustedWriteTimeout(timeout, false);
  }else{
    timeout = adjustedWriteTimeout(timeout);
  }

  if(timeout < 0){  // Case of infinite timeout
    while(1){
      // Check if a frame is available
      pvPort->lockMutex();
      if(pvPort->writeFramesPool().size() > 0){
        pvPort->unlockMutex();
        return true;
      }
      pvPort->unlockMutex();
      msleep(granularity);
      qApp->processEvents();
    }
  }else{
    maxIter = timeout / granularity;
    for(i=0; i<maxIter; i++){
      // Check if a frame is available
      pvPort->lockMutex();
      if(pvPort->writeFramesPool().size() > 0){
        pvPort->unlockMutex();
        return true;
      }
      pvPort->unlockMutex();
      msleep(granularity);
      qApp->processEvents();
    }
  }

  return false;
}

int mdtPortManager::writeData(QByteArray data)
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
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = pvPort->writeFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->append(data);
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return 0;
}

bool mdtPortManager::waitReadenFrame(int timeout)
{
  int maxIter;

  if(timeout == 0){
    timeout = adjustedReadTimeout(timeout, false);
  }else{
    timeout = adjustedReadTimeout(timeout);
  }

  if(pvTransactionsEnabled){
    if(timeout < 0){  // Case of infinite timeout
      while(pvTransactionsDone.size() < 1){
        if(readWaitCanceled()){
          return false;
        }
        qApp->processEvents();
        msleep(50);
      }
    }else{
      maxIter = timeout / 50;
      while(pvTransactionsDone.size() < 1){
        if(readWaitCanceled()){
          return false;
        }
        if(maxIter <= 0){
          return false;
        }
        qApp->processEvents();
        msleep(50);
        maxIter--;
      }
    }
  }else{
    if(timeout < 0){  // Case of infinite timeout
      while(pvReadenFrames.size() < 1){
        if(readWaitCanceled()){
          return false;
        }
        qApp->processEvents();
        msleep(50);
      }
    }else{
      maxIter = timeout / 50;
      while(pvReadenFrames.size() < 1){
        if(readWaitCanceled()){
          return false;
        }
        if(maxIter <= 0){
          return false;
        }
        qApp->processEvents();
        msleep(50);
        maxIter--;
      }
    }
  }

  return true;
}

bool mdtPortManager::waitOnFrame(int id, int timeout, int granularity)
{
  Q_ASSERT(granularity > 0);

  int maxIter;

  if(timeout == 0){
    timeout = adjustedReadTimeout(timeout, false);
  }else{
    timeout = adjustedReadTimeout(timeout);
  }

  // Check if transaction exists in pending queue
  // Note: we check also done queue, because it's possible that transaction was done
  if((!pvTransactionsPending.contains(id))&&(!pvTransactionsDone.contains(id))){
    mdtError e(MDT_PORT_IO_ERROR, "Wait on a frame that was never added to pending queue, id: " + QString::number(id), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    return false;
  }
  // Try until success or timeout
  if(timeout < 0){  // Case of infinite timeout
    while(!pvTransactionsDone.contains(id)){
      if(readWaitCanceled()){
        return false;
      }
      // Wait
      qApp->processEvents();
      msleep(granularity);
    }
  }else{
    maxIter = timeout / granularity;
    while(!pvTransactionsDone.contains(id)){
      if(readWaitCanceled()){
        return false;
      }
      // Check about timeout
      if(maxIter <= 0){
        return false;
      }
      // Wait
      qApp->processEvents();
      msleep(granularity);
      maxIter--;
    }
  }

  return true;
}

QByteArray mdtPortManager::readenFrame(int id)
{
  mdtPortTransaction *transaction;

  transaction = pvTransactionsDone.take(id);
  if(transaction == 0){
    return QByteArray();
  }
  pvTransactionsPool.enqueue(transaction);

  return transaction->data();
}

QList<QByteArray> mdtPortManager::readenFrames()
{
  QList<QByteArray> frames;

  if(pvTransactionsEnabled){
    QMapIterator<int, mdtPortTransaction*> it(pvTransactionsDone);
    mdtPortTransaction *transaction;

    while(it.hasNext()){
      it.next();
      transaction = it.value();
      Q_ASSERT(transaction != 0);
      Q_ASSERT(transaction->id() == it.key());
      frames.append(transaction->data());
      pvTransactionsPool.enqueue(transaction);
    }
    pvTransactionsDone.clear();
  }else{
    frames = pvReadenFrames;
    pvReadenFrames.clear();
  }

  return frames;
}

int mdtPortManager::adjustedReadTimeout(int requestedTimeout, bool warn) const
{
  int minTimeout = config().readTimeout();

  if(minTimeout == -1){ // Case of infinite timeout
    if(requestedTimeout != -1){
      if(warn){
        mdtError e(MDT_PORT_IO_ERROR, "Requested read timeout is to small: " + \
                    QString::number(requestedTimeout) + " , adjusted to infinite", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtPortManager");
        e.commit();
      }
      return minTimeout;
    }
  }else{
    minTimeout += 1000;
    if(requestedTimeout < minTimeout){
      if(warn){
        mdtError e(MDT_PORT_IO_ERROR, "Requested read timeout is to small: " + \
                    QString::number(requestedTimeout) + " , adjusted to " + QString::number(minTimeout) + " [ms]", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtPortManager");
        e.commit();
      }
      return minTimeout;
    }
  }
  return requestedTimeout;
}

int mdtPortManager::adjustedWriteTimeout(int requestedTimeout, bool warn) const
{
  int minTimeout = config().writeTimeout();

  if(minTimeout == -1){ // Case of infinite timeout
    if(requestedTimeout != -1){
      if(warn){
        mdtError e(MDT_PORT_IO_ERROR, "Requested write timeout is to small: " + \
                    QString::number(requestedTimeout) + " , adjusted to infinite", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtPortManager");
        e.commit();
      }
      return minTimeout;
    }
  }else{
    minTimeout += 1000;
    if(requestedTimeout < minTimeout){
      if(warn){
        mdtError e(MDT_PORT_IO_ERROR, "Requested write timeout is to small: " + \
                    QString::number(requestedTimeout) + " , adjusted to " + QString::number(minTimeout) + " [ms]", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtPortManager");
        e.commit();
      }
      return minTimeout;
    }
  }
  return requestedTimeout;
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

void mdtPortManager::flushIn(bool flushPortManagerBuffers, bool flushPortBuffers)
{
  Q_ASSERT(pvPort != 0);

  // Flush port manager buffers
  if(flushPortManagerBuffers){
    if(pvTransactionsEnabled){
      QMutableMapIterator<int, mdtPortTransaction*> itPending(pvTransactionsPending);
      QMutableMapIterator<int, mdtPortTransaction*> itRx(pvTransactionsRx);
      QMutableMapIterator<int, mdtPortTransaction*> itDone(pvTransactionsDone);

      // Clear pending transactions
      while(itPending.hasNext()){
        itPending.next();
        pvTransactionsPool.enqueue(itPending.value());
        itPending.remove();
      }
      // Clear Rx transactions
      while(itRx.hasNext()){
        itRx.next();
        pvTransactionsPool.enqueue(itRx.value());
        itRx.remove();
      }
      // Clear Done transactions
      while(itDone.hasNext()){
        itDone.next();
        pvTransactionsPool.enqueue(itDone.value());
        itDone.remove();
      }
    }
    pvReadenFrames.clear();
  }
  // Flush port's buffers
  if(flushPortBuffers){
    pvPort->flushIn();
  }
}

void mdtPortManager::flushIn()
{
  flushIn(true, true);
}

void mdtPortManager::flushOut(bool flushPortManagerBuffers, bool flushPortBuffers)
{
  if(flushPortBuffers){
    pvPort->flushOut();
  }
}

void mdtPortManager::flushOut()
{
  flushOut(true, true);
}

void mdtPortManager::flush(bool flushPortManagerBuffers, bool flushPortBuffers)
{
  Q_ASSERT(pvPort != 0);

  flushIn(flushPortManagerBuffers, false);
  flushOut(flushPortManagerBuffers, false);
  if(flushPortBuffers){
    pvPort->flush();
  }
}

void mdtPortManager::flush()
{
  flush(true, true);
}

void mdtPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

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
      pvReadenFrames.enqueue(data);
    }
    // Restore frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  // Commit
  commitFrames();
}

/// \todo Error handling (in general ...)
/// \todo On disconnect, should flush I/O ?
void mdtPortManager::onThreadsErrorOccured(int error)
{
  bool ok;
  int maxTry = 5;

  qDebug() << "mdtPortManager::onThreadsErrorOccured() , code: " << error;
  emit(errorStateChanged(error));

  // Try to handle error

  // On IO error, we try to re-open the port
}

void mdtPortManager::restoreTransaction(mdtPortTransaction *transaction)
{
  pvTransactionsPool.enqueue(transaction);
}

void mdtPortManager::addTransaction(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);

  pvTransactionsPending.insert(transaction->id(), transaction);
  // Watch transactions size
  if(pvTransactionsPending.size() > 20){
    mdtError e(MDT_PORT_IO_ERROR, "Pending transactions queue has more than 20 items, will clear it (this is a bug)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    qDeleteAll(pvTransactionsPending);
    pvTransactionsPending.clear();
  }
}

void mdtPortManager::addTransaction(int id, bool queryReplyMode)
{
  mdtPortTransaction *transaction;

  // Get a transaction
  transaction = getNewTransaction();
  // Setup and enqueue transaction
  transaction->setId(id);
  transaction->setQueryReplyMode(queryReplyMode);
  addTransaction(transaction);
}

mdtPortTransaction *mdtPortManager::pendingTransaction(int id)
{
  // QMap returns a default-constructed value if key not exists (i.e. 0 for pointer)
  return pvTransactionsPending.take(id);
}

void mdtPortManager::removeTransactionDone(int id)
{
  mdtPortTransaction *transaction;

  transaction = pvTransactionsDone.take(id);
  if(transaction != 0){
    pvTransactionsPool.enqueue(transaction);
  }
}

void mdtPortManager::enqueueTransactionDone(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);

  pvTransactionsDone.insert(transaction->id(), transaction);
}

void mdtPortManager::enqueueTransactionRx(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);

  pvTransactionsRx.insert(transaction->id(), transaction);
}


void mdtPortManager::commitFrames()
{
  if(pvTransactionsEnabled){
    QMutableMapIterator<int, mdtPortTransaction*> it(pvTransactionsRx);
    mdtPortTransaction *transaction;

    while(it.hasNext()){
      it.next();
      transaction = it.value();
      Q_ASSERT(transaction != 0);
      Q_ASSERT(transaction->id() == it.key());
      emit(newReadenFrame(*transaction));
      if(transaction->isQueryReplyMode()){
        qDebug() << "mdtPortManager::commitFrames(): frame is Q/R mode";
        enqueueTransactionDone(transaction);
      }else{
        pvTransactionsPool.enqueue(transaction);
      }
      it.remove();
    }
    // Watch transactions done size
    if(pvTransactionsDone.size() > 200){
      mdtError e(MDT_PORT_IO_ERROR, "Transactions done queue has more than 200 items, will clear it (this is a bug)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortManager");
      e.commit();
      qDeleteAll(pvTransactionsDone);
      pvTransactionsDone.clear();
    }
  }else{
    int i;

    for(i=0; i<pvReadenFrames.size(); i++){
      emit(newReadenFrame(pvReadenFrames.at(i)));
    }
    if(!pvEnqueueAllReadenFrames){
      pvReadenFrames.clear();
    }
    // Watch readen frames size
    if(pvReadenFrames.size() > 200){
      mdtError e(MDT_PORT_IO_ERROR, "Readen frames queue has more than 200 items, will clear it (this is a bug)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortManager");
      e.commit();
      pvReadenFrames.clear();
    }
  }
}

void mdtPortManager::cancelReadWait()
{
  pvCancelReadWait = true;
}

bool mdtPortManager::readWaitCanceled()
{
  if(pvCancelReadWait){
    pvCancelReadWait = false;
    return true;
  }
  return false;
}
