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
#include "mdtError.h"
#include <QTimer>
#include <QApplication>
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
  pvTransactionsAllocatedCount = 0;
  // As default, we not keep all incoming frames
  setKeepTransactionsDone(false);
  // Setup state machine
  pvStateMachine = 0;
  buildStateMachine();
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
  delete pvStateMachine;
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
  Q_ASSERT(pvPort->isOpen());

  qDebug() << "mdtPortManager::start() ...";
  int i;

  for(i=0; i<pvThreads.size(); i++){
    if(!pvThreads.at(i)->start()){
      return false;
    }
  }
  if(!waitOnReadyState()){
    return false;
  }
  qDebug() << "mdtPortManager::start() DONE";

  return true;
}

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

void mdtPortManager::stop()
{
  int i;

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
}

void mdtPortManager::setPortName(const QString &portName)
{
  Q_ASSERT(pvPort != 0);

  pvPort->setPortName(portName);
}

void mdtPortManager::setPortInfo(mdtPortInfo info)
{
  pvPortInfo = info;
  qDebug() << "mdtPortManager::setPortInfo() : port name: " << pvPortInfo.portName();
  setPortName(pvPortInfo.portName());
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

bool mdtPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);

  if(pvPort->setup() != mdtAbstractPort::NoError){
    return false;
  }
  // Initialize flags
  pvCancelReadWait = false;
  setCurrentTransactionId(0);

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
  /// \todo check about flush I/O
  // Remove pending and done transactions
  flushTransactionsPending();
  flushTransactionsDone();
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
    qDebug() << "mdtPortManager::getNewTransaction(): create new transaction. DELTA: " << (pvTransactionsAllocatedCount - pvTransactionsPool.size() - pvTransactionsPending.size() - pvTransactionsDone.size() - 1);
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
    for(i=0; i<maxIter; ++i){
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
  emit(busy());

  return false;
}

int mdtPortManager::writeData(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!pvTransactionsPending.contains(transaction->id()));
  Q_ASSERT(!transactionsDoneContains(transaction->id()));

  mdtFrame *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    restoreTransaction(transaction);
    emit(busy());
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
  pvPort->unlockMutex();

  return transaction->id();
}

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

