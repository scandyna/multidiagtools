/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
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

int mdtSerialPortConfig::baudRate() const
{
  return pvBaudRate;
}

void mdtSerialPortConfig::setParity(mdtSerialPortConfig::parity_t _parity)
{
  pvParity = _parity;
}

mdtSerialPortConfig::parity_t mdtSerialPortConfig::parity() const
{
  return pvParity;
}

void mdtSerialPortConfig::setDataBitsCount(int n)
{
  pvDataBitCount = n;
}

int mdtSerialPortConfig::dataBitsCount() const
{
  return pvDataBitCount;
}

void mdtSerialPortConfig::setStopBitsCount(int n)
{
  pvStopBitCount = n;
}

int mdtSerialPortConfig::stopBitsCount() const
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

bool mdtSerialPortConfig::flowCtlRtsCtsEnabled() const
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

bool mdtSerialPortConfig::flowCtlXonXoffEnabled() const
{
  return pvFlowCtlXonXoffEnabled;
}

char mdtSerialPortConfig::xonChar() const
{
  return pvXonChar;
}

char mdtSerialPortConfig::xoffChar() const
{
  return pvXoffChar;
}

bool mdtSerialPortConfig::isValid() const
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
  return mdtPortConfig::matches(other);
}
