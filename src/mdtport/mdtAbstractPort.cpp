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
#include "mdtAbstractPort.h"
#include "mdtFrameUsbTmc.h"

#include <QDebug>

mdtAbstractPort::mdtAbstractPort(QObject *parent)
 : QObject(parent)
{
  pvReadTimeoutOccured = false;
  pvReadTimeoutOccuredPrevious = false;
  pvWriteTimeoutOccured = false;
  pvWriteTimeoutOccuredPrevious = false;
  pvIsOpen = false;
  pvConfig = 0;
  pvCancelWrite = false;

  // Emit signals with initial states
  emit readTimeoutStateChanged(pvReadTimeoutOccured);
  emit writeTimeoutStateChanged(pvWriteTimeoutOccured);
}

mdtAbstractPort::~mdtAbstractPort()
{
  qDebug() << "mdtAbstractPort::~mdtAbstractPort() ...";
  qDebug() << "mdtAbstractPort::~mdtAbstractPort() END";
}

void mdtAbstractPort::setPortName(const QString &portName)
{
  pvPortName = portName;
}

QString &mdtAbstractPort::portName()
{
  return pvPortName;
}

bool mdtAbstractPort::isOpen() const
{
  return pvIsOpen;
}

mdtAbstractPort::error_t mdtAbstractPort::open()
{
  error_t err;

  if(isOpen()){
    close();
  }
  Q_ASSERT(!pvIsOpen);
  // Call system open method
  err = pvOpen();
  if(err == NoError){
    pvIsOpen = true;
  }

  return err;
}

void mdtAbstractPort::close()
{
  if(!isOpen()){
    return;
  }
  // Call system's flush methods
  pvFlushIn();
  pvFlushOut();
  // Call system close method
  pvClose();
  // Delete queues
  qDeleteAll(pvReadFramesPool);
  pvReadFramesPool.clear();
  qDeleteAll(pvReadenFrames);
  pvReadenFrames.clear();
  qDeleteAll(pvWriteFramesPool);
  pvWriteFramesPool.clear();
  qDeleteAll(pvWriteFrames);
  pvWriteFrames.clear();
  pvIsOpen = false;
  qDebug() << "mdtAbstractPort::close() END";
}

void mdtAbstractPort::setConfig(mdtPortConfig *cfg)
{
  pvConfig = cfg;
}

mdtPortConfig &mdtAbstractPort::config()
{
  Q_ASSERT(pvConfig != 0);

  return *pvConfig;
}

mdtAbstractPort::error_t mdtAbstractPort::setup()
{
  error_t err;

  // Open port if needed
  if(!isOpen()){
    err = open();
    if(err != NoError){
      return err;
    }
  }
  // Call setup
  err = pvSetup();
  if(err != NoError){
    close();
    return err;
  }
  // Init queues
  initQueues();

  return NoError;
}

mdtAbstractPort::error_t mdtAbstractPort::reconnect(int timeout)
{
  return UnhandledError;
}

mdtAbstractPort::error_t mdtAbstractPort::waitForReadyRead(int msecs)
{
  Q_ASSERT(pvConfig != 0);

  error_t retVal;

  setReadTimeout(msecs);
  retVal = waitForReadyRead();
  setReadTimeout(pvConfig->readTimeout());

  return retVal;
}

bool mdtAbstractPort::suspendTransmission()
{
  return true;
}

bool mdtAbstractPort::resumeTransmission()
{
  return true;
}

void mdtAbstractPort::flushIn()
{
  lockMutex();
  while(pvReadenFrames.size() > 0){
    pvReadFramesPool.enqueue(pvReadenFrames.dequeue());
  }
  pvFlushIn();
  unlockMutex();
}

void mdtAbstractPort::flushOut()
{
  lockMutex();
  while(pvWriteFrames.size() > 0){
    pvWriteFramesPool.enqueue(pvWriteFrames.dequeue());
  }
  pvFlushOut();
  unlockMutex();
}

void mdtAbstractPort::updateReadTimeoutState(bool state)
{
  if(state != pvReadTimeoutOccuredPrevious){
    emit readTimeoutStateChanged(state);
  }
  pvReadTimeoutOccuredPrevious = pvReadTimeoutOccured;
  pvReadTimeoutOccured = state;
}

void mdtAbstractPort::updateWriteTimeoutState(bool state)
{
  if(state != pvWriteTimeoutOccured){
    emit writeTimeoutStateChanged(state);
  }
  pvWriteTimeoutOccuredPrevious = pvWriteTimeoutOccured;
  pvWriteTimeoutOccured = state;
}

