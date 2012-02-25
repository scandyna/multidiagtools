
#include "mdtAbstractSerialPort.h"

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : mdtDeviceFile(parent)
{
  pvCarIsOn = false;
  pvDsrIsOn = false;
  pvCtsIsOn = false;
  pvRngIsOn = false;
  // Emit signals with initial states
  emit carChanged(pvCarIsOn);
  emit dsrChanged(pvDsrIsOn);
  emit ctsChanged(pvCtsIsOn);
  emit rngChanged(pvRngIsOn);
}

mdtAbstractSerialPort::~mdtAbstractSerialPort()
{
}
