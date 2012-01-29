
#include "mdtSerialPort.h"

mdtSerialPort::mdtSerialPort(QObject *parent)
 : mdtSerialPortSys(parent)
{
  // Set default timeouts
  setRxTimeout(500);
  setTxTimeout(500);
}

mdtSerialPort::~mdtSerialPort()
{
  
}

bool mdtSerialPort::rxTimeoutOccured()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRxTimeoutOccured;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::txTimeoutOccured()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvTxTimeoutOccured;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::carIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCarIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::dsrIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvDsrIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::ctsIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCtsIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::rngIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRngIsOn;
  pvMutex.unlock();

  return retVal;
}

void mdtSerialPort::lockMutex()
{
  pvMutex.lock();
}

void mdtSerialPort::unlockMutex()
{
  pvMutex.unlock();
}

void mdtSerialPort::setRts(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setRts(on);
  pvMutex.unlock();
}

void mdtSerialPort::setDtr(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setDtr(on);
  pvMutex.unlock();
}
