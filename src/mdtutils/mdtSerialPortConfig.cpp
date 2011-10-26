
#include "mdtSerialPortConfig.h"

mdtSerialPortConfig::mdtSerialPortConfig()
{
}

mdtSerialPortConfig::~mdtSerialPortConfig()
{
}

void mdtSerialPortConfig::setInterface(const QString &iface)
{
  pvInterface = iface;
}

QString mdtSerialPortConfig::interface()
{
  return pvInterface;
}

void mdtSerialPortConfig::setBaudRate(int baudRate)
{
  pvBaudRate = baudRate;
}

int mdtSerialPortConfig::baudRate()
{
  return pvBaudRate;
}

void mdtSerialPortConfig::setParity(mdtSerialPortConfig::parity_t _parity)
{
  pvParity = _parity;
}

mdtSerialPortConfig::parity_t mdtSerialPortConfig::parity()
{
  return pvParity;
}

void mdtSerialPortConfig::setDataBitsCount(int n)
{
  pvDataBitCount = n;
}

int mdtSerialPortConfig::dataBitsCount()
{
  return pvDataBitCount;
}

void mdtSerialPortConfig::setStopBitsCount(int n)
{
  pvStopBitCount = n;
}

int mdtSerialPortConfig::stopBitsCount()
{
  return pvStopBitCount;
}

void mdtSerialPortConfig::enableFlowCtlRtsCts()
{
  pvFlowCtlRtsCtsEnabled = true;
}

void mdtSerialPortConfig::diseableFlowCtlRtsCts()
{
  pvFlowCtlRtsCtsEnabled = false;
}

bool mdtSerialPortConfig::flowCtlRtsCtsEnabled()
{
  return pvFlowCtlRtsCtsEnabled;
}

void mdtSerialPortConfig::enableFlowCtlXonXoff(char _xonChar, char _xoffChar)
{
  pvFlowCtlXonXoffEnabled = true;
  pvXonChar = _xonChar;
  pvXoffChar = _xoffChar;
}

void mdtSerialPortConfig::diseableFlowCtlXonXoff()
{
  pvFlowCtlXonXoffEnabled = false;
}

bool mdtSerialPortConfig::flowCtlXonXoffEnabled()
{
  return pvFlowCtlXonXoffEnabled;
}

char mdtSerialPortConfig::xonChar()
{
  return pvXonChar;
}

char mdtSerialPortConfig::xoffChar()
{
  return pvXoffChar;
}

bool mdtSerialPortConfig::isValid()
{
  return false;
}

