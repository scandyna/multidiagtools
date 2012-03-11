
#include "mdtAbstractPort.h"


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

mdtPortConfig &mdtAbstractPort::config()
{
  return pvConfig;
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
  bool retVal;

  lockMutex();
  retVal = pvReadTimeoutOccured;
  unlockMutex();

  return retVal;
}

bool mdtAbstractPort::writeTimeoutOccured()
{
  bool retVal;

  lockMutex();
  retVal = pvWriteTimeoutOccured;
  unlockMutex();

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
