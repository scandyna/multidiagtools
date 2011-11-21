
#include "mdtAbstractSerialPortManager.h"
#include <QDebug>

mdtAbstractSerialPortManager::mdtAbstractSerialPortManager()
{
  pvSerialPort = 0;
}

mdtAbstractSerialPortManager::~mdtAbstractSerialPortManager()
{
}

void mdtAbstractSerialPortManager::setSerialPortObj(mdtAbstractSerialPort *sp)
{
  Q_ASSERT(sp != 0);

  pvSerialPort = sp;
}
