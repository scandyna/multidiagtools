
#include "mdtUsbtmcFile.h"

mdtUsbtmcFile::mdtUsbtmcFile(QObject *parent)
 : mdtDeviceFile(parent)
{
}

mdtUsbtmcFile::~mdtUsbtmcFile()
{
}

bool mdtUsbtmcFile::waitEventRead()
{
  pvReadWriteMutex.lock();
  if(!pvReadCondition.wait(&pvReadWriteMutex, 1000)){
    updateReadTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateReadTimeoutState(false);
  if(!mdtDeviceFile::waitEventRead()){
    pvReadWriteMutex.unlock();
    return false;
  }
  pvReadWriteMutex.unlock();

  return true;
}

void mdtUsbtmcFile::readOneFrame()
{
  pvReadWriteMutex.lock();
  pvReadCondition.wakeAll();
  pvReadWriteMutex.unlock();
}

bool mdtUsbtmcFile::waitEventWriteReady()
{
  pvReadWriteMutex.lock();
  if(!pvWriteCondition.wait(&pvReadWriteMutex, 1000)){
    updateWriteTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateWriteTimeoutState(false);
  if(!mdtDeviceFile::waitEventWriteReady()){
    pvReadWriteMutex.unlock();
    return false;
  }
  pvReadWriteMutex.unlock();

  return true;
}

void mdtUsbtmcFile::writeOneFrame()
{
  pvReadWriteMutex.lock();
  pvWriteCondition.wakeAll();
  pvReadWriteMutex.unlock();
}
