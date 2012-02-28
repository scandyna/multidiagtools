
#include "mdtAbstractDeviceFile.h"


mdtAbstractDeviceFile::mdtAbstractDeviceFile(QObject *parent)
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

mdtAbstractDeviceFile::~mdtAbstractDeviceFile()
{
}

mdtDeviceFileConfig &mdtAbstractDeviceFile::config()
{
  return pvConfig;
}

void mdtAbstractDeviceFile::updateReadTimeoutState(bool state)
{
  if(state != pvReadTimeoutOccuredPrevious){
    emit readTimeoutStateChanged(state);
  }
  pvReadTimeoutOccuredPrevious = pvReadTimeoutOccured;
  pvReadTimeoutOccured = state;
}

void mdtAbstractDeviceFile::updateWriteTimeoutState(bool state)
{
  if(state != pvWriteTimeoutOccured){
    emit writeTimeoutStateChanged(state);
  }
  pvWriteTimeoutOccuredPrevious = pvWriteTimeoutOccured;
  pvWriteTimeoutOccured = state;
}

bool mdtAbstractDeviceFile::readTimeoutOccured()
{
  bool retVal;

  lockMutex();
  retVal = pvReadTimeoutOccured;
  unlockMutex();

  return retVal;
}

bool mdtAbstractDeviceFile::writeTimeoutOccured()
{
  bool retVal;

  lockMutex();
  retVal = pvWriteTimeoutOccured;
  unlockMutex();

  return retVal;
}

QQueue<mdtFrame*> &mdtAbstractDeviceFile::readenFrames()
{
  return pvReadenFrames;
}

QQueue<mdtFrame*> &mdtAbstractDeviceFile::readFramesPool()
{
  return pvReadFramesPool;
}

QQueue<mdtFrame*> &mdtAbstractDeviceFile::writeFrames()
{
  return pvWriteFrames;
}

QQueue<mdtFrame*> &mdtAbstractDeviceFile::writeFramesPool()
{
  return pvWriteFramesPool;
}

void mdtAbstractDeviceFile::lockMutex()
{
  pvMutex.lock();
}

void mdtAbstractDeviceFile::unlockMutex()
{
  pvMutex.unlock();
}