bool mdtAbstractPort::readTimeoutOccured()
{
  return pvReadTimeoutOccured;
}

bool mdtAbstractPort::writeTimeoutOccured()
{
  return pvWriteTimeoutOccured;
}

void mdtAbstractPort::flush()
{
  lockMutex();
  while(pvReadenFrames.size() > 0){
    pvReadFramesPool.enqueue(pvReadenFrames.dequeue());
  }
  pvFlushIn();
  while(pvWriteFrames.size() > 0){
    pvWriteFramesPool.enqueue(pvWriteFrames.dequeue());
  }
  pvFlushOut();
  unlockMutex();
}

void mdtAbstractPort::initQueues()
{
  mdtFrame *frame;

  // Create the read frames pools with requested type
  qDebug() << "mdtAbstractPort::initQueues() , n: " << config().readQueueSize() << " , fsize: " << config().readFrameSize();
  for(int i=0; i<config().readQueueSize(); i++){
    switch(config().frameType()){
      // Raw (binary) frame
      case mdtFrame::FT_RAW:
        frame = new mdtFrame;
        Q_ASSERT(frame != 0);
        frame->setDirectlyComplete(true);
        break;
      // Raw (binary) frame for use with timeout protocol
      case mdtFrame::FT_RAW_TOP:
        frame = new mdtFrame;
        break;
      // ASCII frame type
      case mdtFrame::FT_ASCII:
        frame = new mdtFrameAscii;
        dynamic_cast<mdtFrameAscii*>(frame)->setEofSeq(config().endOfFrameSeq());
        break;
      // MODBUS/TCP frame type
      case mdtFrame::FT_MODBUS_TCP:
        frame = new mdtFrameModbusTcp;
        break;
      // USBTMC frame type
      case mdtFrame::FT_USBTMC:
        frame = new mdtFrameUsbTmc;
        break;
      // Base frame type
      default:
        frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(config().readFrameSize());
    pvReadFramesPool.enqueue(frame);
  }
  // Create the write frames pools
  for(int i=0; i<config().writeQueueSize(); i++){
    switch(config().frameType()){
      // Raw (binary) frame
      case mdtFrame::FT_RAW:
        frame = new mdtFrame;
        break;
      // Raw (binary) frame for use with timeout protocol
      case mdtFrame::FT_RAW_TOP:
        frame = new mdtFrame;
        break;
      // ASCII frame type
      case mdtFrame::FT_ASCII:
        frame = new mdtFrameAscii;
        break;
      // MODBUS/TCP frame type
      case mdtFrame::FT_MODBUS_TCP:
        frame = new mdtFrameModbusTcp;
        break;
      // USBTMC frame type
      case mdtFrame::FT_USBTMC:
        frame = new mdtFrameUsbTmc;
        break;
      // Base frame type
      default:
        frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(config().writeFrameSize());
    pvWriteFramesPool.enqueue(frame);
  }
}

QQueue<mdtFrame*> &mdtAbstractPort::readenFrames()
{
  return pvReadenFrames;
}

QQueue<mdtFrame*> &mdtAbstractPort::readFramesPool()
{
  return pvReadFramesPool;
}

void mdtAbstractPort::addFrameToWrite(mdtFrame *frame)
{
  Q_ASSERT(frame != 0);

  pvWriteFrames.enqueue(frame);
  pvCancelWrite = false;
  pvWriteFrameAvailable.wakeAll();
}

mdtFrame *mdtAbstractPort::getFrameToWrite()
{
  mdtFrame *frame;

  if(pvWriteFrames.size() < 1){
    pvWriteFrameAvailable.wait(&pvMutex);
  }
  // If abortFrameToWriteWait() was called, it can happen that queue is empty
  if(pvWriteFrames.size() < 1){
    return 0;
  }
  frame = pvWriteFrames.dequeue();
  Q_ASSERT(frame != 0);

  return frame;
}

void mdtAbstractPort::abortFrameToWriteWait()
{
  pvWriteFrameAvailable.wakeAll();
}

QQueue<mdtFrame*> &mdtAbstractPort::writeFrames()
{
  return pvWriteFrames;
}

QQueue<mdtFrame*> &mdtAbstractPort::writeFramesPool()
{
  return pvWriteFramesPool;
}

void mdtAbstractPort::lockMutex()
{
  pvMutex.lock();
}

void mdtAbstractPort::unlockMutex()
{
  pvMutex.unlock();
}
