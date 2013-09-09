/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtPortManagerStateMachine.h"
#include "mdtError.h"
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QMutableListIterator>

#include "mdtPortThread.h"

#include <QDebug>

mdtPortManager::mdtPortManager(QObject *parent)
 : QThread(parent)
{
  qRegisterMetaType<mdtPortTransaction>("mdtPortTransaction");

  pvPort = 0;
  pvReadThread = 0;
  pvWriteThread = 0;
  pvThreadHandlesReadTimeout = false;
  pvTransactionsAllocatedCount = 0;
  pvPortMutexLocked = false;
  pvTransactionTimeoutOccured = false;
  pvTransactionTimer = new QTimer(this);
  pvTransactionTimer->setSingleShot(true);
  connect(pvTransactionTimer, SIGNAL(timeout()), this, SLOT(onTransactionTimeoutOccured()));
  connect(this, SIGNAL(pmTransactionTimeoutEvent()), this, SLOT(onTransactionTimeoutOccured()));
  // As default, we not keep all incoming frames
  setKeepTransactionsDone(false);
  // Setup state machine
  pvStateMachine = new mdtPortManagerStateMachine(this);
  pvStateMachine->buildMainStateMachine(this);
}

mdtPortManager::~mdtPortManager()
{
  if(pvPort != 0){
    stop();
    /**
    if(isRunning()){
      stop();
    }
    if(pvPort->isOpen()){
      pvPort->close();
    }
    */
  }
  qDeleteAll(pvTransactionsPool);
  ///delete pvStateMachine;
}

QList<mdtPortInfo*> mdtPortManager::scan()
{
  return QList<mdtPortInfo*>();
}

void mdtPortManager::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);
  ///Q_ASSERT(!isRunning());
  Q_ASSERT(isClosed());

  pvPort = port;
}

