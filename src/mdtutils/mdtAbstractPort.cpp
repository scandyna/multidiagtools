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

//#include <QDebug>

mdtAbstractPort::mdtAbstractPort(QObject *parent)
 : QObject(parent)
{
  pvReadTimeoutOccured = false;
  pvReadTimeoutOccuredPrevious = false;
  pvWriteTimeoutOccured = false;
  pvWriteTimeoutOccuredPrevious = false;

  // Emit signals with initial states
  emit readTimeoutStateChanged(pvReadTimeoutOccured);
  emit writeTimeoutStateChanged(pvWriteTimeoutOccured);
}

mdtAbstractPort::~mdtAbstractPort()
{
}

bool mdtAbstractPort::open(mdtPortConfig &cfg)
{
  mdtFrame *frame;

  // Create the read frames pools with requested type
  for(int i=0; i<cfg.readQueueSize(); i++){
    switch(cfg.frameType()){
      // ASCII frame type
      case mdtFrame::FT_ASCII:
        frame = new mdtFrameAscii;
        dynamic_cast<mdtFrameAscii*>(frame)->setEofSeq(cfg.endOfFrameSeq());
        break;
      // MODBUS/TCP frame type
      case mdtFrame::FT_MODBUS_TCP:
        frame = new mdtFrameModbusTcp;
        break;
      // Base frame type
      default:
        frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.readFrameSize());
    pvReadFramesPool.enqueue(frame);
  }
  // Create the write frames pools
  for(int i=0; i<cfg.writeQueueSize(); i++){
    switch(cfg.frameType()){
      // ASCII frame type
      case mdtFrame::FT_ASCII:
        frame = new mdtFrameAscii;
        break;
      // MODBUS/TCP frame type
      case mdtFrame::FT_MODBUS_TCP:
        frame = new mdtFrameModbusTcp;
        break;
      // Base frame type
      default:
        frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.writeFrameSize());
    pvWriteFramesPool.enqueue(frame);
  }
  pvConfig = cfg;
  unlockMutex();
  return true;
}

void mdtAbstractPort::close()
{
  // Delete queues
  qDeleteAll(pvReadFramesPool);
  pvReadFramesPool.clear();
  qDeleteAll(pvReadenFrames);
  pvReadenFrames.clear();
  qDeleteAll(pvWriteFramesPool);
  pvWriteFramesPool.clear();
  qDeleteAll(pvWriteFrames);
  pvWriteFrames.clear();
  // Unlock the mutex
  unlockMutex();
}

mdtPortConfig &mdtAbstractPort::config()
{
  return pvConfig;
}

bool mdtAbstractPort::waitForReadyRead(int msecs)
{
  setReadTimeout(msecs);
  return waitForReadyRead();
}

void mdtAbstractPort::readOneFrame()
{
}

void mdtAbstractPort::writeOneFrame()
{
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

/// NOTE: verrouillage à revoir !!
bool mdtAbstractPort::readTimeoutOccured()
{
  bool retVal;

  ///lockMutex();
  retVal = pvReadTimeoutOccured;
  ///unlockMutex();

  return retVal;
}

/// NOTE: verrouillage à revoir !!
bool mdtAbstractPort::writeTimeoutOccured()
{
  bool retVal;

  ///lockMutex();
  retVal = pvWriteTimeoutOccured;
  ///unlockMutex();

  return retVal;
}

QQueue<mdtFrame*> &mdtAbstractPort::readenFrames()
{
  return pvReadenFrames;
}

QQueue<mdtFrame*> &mdtAbstractPort::readFramesPool()
{
  return pvReadFramesPool;
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