bool mdtPortManager::waitTransactionDone(int id, int timeout, int granularity)
{
  Q_ASSERT(granularity > 0);

  int maxIter;
  mdtPortTransaction *transaction;

  if(timeout == 0){
    timeout = adjustedReadTimeout(timeout, false);
  }else{
    timeout = adjustedReadTimeout(timeout);
  }
  maxIter = timeout / granularity;
  // Check if transaction exists in pending queue
  // Note: we check also done queue, because it's possible that transaction was done
  if((!pvTransactionsPending.contains(id))&&(!transactionsDoneContains(id))){
    mdtError e(MDT_PORT_IO_ERROR, "Wait on a frame that was never added to pending queue, id: " + QString::number(id), mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortManager");
    e.commit();
    emit(unhandledError());
    return false;
  }
  // Try until success or timeout/error
  while(!transactionsDoneContains(id)){
    if(readWaitCanceled()){
      transaction = transactionPending(id);
      if(transaction != 0){
        restoreTransaction(transaction);
      }
      return false;
    }
    // Check about timeout
    if(maxIter <= 0){
      transaction = transactionPending(id);
      if(transaction != 0){
        restoreTransaction(transaction);
      }
      return false;
    }
    // Wait
    qApp->processEvents();
    msleep(granularity);
    maxIter--;
  }

  return true;
}

bool mdtPortManager::waitOneTransactionDone(int timeout, int granularity)
{
  Q_ASSERT(granularity > 0);

  int maxIter;

  if(timeout == 0){
    timeout = adjustedReadTimeout(timeout, false);
  }else{
    timeout = adjustedReadTimeout(timeout);
  }
  maxIter = timeout / granularity;
  // Try until success or timeout/error
  while(pvTransactionsDone.isEmpty()){
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
  return pvCurrentState;
}

void mdtPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

  pvPort->flush();
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
  // Watch transactions size
  if(pvTransactionsPending.size() > 20){
    mdtError e(MDT_PORT_IO_ERROR, "Pending transactions queue has more than 20 items, will clear it (this is a bug)", mdtError::Error);
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

void mdtPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;
  mdtPortTransaction *transaction;
  int framesCount = 0;

  
  // Get frames in readen queue
  pvPort->lockMutex();
  qDebug() << "mdtPortManager::fromThreadNewFrameReaden(), queue: " << pvPort->readenFrames();
  while(pvPort->readenFrames().size() > 0){
    frame = pvPort->readenFrames().dequeue();
    Q_ASSERT(frame != 0);
    /// \todo Error on incomplete frame
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
    }
    // Restore frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  // Commit
  if(framesCount > 0){
    commitFrames();
  }
}

/// \todo On disconnect, should flush I/O ?
void mdtPortManager::onThreadsErrorOccured(int error)
{
  qDebug() << "mdtPortManager::onThreadsErrorOccured() , code: " << (mdtAbstractPort::error_t)error;
  ///emit(errorStateChanged(error));

  switch(error){
    case mdtAbstractPort::NoError:
      emit(ready());
      break;
    case mdtAbstractPort::Disconnected:
      emit(disconnected());
      break;
    case mdtAbstractPort::Connecting:
      emit(connecting());
      break;
    case mdtAbstractPort::ReadPoolEmpty:
      emit(busy());
      break;
    case mdtAbstractPort::WritePoolEmpty:
      emit(busy());
      break;
    case mdtAbstractPort::WriteCanceled:
      emit(handledError());
      break;
    case mdtAbstractPort::ReadCanceled:
      cancelReadWait();
      emit(handledError());
      break;
    case mdtAbstractPort::ControlCanceled:
      emit(handledError());
      break;
    case mdtAbstractPort::ReadTimeout:
      emit(busy());
      break;
    case mdtAbstractPort::WriteTimeout:
      emit(busy());
      break;
    case mdtAbstractPort::ControlTimeout:
      emit(busy());
      break;
    case mdtAbstractPort::UnhandledError:
      emit(unhandledError());
      break;
    default:
      emit(unhandledError());
  }
}

void mdtPortManager::setStateDisconnected()
{
  if(pvCurrentState != Disconnected){
    pvCurrentState = Disconnected;
    qDebug() << "mdtPortManager: new state is Disconnected";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::setStateConnecting()
{
  if(pvCurrentState != Connecting){
    pvCurrentState = Connecting;
    qDebug() << "mdtPortManager: new state is Connecting";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::setStateReady()
{
  Q_ASSERT(pvPort != 0);

  int i;

  if(pvCurrentState != Ready){
    // Check that all threads are ready
    if(pvThreads.size() > 1){
      for(i=0; i<pvThreads.size(); i++){
        pvPort->lockMutex();
        if(pvThreads.at(i)->currentError() != mdtAbstractPort::NoError){
          pvPort->unlockMutex();
          return;
        }
        pvPort->unlockMutex();
      }
    }
    pvCurrentState = Ready;
    qDebug() << "mdtPortManager: new state is Ready";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::setStateBusy()
{
  if(pvCurrentState != Busy){
    pvCurrentState = Busy;
    qDebug() << "mdtPortManager: new state is Busy";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::setStateWarning()
{
  if(pvCurrentState != Warning){
    pvCurrentState = Warning;
    qDebug() << "mdtPortManager: new state is Warning";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::setStateError()
{
  if(pvCurrentState != Error){
    pvCurrentState = Error;
    qDebug() << "mdtPortManager: new state is Error";
    emit(stateChanged(pvCurrentState));
  }
}

void mdtPortManager::buildStateMachine()
{
  Q_ASSERT(pvStateMachine == 0);

  pvStateMachine = new QStateMachine;
  pvStateDisconnected = new QState;
  pvStateConnecting = new QState;
  pvStateReady = new QState;
  pvStateBusy = new QState;
  pvStateWarning = new QState;
  pvStateError = new QState;
  connect(pvStateDisconnected, SIGNAL(entered()), this, SLOT(setStateDisconnected()));
  pvStateDisconnected->addTransition(this, SIGNAL(connecting()), pvStateConnecting);
  pvStateDisconnected->addTransition(this, SIGNAL(ready()), pvStateReady);
  connect(pvStateConnecting, SIGNAL(entered()), this, SLOT(setStateConnecting()));
  pvStateConnecting->addTransition(this, SIGNAL(disconnected()), pvStateDisconnected);
  pvStateConnecting->addTransition(this, SIGNAL(unhandledError()), pvStateError);
  pvStateConnecting->addTransition(this, SIGNAL(ready()), pvStateReady);
  connect(pvStateReady, SIGNAL(entered()), this, SLOT(setStateReady()));
  pvStateReady->addTransition(this, SIGNAL(disconnected()), pvStateDisconnected);
  pvStateReady->addTransition(this, SIGNAL(unhandledError()), pvStateError);
  pvStateReady->addTransition(this, SIGNAL(handledError()), pvStateWarning);
  pvStateReady->addTransition(this, SIGNAL(busy()), pvStateBusy);
  connect(pvStateBusy, SIGNAL(entered()), this, SLOT(setStateBusy()));
  pvStateBusy->addTransition(this, SIGNAL(disconnected()), pvStateDisconnected);
  pvStateBusy->addTransition(this, SIGNAL(ready()), pvStateReady);
  pvStateBusy->addTransition(this, SIGNAL(unhandledError()), pvStateError);
  pvStateBusy->addTransition(this, SIGNAL(handledError()), pvStateWarning);
  connect(pvStateWarning, SIGNAL(entered()), this, SLOT(setStateWarning()));
  pvStateWarning->addTransition(this, SIGNAL(disconnected()), pvStateDisconnected);
  pvStateWarning->addTransition(this, SIGNAL(ready()), pvStateReady);
  connect(pvStateError, SIGNAL(entered()), this, SLOT(setStateError()));
  pvStateMachine->addState(pvStateDisconnected);
  pvStateMachine->addState(pvStateConnecting);
  pvStateMachine->addState(pvStateReady);
  pvStateMachine->addState(pvStateBusy);
  pvStateMachine->addState(pvStateWarning);
  pvStateMachine->addState(pvStateError);
  pvStateMachine->setInitialState(pvStateDisconnected);
  pvStateMachine->start();
}

bool mdtPortManager::waitOnReadyState()
{
  while(pvCurrentState != Ready){
    wait(100, 50);
  }

  return true;
}