void mdtPortManager::lockPortMutex()
{
  Q_ASSERT(pvPort != 0);

  if(pvPortMutexLocked){
    mdtError e(MDT_PORT_IO_ERROR, "Trying to lock port mutex from main thread while it is allready locked (from main thread)", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    return;
  }
  pvPortMutexLocked = true;
  pvPort->lockMutex();
}

void mdtPortManager::unlockPortMutex()
{
  Q_ASSERT(pvPort != 0);

  pvPort->unlockMutex();
  pvPortMutexLocked = false;
}

void mdtPortManager::detachPort(bool deletePort, bool deleteThreads)
{
  qDebug() << "mdtPortManager::detachPort() ...";
  if(pvPort == 0){
    return;
  }

  ///if(isRunning()){
  if(!isClosed()){
    stop();
  }
  // Detach each thread
  removeThreads(deleteThreads);
  // Detach port
  /**
  if(pvPort->isOpen()){
    pvPort->close();
  }
  */
  if(deletePort){
    delete pvPort;
  }
  pvPort = 0;
  qDebug() << "mdtPortManager::detachPort() DONE";
}

void mdtPortManager::addThread(mdtPortThread *thread)
{
  Q_ASSERT(pvPort != 0);
  ///Q_ASSERT(!isRunning());
  Q_ASSERT(isClosed());
  Q_ASSERT(thread != 0);

  // Assign port to thread
  thread->setPort(pvPort);
  if(thread->isReader()){
    connect(thread, SIGNAL(newFrameReaden()), this, SLOT(fromThreadNewFrameReaden()));
    pvReadThread = thread;
    pvThreadHandlesReadTimeout = pvReadThread->handlesTimeout();
  }
  if(thread->isWriter()){
    pvWriteThread = thread;
  }
  connect(thread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
  connect(thread, SIGNAL(ready(mdtPortThread*)), this, SLOT(onThreadReady(mdtPortThread*)));
  connect(thread, SIGNAL(finished(mdtPortThread*)), this, SLOT(onThreadStopped(mdtPortThread*)));
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
  ///Q_ASSERT(!isRunning());
  Q_ASSERT(isClosed());
  Q_ASSERT(pvThreadsReady.isEmpty());

  int i;
  mdtPortThread *thread;

  for(i=0; i<pvThreads.size(); i++){
    thread = pvThreads.at(i);
    if(thread->isReader()){
      disconnect(thread, SIGNAL(newFrameReaden()), this, SLOT(fromThreadNewFrameReaden()));
    }
    disconnect(thread, SIGNAL(errorOccured(int)), this, SLOT(onThreadsErrorOccured(int)));
    disconnect(thread, SIGNAL(ready(mdtPortThread*)), this, SLOT(onThreadReady(mdtPortThread*)));
    disconnect(thread, SIGNAL(finished(mdtPortThread*)), this, SLOT(onThreadStopped(mdtPortThread*)));
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

  qDebug() << "mdtPortManager::start() ...";
  ///int i;

  pvTransactionTimeoutOccured = false;
  // Open port
  if(!openPort()){
    return false;
  }
  // We emit the pmStartThreadsEvent (see mdtPortManagerStateMachine for details)
  emit pmStartThreadsEvent();
  // Wait until we are ready
  while(!isReady()){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  Q_ASSERT(pvPort->isOpen());
  qDebug() << "mdtPortManager::start() DONE";

  return true;
}

/**
bool mdtPortManager::isRunning()
{
  int i;

  if(pvPort == 0){
    return false;
  }
  if(pvThreads.size() < 1){
    return false;
  }
  for(i=0; i<pvThreads.size(); i++){
    if(!pvThreads.at(i)->isRunning()){
      return false;
    }
  }

  return true;
}
*/

void mdtPortManager::stop()
{
  Q_ASSERT(pvStateMachine != 0);
  ///int i;

  stopTransactionTimer();
  // We emit the pmStopThreadsEvent (see mdtPortManagerStateMachine for details)
  emit pmStopThreadsEvent();
  // Wait until all threads are stopped
  pvStateMachine->waitOnState(Stopped);
  // Close port
  closePort();
  emit pmPortClosedEvent();
  pvStateMachine->waitOnState(PortClosed);
  Q_ASSERT(isClosed());
  Q_ASSERT(!pvPort->isOpen());

  qDebug() << "mdtPortManager::stop() DONE";

  /**
  qDebug() << "mdtPortManager::stop() ...";
  for(i=0; i<pvThreads.size(); i++){
    if(pvThreads.at(i)->isRunning()){
      qDebug() << "mdtPortManager::stop() stopping thread ...";
      pvThreads.at(i)->stop();
      qDebug() << "mdtPortManager::stop() stopping thread DONE";
    }
  }
  // Threads have emited the Disconnected error.
  //  We process events to be shure to receive it now(and not later, after a restart..)
  qApp->processEvents();
  qDebug() << "mdtPortManager::stop() DONE";
  */
}

void mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(isClosed());

  pvPortInfo = mdtPortInfo();
  pvPort->setPortName(portName);
}

void mdtPortManager::setPortInfo(mdtPortInfo info)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(isClosed());

  pvPortInfo = info;
  pvPort->setPortName(pvPortInfo.portName());
  /**
  qDebug() << "mdtPortManager::setPortInfo() : port name: " << pvPortInfo.portName();
  setPortName(pvPortInfo.portName());
  */
}

mdtPortInfo mdtPortManager::portInfo()
{
  return pvPortInfo;
}

QString mdtPortManager::portName() const
{
  return pvPortInfo.portName();
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

void mdtPortManager::setKeepTransactionsDone(bool keep)
{
  pvKeepTransactionsDone = keep;
}

bool mdtPortManager::keepTransactionsDone() const
{
  return pvKeepTransactionsDone;
}

mdtPortTransaction *mdtPortManager::getNewTransaction()
{
  mdtPortTransaction *transaction;

  if(pvTransactionsPool.size() < 1){
    transaction = new mdtPortTransaction;
    pvTransactionsAllocatedCount++;
    // Check about memory leaks
    if((pvTransactionsAllocatedCount - pvTransactionsPool.size() - pvTransactionsPending.size() - pvTransactionsDone.size()) > 10){
      mdtError e(MDT_PORT_IO_ERROR, "Transactions management produces memory leakage", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtPortManager");
      e.commit();
    }
  }else{
    transaction = pvTransactionsPool.dequeue();
  }
  Q_ASSERT(transaction != 0);
  transaction->clear();

  return transaction;
}

void mdtPortManager::restoreTransaction(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!pvTransactionsPool.contains(transaction));

  pvTransactionsPool.enqueue(transaction);
}

/// \todo Remove timeout parameter
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
      // Check state first
      ///if(pvCurrentState == Ready){
      if(currentState() == Ready){
        // Check if a frame is available
        lockPortMutex();
        if(pvPort->writeFramesPool().size() > 0){
          unlockPortMutex();
          return true;
        }
        unlockPortMutex();
      }
      /// \todo Adapter
      msleep(granularity);
      qApp->processEvents();
    }
  }else{
    maxIter = timeout / granularity;
    for(i=0; i<maxIter; ++i){
      // Check state first
      ///if(pvCurrentState == Ready){
      if(currentState() == Ready){
        // Check if a frame is available
        lockPortMutex();
        if(pvPort->writeFramesPool().size() > 0){
          unlockPortMutex();
          return true;
        }
        unlockPortMutex();
      }
      /// \todo Adapter
      msleep(granularity);
      qApp->processEvents();
    }
  }
  ///emit(busy());

  return false;
}

