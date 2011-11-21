
#include "mdtSerialPortInterface.h"
#include "mdtSerialPortInterfaceOptAttribute.h"

mdtSerialPortInterface::mdtSerialPortInterface()
{
}

mdtSerialPortInterface::~mdtSerialPortInterface()
{
}

void mdtSerialPortInterface::setUartType(sp_uart_type_t type)
{
  pvUartType = type;
  // Set the string type
  switch(type){
    case UT_UNKNOWN:
      pvUartTypeStr = "Unknow";
      break;

    case UT_8250:
      pvUartTypeStr = "8250";
      break;
    case UT_16450:
      pvUartTypeStr = "16450";
      break;
    case UT_16550:
      pvUartTypeStr = "16550";
      break;
    case UT_16550A:
      pvUartTypeStr = "16550A";
      break;
    case UT_CIRRUS:
      pvUartTypeStr = "Cirrus";
      break;
    case UT_16650:
      pvUartTypeStr = "16650";
      break;
    case UT_16650V2:
      pvUartTypeStr = "16650 v2";
      break;
    case UT_16750:
      pvUartTypeStr = "16750";
      break;
    case UT_STARTECH:
      pvUartTypeStr = "Startech";
      break;
    case UT_16C950:
      pvUartTypeStr = "Oxford 16C950";
      break;
    case UT_16654:
      pvUartTypeStr = "16654";
      break;
    case UT_16850:
      pvUartTypeStr = "16850";
      break;
    case UT_RSA:
      pvUartTypeStr = "RSA-DV II/s";
      break;
    default:
      pvUartTypeStr = "Unknow";
  }
}

mdtSerialPortInterface::sp_uart_type_t mdtSerialPortInterface::uartType()
{
  return pvUartType;
}


QString mdtSerialPortInterface::uartTypeStr()
{
  return pvUartTypeStr;
}

void mdtSerialPortInterface::setPortNumber(int portNumber)
{
  pvPortNumber = portNumber;
}

int mdtSerialPortInterface::portNumber()
{
  return pvPortNumber;
}

void mdtSerialPortInterface::setPortName(const QString &portName)
{
  pvPortName = portName;
}

QString mdtSerialPortInterface::portName()
{
  return pvPortName;
}

void mdtSerialPortInterface::setAvailableBaudRates(const QList<int> &availableBaudRates)
{
  pvAvailableBaudRates = availableBaudRates;
}

QList<int> mdtSerialPortInterface::availableBaudRates()
{
  return pvAvailableBaudRates;
}
