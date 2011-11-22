
#include "mdtAbstractSerialPort.h"

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : QObject(parent)
{
  pvRxTimeoutOccured = false;
  pvTxTimeoutOccured = false;
  pvCarIsOn = false;
  pvDsrIsOn = false;
  pvCtsIsOn = false;
  pvRngIsOn = false;
  // Emit signals with initial states
  emit carChanged(pvCarIsOn);
  emit dsrChanged(pvDsrIsOn);
  emit ctsChanged(pvCtsIsOn);
  emit rngChanged(pvRngIsOn);
  emit rxTimeoutStateChanged(pvRxTimeoutOccured);
  emit txTimeoutStateChanged(pvTxTimeoutOccured);
}

mdtAbstractSerialPort::~mdtAbstractSerialPort()
{
}
