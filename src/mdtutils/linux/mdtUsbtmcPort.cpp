
#include "mdtUsbtmcPort.h"

mdtUsbtmcPort::mdtUsbtmcPort(QObject *parent)
 : mdtPort(parent)
{
}

mdtUsbtmcPort::~mdtUsbtmcPort()
{
}

bool mdtUsbtmcPort::waitEventRead()
{
  pvReadWriteMutex.lock();
  if(!pvReadCondition.wait(&pvReadWriteMutex, 500)){
    updateReadTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateReadTimeoutState(false);
  if(!mdtPort::waitEventRead()){
    pvReadWriteMutex.unlock();
    return false;
  }
  pvReadWriteMutex.unlock();

  return true;
}

bool mdtUsbtmcPort::waitEventWriteReady()
{
  pvReadWriteMutex.lock();
  if(!pvWriteCondition.wait(&pvReadWriteMutex, 500)){
    updateWriteTimeoutState(true);
    pvReadWriteMutex.unlock();
    return true;
  }
  updateWriteTimeoutState(false);
  if(!mdtPort::waitEventWriteReady()){
    pvReadWriteMutex.unlock();
    return false;
  }
  pvReadWriteMutex.unlock();

  return true;
}

void mdtUsbtmcPort::writeOneFrame()
{
  pvReadWriteMutex.lock();
  pvWriteCondition.wakeAll();
  pvReadWriteMutex.unlock();
}

void mdtUsbtmcPort::readOneFrame()
{
  pvReadWriteMutex.lock();
  pvReadCondition.wakeAll();
  pvReadWriteMutex.unlock();
}
