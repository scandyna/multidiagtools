
#include "mdtAbstractSerialPort.h"

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : QObject(parent)
{
  pvCarIsOn = false;
  pvDsrIsOn = false;
  pvCtsIsOn = false;
  pvRngIsOn = false;
}

mdtAbstractSerialPort::~mdtAbstractSerialPort()
{
}