int mdtPortManager::sendData(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!pvTransactionsPending.contains(transaction->id()));
  Q_ASSERT(!transactionsDoneContains(transaction->id()));

  mdtFrame *frame = 0;

  // Wait until we can write
  while(1){
    // If port manager was stopped, we return
    if(isClosed()){
      restoreTransaction(transaction);
      return mdtAbstractPort::WriteCanceled;
    }
    if(isReady()){
      lockPortMutex();
      if(pvPort->writeFramesPool().size() > 0){
        frame = pvPort->writeFramesPool().dequeue();
        Q_ASSERT(frame != 0);
        // Here we are ready to write - we keep port mutext locked
        break;
      }
      unlockPortMutex();
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  // We are ready to write
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->append(transaction->data());
  pvPort->addFrameToWrite(frame);
  addTransactionPending(transaction);
  unlockPortMutex();

  return transaction->id();
}

int mdtPortManager::sendData(const QByteArray &data, bool queryReplyMode)
{
  Q_ASSERT(pvPort != 0);

  mdtPortTransaction *transaction;

  incrementCurrentTransactionId();
  transaction = getNewTransaction();
  Q_ASSERT(transaction != 0);
  transaction->setId(currentTransactionId());
  transaction->setData(data);
  transaction->setQueryReplyMode(queryReplyMode);

  return sendData(transaction);
}

/**
int mdtPortManager::writeData(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!pvTransactionsPending.contains(transaction->id()));
  Q_ASSERT(!transactionsDoneContains(transaction->id()));

  mdtFrame *frame;

  // Get a frame in pool
  lockPortMutex();
  if(pvPort->writeFramesPool().size() < 1){
    unlockPortMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    restoreTransaction(transaction);
    ///emit(busy());
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = pvPort->writeFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->append(transaction->data());
  pvPort->addFrameToWrite(frame);
  addTransactionPending(transaction);
  unlockPortMutex();

  return transaction->id();
}
*/

/**
int mdtPortManager::writeData(const QByteArray &data, bool queryReplyMode)
{
  Q_ASSERT(pvPort != 0);

  mdtPortTransaction *transaction;

  incrementCurrentTransactionId();
  transaction = getNewTransaction();
  Q_ASSERT(transaction != 0);
  transaction->setId(currentTransactionId());
  transaction->setData(data);
  transaction->setQueryReplyMode(queryReplyMode);

  return writeData(transaction);
}
*/

bool mdtPortManager::waitTransactionDone(int id)
{
  Q_ASSERT(pvPort != 0);

  ///int maxIter;

  // Check if transaction is allready done
  if(transactionsDoneContains(id)){
    return true;
  }
  // Check if transaction exists in pending queue
  if(!pvTransactionsPending.contains(id)){
    mdtError e(MDT_PORT_IO_ERROR, "Wait on a frame that was never added to pending queue, id: " + QString::number(id), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    ///emit(unhandledError());
    emit pmUnhandledErrorEvent();
    return false;
  }
  /*
   * If thread handles timeout/disconnection:
   *  In normal way, the thread notifies the error, the state becomes != Ready and this method returns false
   *  But, if many errors are notified, state changes often,
   *   and it happens that we are waiting, in Ready state, on a ID that was flushed, and this for a eternity.
   *   (Keep in mind that we let event loop running, and this way new transactions can be initalized, etc...)
   *  Another problem that is not signaled from thread is a frame the never completes.
   *  To avoid this problem, and keep coherence with port thread,
   *   we add a offset to wait timeout, and return if this time was elapsed.
   * If thread does not handle timeout:
   *  We simply return after timeout
   */
  if(pvThreadHandlesReadTimeout){
    startTransactionTimer(config().readTimeout() + 200);
  }else{
    startTransactionTimer(config().readTimeout());
  }
  /**
  if(pvThreadHandlesReadTimeout){
    maxIter = (config().readTimeout() + 200) / 50;
  }else{
    maxIter = config().readTimeout() / 50;
  }
  */
  // Try until success or timeout/error
  while(!transactionsDoneContains(id)){
    // Check about timeout or other error
    //  Note: on busy state, we continue waiting (f.ex. ReadPoolEmpty error)
    ///if((pvCurrentState != Ready)&&(pvCurrentState != Busy)){
    if((!isReady())&&(currentState() != Busy)){
      // Transactions are restored by onThreadsErrorOccured()
      stopTransactionTimer();
      return false;
    }
    // Check about timeout
    if(transactionTimeoutOccured()){
      // Transactions are restored by onTransactionTimeoutOccured()
      return false;
    }
    /**
    if(maxIter <= 0){
      flushTransactionsPending();
      flushTransactionsDone();
      return false;
    }
    maxIter--;
    */
    // Wait
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    /// \todo Adapter
    /**
    qApp->processEvents();
    msleep(50);
    */
  }
  stopTransactionTimer();

  return true;
}

bool mdtPortManager::waitOneTransactionDone()
{
  Q_ASSERT(pvPort != 0);

  ///int maxIter;

  /*
   * If thread handles timeout/disconnection:
   *  In normal way, the thread notifies the error, the state becomes != Ready and this method returns false
   *  But, if many errors are notified, state changes often,
   *   and it happens that we are waiting, in Ready state, until a transaction is done, and this for a eternity.
   *   (Keep in mind that we let event loop running, and this way new transactions can be initalized, etc...)
   *  Another problem that is not signaled from thread is a frame the never completes.
   *  To avoid this problems, and keep coherence with port thread,
   *   we add a offset to wait timeout, and return if this time was elapsed.
   * If thread does not handle timeout:
   *  We simply return after timeout
   */
  if(pvThreadHandlesReadTimeout){
    startTransactionTimer(config().readTimeout() + 200);
  }else{
    startTransactionTimer(config().readTimeout());
  }
  /**
  if(pvThreadHandlesReadTimeout){
    maxIter = (config().readTimeout() + 200) / 50;
  }else{
    maxIter = config().readTimeout() / 50;
  }
  */
  // Try until success or timeout/error
  while(pvTransactionsDone.isEmpty()){
    // Check about timeout or other error
    //  Note: on busy state, we continue waiting (f.ex. ReadPoolEmpty error)
    ///if((pvCurrentState != Ready)&&(pvCurrentState != Busy)){
    if((!isReady())&&(currentState() != Busy)){
      // Transactions are restored by onThreadsErrorOccured()
      stopTransactionTimer();
      return false;
    }
    // Check about timeout
    if(transactionTimeoutOccured()){
      // Transactions are restored by onTransactionTimeoutOccured()
      return false;
    }
    /**
    if(maxIter <= 0){
      flushTransactionsPending();
      flushTransactionsDone();
      return false;
    }
    maxIter--;
    */
    // Wait
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    /// \todo Adapter
    /**
    qApp->processEvents();
    msleep(50);
    */
  }
  stopTransactionTimer();

  return true;
}

QByteArray mdtPortManager::readenFrame(int id)
{
  mdtPortTransaction *transaction;

  transaction = transactionDone(id);
  if(transaction == 0){
    return QByteArray();
  }
  restoreTransaction(transaction);

  return transaction->data();
}

QList<QByteArray> mdtPortManager::readenFrames()
{
  mdtPortTransaction *transaction;
  QList<QByteArray> frames;
  int i;

  for(i=0; i<pvTransactionsDone.size(); ++i){
    transaction = pvTransactionsDone.at(i);
    Q_ASSERT(transaction != 0);
    frames.append(transaction->data());
    restoreTransaction(transaction);
  }
  pvTransactionsDone.clear();

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
    /// \todo Adapter
    msleep(granularity);
    qApp->processEvents();
  }
}

void mdtPortManager::flushIn(bool flushPortManagerBuffers, bool flushPortBuffers)
{
  Q_ASSERT(pvPort != 0);

  // Flush port manager buffers
  if(flushPortManagerBuffers){
    // Clear pending transactions
    flushTransactionsPending();
    // Clear Done transactions
    flushTransactionsDone();
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

mdtPortManager::state_t mdtPortManager::currentState() const
{
  Q_ASSERT(pvStateMachine != 0);

  return (state_t)pvStateMachine->currentState();
}

bool mdtPortManager::isReady() const
{
  if((currentState() == Ready)||(currentState() == PortReady)){
    return true;
  }

  return false;
}

bool mdtPortManager::isClosed() const
{
  return (currentState() == PortClosed);
}

void mdtPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

  pvPort->flush();
}

bool mdtPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);

  if(pvPort->setup() != mdtAbstractPort::NoError){
    /// \todo Handle and/or display error
    return false;
  }
  // Initialize flags
  setCurrentTransactionId(0);
  pvMaxTransactionsPending = config().writeQueueSize();

  return true;
}

