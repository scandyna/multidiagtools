
#include "mdtAbstractSerialPort.h"

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : QObject(parent)
{
  pvRxTimeoutOccured = false;
  pvRxTimeoutOccuredPrevious = false;
  pvTxTimeoutOccured = false;
  pvTxTimeoutOccuredPrevious = false;
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

void mdtAbstractSerialPort::updateRxTimeoutState(bool state)
{
  if(state != pvRxTimeoutOccuredPrevious){
    emit rxTimeoutStateChanged(state);
  }
  pvRxTimeoutOccuredPrevious = pvRxTimeoutOccured;
  pvRxTimeoutOccured = state;
}

void mdtAbstractSerialPort::updateTxTimeoutState(bool state)
{
  if(state != pvTxTimeoutOccuredPrevious){
    emit txTimeoutStateChanged(state);
  }
  pvTxTimeoutOccuredPrevious = pvTxTimeoutOccured;
  pvTxTimeoutOccured = state;
}

QQueue<mdtFrame*> &mdtAbstractSerialPort::rxFrames()
{
  return pvRxFrames;
}

QQueue<mdtFrame*> &mdtAbstractSerialPort::rxFramesPool()
{
  return pvRxFramesPool;
}

QQueue<mdtFrame*> &mdtAbstractSerialPort::txFrames()
{
  return pvTxFrames;
}

QQueue<mdtFrame*> &mdtAbstractSerialPort::txFramesPool()
{
  return pvTxFramesPool;
}
