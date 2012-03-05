
#include "mdtAbstractSerialPort.h"

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : mdtPort(parent)
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
  // Attributes
  pvUartType = UT_UNKNOW;
}

mdtAbstractSerialPort::~mdtAbstractSerialPort()
{
}

mdtAbstractSerialPort::sp_uart_type_t mdtAbstractSerialPort::uartType()
{
  return pvUartType;
}

QString mdtAbstractSerialPort::uartTypeStr()
{
  switch(pvUartType){
    case UT_UNKNOW:
      return "Unknow";
    case UT_8250:
      return "8250";
    case UT_16450:
      return "16450";
    case UT_16550:
      return "16550";
    case UT_16550A:
      return "16550A";
    case UT_CIRRUS:
      return "Cirrus";
    case UT_16650:
      return "16650";
    case UT_16650V2:
      return "16650 v2";
    case UT_16750:
      return "16750";
    case UT_STARTECH:
      return "Startech";
    case UT_16C950:
      return "Oxford 16C950";
    case UT_16654:
      return "16654";
    case UT_16850:
      return "16850";
    case UT_RSA:
      return "RSA-DV II/s";
    default:
      return "Unknow";
  }
}

QList<int> mdtAbstractSerialPort::availableBaudRates()
{
  return pvAvailableBaudRates;
}
