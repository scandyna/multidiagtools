/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
///#include "mdtFrameUsbTmc.h"

mdtAbstractPort::mdtAbstractPort(QObject *parent)
 : QObject(parent)
{
  pvIsOpen = false;
  pvConfig = 0;
  pvFlushInRequestPending = false;
  pvFlushOutRequestPending = false;
}

mdtAbstractPort::~mdtAbstractPort()
{
  qDebug() << "mdtAbstractPort::~mdtAbstractPort()";
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
  qDebug() << "mdtAbstractPort::close() ...";
  if(!isOpen()){
    return;
  }
  // Call system's flush methods
  pvFlushIn();
  pvFlushOut();
  // Call system close method
  pvClose();
  // Delete queues
  qDebug() << "mdtAbstractPort::close() deleting read pool queues ...";
  qDeleteAll(pvReadFramesPool);
  pvReadFramesPool.clear();
  qDebug() << "mdtAbstractPort::close() deleting readen queues ...";
  qDeleteAll(pvReadenFrames);
  pvReadenFrames.clear();
  qDebug() << "mdtAbstractPort::close() deleting write pool queues ...";
  qDeleteAll(pvWriteFramesPool);
  pvWriteFramesPool.clear();
  qDebug() << "mdtAbstractPort::close() deleting to write ...";
  qDeleteAll(pvWriteFrames);
  pvWriteFrames.clear();
  pvIsOpen = false;
  qDebug() << "mdtAbstractPort::close() DONE";
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
  pvFlushInRequestPending = true;
  unlockMutex();
}

bool mdtAbstractPort::flushInRequestPending()
{
  if(pvFlushInRequestPending){
    pvFlushInRequestPending = false;
    return true;
  }
  return false;
}

void mdtAbstractPort::flushOut()
{
  lockMutex();
  while(pvWriteFrames.size() > 0){
    pvWriteFramesPool.enqueue(pvWriteFrames.dequeue());
  }
  pvFlushOut();
  pvFlushOutRequestPending = true;
  unlockMutex();
}

bool mdtAbstractPort::flushOutRequestPending()
{
  if(pvFlushOutRequestPending){
    pvFlushOutRequestPending = false;
    return true;
  }
  return false;
}

void mdtAbstractPort::flush()
{
  lockMutex();
  while(pvReadenFrames.size() > 0){
    pvReadFramesPool.enqueue(pvReadenFrames.dequeue());
  }
  pvFlushIn();
  pvFlushInRequestPending = true;
  while(pvWriteFrames.size() > 0){
    pvWriteFramesPool.enqueue(pvWriteFrames.dequeue());
  }
  pvFlushOut();
  pvFlushOutRequestPending = true;
  unlockMutex();
}

void mdtAbstractPort::initQueues()
{
  mdtFrame *frame;

  // Create the read frames pools with requested type
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
      ///case mdtFrame::FT_USBTMC:
      ///  frame = new mdtFrameUsbTmc;
      ///  break;
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
      ///case mdtFrame::FT_USBTMC:
      ///  frame = new mdtFrameUsbTmc;
      ///  break;
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
  pvWriteFrameAvailable.wakeAll();
}

mdtFrame *mdtAbstractPort::getFrameToWrite()
{
  mdtFrame *frame;

  // If queue is empty, wait
  if(pvWriteFrames.size() < 1){
    pvWriteFrameAvailable.wait(&pvMutex);
    // During wait it can happen that a flush was requested,
    //  witch clears the write queue. If this happens,
    //  we must clear the pvFlushOutRequestPending flag,
    //  else thread will not write new frame that must be sent.
    pvFlushOutRequestPending = false;
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

QDebug operator<<(QDebug dbg, mdtAbstractPort::error_t error)
{
  switch(error){
    case mdtAbstractPort::NoError:
      return dbg.nospace() << "(" << (int)error << ", NoError" << ")";
    case mdtAbstractPort::PortLocked:
      return dbg.nospace() << "(" << (int)error << ", PortLocked" << ")";
    case mdtAbstractPort::PortNotFound:
      return dbg.nospace() << "(" << (int)error << ", PortNotFound" << ")";
    case mdtAbstractPort::PortAccess:
      return dbg.nospace() << "(" << (int)error << ", PortAccess" << ")";
    case mdtAbstractPort::SetupError:
      return dbg.nospace() << "(" << (int)error << ", SetupError" << ")";
    case mdtAbstractPort::WriteCanceled:
      return dbg.nospace() << "(" << (int)error << ", WriteCanceled" << ")";
    case mdtAbstractPort::ReadCanceled:
      return dbg.nospace() << "(" << (int)error << ", ReadCanceled" << ")";
    case mdtAbstractPort::ControlCanceled:
      return dbg.nospace() << "(" << (int)error << ", ControlCanceled" << ")";
    case mdtAbstractPort::ReadTimeout:
      return dbg.nospace() << "(" << (int)error << ", ReadTimeout" << ")";
    case mdtAbstractPort::WriteTimeout:
      return dbg.nospace() << "(" << (int)error << ", WriteTimeout" << ")";
    case mdtAbstractPort::MessageInTimeout:
      return dbg.nospace() << "(" << (int)error << ", MessageInTimeout" << ")";
    case mdtAbstractPort::MessageInCanceled:
      return dbg.nospace() << "(" << (int)error << ", MessageInCanceled" << ")";
    case mdtAbstractPort::ControlTimeout:
      return dbg.nospace() << "(" << (int)error << ", ControlTimeout" << ")";
    case mdtAbstractPort::Disconnected:
      return dbg.nospace() << "(" << (int)error << ", Disconnected" << ")";
    case mdtAbstractPort::Connecting:
      return dbg.nospace() << "(" << (int)error << ", Connecting" << ")";
    case mdtAbstractPort::ConnectionFailed:
      return dbg.nospace() << "(" << (int)error << ", ConnectionFailed" << ")";
    case mdtAbstractPort::UnhandledError:
      return dbg.nospace() << "(" << (int)error << ", UnhandledError" << ")";
    case mdtAbstractPort::ReadPoolEmpty:
      return dbg.nospace() << "(" << (int)error << ", ReadPoolEmpty" << ")";
    case mdtAbstractPort::WritePoolEmpty:
      return dbg.nospace() << "(" << (int)error << ", WritePoolEmpty" << ")";
    case mdtAbstractPort::ErrorHandled:
      return dbg.nospace() << "(" << (int)error << ", ErrorHandled" << ")";
    case mdtAbstractPort::ReadWriteBusy:
      return dbg.nospace() << "(" << (int)error << ", ReadWriteBusy" << ")";
    case mdtAbstractPort::UsbWriteStall:
      return dbg.nospace() << "(" << (int)error << ", UsbWriteStall" << ")";
    case mdtAbstractPort::UsbReadStall:
      return dbg.nospace() << "(" << (int)error << ", UsbReadStall" << ")";
  }
  return dbg.nospace() << "(" << (int)error << ", ???" << ")";
}