void mdtPortManager::closePort()
{
  // It can happen that port was never set
  if(pvPort == 0){
    return;
  }
  ///stop();
  // Close the port
  pvPort->close();
  // Remove pending and done transactions
  flushTransactionsPending();
  flushTransactionsDone();
}

void mdtPortManager::setCurrentTransactionId(int id)
{
  pvCurrentTransactionId = id;
}

int mdtPortManager::currentTransactionId() const
{
  return pvCurrentTransactionId;
}

void mdtPortManager::incrementCurrentTransactionId(int min, int max)
{
  ++pvCurrentTransactionId;
  if(pvCurrentTransactionId < min){
    pvCurrentTransactionId = min;
  }
  if(pvCurrentTransactionId > max){
    pvCurrentTransactionId = min;
  }
}

void mdtPortManager::addTransactionPending(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!pvTransactionsPending.contains(transaction->id()));

  pvTransactionsPending.insert(transaction->id(), transaction);
  // If we have more transactions pending than write queue size, we go to busy state
  if(pvTransactionsPending.size() > pvMaxTransactionsPending){
    ///emit busy();
  }
  // Watch transactions size
  if(pvTransactionsPending.size() > (pvMaxTransactionsPending +20)){
    mdtError e(MDT_PORT_IO_ERROR, "Pending transactions queue has more than 20 items over maximum authorized, will clear it (this is a bug)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    qDeleteAll(pvTransactionsPending);
    pvTransactionsPending.clear();
  }
}

