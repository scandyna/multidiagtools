
#include "mdtSerialPortConfig.h"
#include <QtGlobal>
#include <QDebug>

mdtSerialPortConfig::mdtSerialPortConfig()
{
  setDefault();
}

mdtSerialPortConfig::~mdtSerialPortConfig()
{
}

void mdtSerialPortConfig::setDefault()
{
  mdtPortConfig::setDefault();
  pvBaudRate = 9600;
  pvParity = NoParity;
  pvDataBitCount = 8;
  pvStopBitCount = 1;
  pvFlowCtlRtsCtsEnabled = false;
  pvFlowCtlXonXoffEnabled = false;
  pvXonChar = MDT_DEF_XON;
  pvXoffChar = MDT_DEF_XOFF;
  pvIsValid = false;
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

bool mdtSerialPortConfig::operator==(const mdtSerialPortConfig &other)
{
  return matches(other);
}

bool mdtSerialPortConfig::operator!=(const mdtSerialPortConfig &other)
{
  return !matches(other);
}

bool mdtSerialPortConfig::matches(const mdtSerialPortConfig &other)
{
  if(pvBaudRate != other.pvBaudRate){
    return false;
  }
  if(pvParity != other.pvParity){
    return false;
  }
  if(pvDataBitCount != other.pvDataBitCount){
    return false;
  }
  if(pvDataBitCount != other.pvDataBitCount){
    return false;
  }
  if(pvStopBitCount != other.pvStopBitCount){
    return false;
  }
  if(pvFlowCtlRtsCtsEnabled != other.pvFlowCtlRtsCtsEnabled){
    return false;
  }
  if(pvFlowCtlXonXoffEnabled != other.pvFlowCtlXonXoffEnabled){
    return false;
  }
  if(pvXonChar != other.pvXonChar){
    return false;
  }
  if(pvXoffChar != other.pvXoffChar){
    return false;
  }
  if(pvIsValid != other.pvIsValid){
    return false;
  }
  return true;
}
