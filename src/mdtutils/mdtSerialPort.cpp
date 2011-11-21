
#include "mdtSerialPort.h"

mdtSerialPort::mdtSerialPort(QObject *parent)
 : mdtSerialPortSys(parent)
{
  
}

mdtSerialPort::~mdtSerialPort()
{
  
}

bool mdtSerialPort::carIsOn()
{
  // TODO: handle thread problems
  return pvCarIsOn;
}

bool mdtSerialPort::dsrIsOn()
{
  // TODO: handle thread problems
  return pvDsrIsOn;
}

bool mdtSerialPort::ctsIsOn()
{
  // TODO: handle thread problems
  return pvCtsIsOn;
}

bool mdtSerialPort::rngIsOn()
{
  // TODO: handle thread problems
  return pvRngIsOn;
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
  // TODO: handle thread problems
  mdtSerialPortSys::setRts(on);
}

void mdtSerialPort::setDtr(bool on)
{
  // TODO: handle thread problems
  mdtSerialPortSys::setDtr(on);
}