mdtPortTransaction *mdtPortManager::transactionPending(int id)
{
  // QMap returns a default-constructed value if key not exists (i.e. 0 for pointer)
  return pvTransactionsPending.take(id);
}

void mdtPortManager::flushTransactionsPending()
{
  QMutableMapIterator<int, mdtPortTransaction*> it(pvTransactionsPending);
  mdtPortTransaction *transaction;

  while(it.hasNext()){
    it.next();
    transaction = it.value();
    Q_ASSERT(transaction != 0);
    restoreTransaction(transaction);
    it.remove();
  }
}

void mdtPortManager::addTransactionDone(mdtPortTransaction *transaction)
{
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!transactionsDoneContains(transaction->id()));

  pvTransactionsDone.enqueue(transaction);
}

bool mdtPortManager::transactionsDoneContains(int id) const
{
  int i;

  for(i=0; i<pvTransactionsDone.size(); ++i){
    Q_ASSERT(pvTransactionsDone.at(i) != 0);
    if(pvTransactionsDone.at(i)->id() == id){
      return true;
    }
  }

  return false;
}

mdtPortTransaction *mdtPortManager::transactionDone(int id)
{
  int i;
  mdtPortTransaction *transaction;

  for(i=0; i<pvTransactionsDone.size(); ++i){
    transaction = pvTransactionsDone.at(i);
    Q_ASSERT(transaction != 0);
    if(transaction->id() == id){
      pvTransactionsDone.removeAt(i);
      return transaction;
    }
  }

  return 0;
}

void mdtPortManager::flushTransactionsDone()
{
  int i;

  for(i=0; i<pvTransactionsDone.size(); ++i){
    Q_ASSERT(pvTransactionsDone.at(i) != 0);
    restoreTransaction(pvTransactionsDone.at(i));
  }
  pvTransactionsDone.clear();
}

void mdtPortManager::startTransactionTimer(int timeout)
{
  Q_ASSERT(pvTransactionTimer != 0);

  pvTransactionTimeoutOccured = false;
  pvTransactionTimer->start(timeout);
}

void mdtPortManager::stopTransactionTimer()
{
  Q_ASSERT(pvTransactionTimer != 0);

  pvTransactionTimeoutOccured = false;
  pvTransactionTimer->stop();
}

bool mdtPortManager::transactionTimeoutOccured() const
{
  return pvTransactionTimeoutOccured;
}

void mdtPortManager::commitFrames()
{
  QMutableListIterator<mdtPortTransaction*> it(pvTransactionsDone);
  mdtPortTransaction *transaction;

  while(it.hasNext()){
    transaction = it.next();
    Q_ASSERT(transaction != 0);
    emit(newTransactionDone(transaction));
    if((!transaction->isQueryReplyMode())&&(!keepTransactionsDone())){
      restoreTransaction(transaction);
      it.remove();
    }
  }
}

void mdtPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;
  mdtPortTransaction *transaction;
  int framesCount = 0;

  // Get frames in readen queue
  lockPortMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = pvPort->readenFrames().dequeue();
    Q_ASSERT(frame != 0);
    if(frame->isComplete()){
      // Get a transaction
      transaction = transactionPending(currentTransactionId());
      if(transaction == 0){
        incrementCurrentTransactionId();
        transaction = getNewTransaction();
        transaction->setId(currentTransactionId());
      }
      Q_ASSERT(transaction != 0);
      Q_ASSERT(transaction->id() == currentTransactionId());
      // Copy data
      QByteArray data;
      data.append(frame->data(), frame->size());
      transaction->setData(data);
      // Add to transactions done queue
      addTransactionDone(transaction);
      ++framesCount;
    }else{
      mdtError e(MDT_PORT_IO_ERROR, "Received a uncomplete frame, will discard it", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortManager");
      e.commit();
    }
    // Restore frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  unlockPortMutex();
  // Commit
  if(framesCount > 0){
    commitFrames();
  }
}

void mdtPortManager::onThreadsErrorOccured(int error)
{
  switch(error){
    case mdtAbstractPort::NoError:
      ///emit(ready());
      break;
    case mdtAbstractPort::Disconnected:
      flushTransactionsPending();
      flushTransactionsDone();
      ///emit(disconnected());
      break;
    case mdtAbstractPort::Connecting:
      ///emit(connecting());
      break;
    case mdtAbstractPort::ReadPoolEmpty:
      ///emit(busy());
      break;
    case mdtAbstractPort::WritePoolEmpty:
      ///emit(busy());
      break;
    case mdtAbstractPort::WriteCanceled:
      flushTransactionsPending();
      ///emit(handledError());
      break;
    case mdtAbstractPort::ReadCanceled:
      flushTransactionsPending();
      flushTransactionsDone();
      ///emit(handledError());
      break;
      /**
    case mdtAbstractPort::ControlCanceled:
      emit(handledError());
      break;
      */
    case mdtAbstractPort::ReadTimeout:
      flushTransactionsPending();
      flushTransactionsDone();
      ///emit(busy());
      break;
    case mdtAbstractPort::WriteTimeout:
      flushTransactionsPending();
      ///emit(busy());
      break;
      /**
    case mdtAbstractPort::ControlTimeout:
      emit(busy());
      break;
      */
      /**
    case mdtAbstractPort::UnhandledError:
      emit(unhandledError());
      break;
      */
    default:
      flushTransactionsPending();
      flushTransactionsDone();
      ///emit(unhandledError());
  }
  qDebug() << "-> New state: " << currentState();
}

void mdtPortManager::onThreadReady(mdtPortThread *thread)
{
  Q_ASSERT(thread != 0);

  qDebug() << "Thread ready: " << thread;
  pvThreadsReady.append(thread);
  if(pvThreadsReady.size() == pvThreads.size()){
    emit pmAllThreadsReadyEvent();
  }
}

void mdtPortManager::onThreadStopped(mdtPortThread *thread)
{
  Q_ASSERT(thread != 0);
  Q_ASSERT(pvThreadsReady.contains(thread));

  qDebug() << "Thread finished: " << thread;
  pvThreadsReady.removeOne(thread);
  if(pvThreadsReady.isEmpty()){
    emit pmAllThreadsStoppedEvent();
  }
}

void mdtPortManager::startThreads()
{
  int i;

  qDebug() << "requeste start threads ...";
  for(i=0; i<pvThreads.size(); i++){
    pvThreads.at(i)->start();
  }
}

void mdtPortManager::stopThreads()
{
  int i;

  qDebug() << "requeste stop threads ...";
  for(i=0; i<pvThreads.size(); i++){
    pvThreads.at(i)->stop();
  }
}

void mdtPortManager::onTransactionTimeoutOccured()
{
  Q_ASSERT(pvTransactionTimer != 0);

  pvTransactionTimeoutOccured = true;
  flushTransactionsPending();
  flushTransactionsDone();
}

bool mdtPortManager::waitOnReadyState()
{
  while(currentState() != Ready){
    /// \todo Should add a timeout check (read? write? connect?)
    wait(100, 50);
  }

  return true;
}
