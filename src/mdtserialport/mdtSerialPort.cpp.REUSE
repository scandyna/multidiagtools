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
#include "mdtSerialPort.h"
#include "mdtFrameAscii.h"

mdtSerialPort::mdtSerialPort(QObject *parent)
 : mdtSerialPortSys(parent)
{
}

mdtSerialPort::~mdtSerialPort()
{
}

/*
bool mdtSerialPort::open(mdtSerialPortConfig &cfg)
{
  pvMutex.lock();
  if(!mdtSerialPortSys::open(cfg)){
    pvMutex.unlock();
    return false;
  }
  pvMutex.unlock();

  return true;
}
*/

bool mdtSerialPort::carIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCarIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::dsrIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvDsrIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::ctsIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCtsIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::rngIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRngIsOn;
  pvMutex.unlock();

  return retVal;
}

void mdtSerialPort::setRts(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setRts(on);
  pvMutex.unlock();
}

void mdtSerialPort::setDtr(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setDtr(on);
  pvMutex.unlock();
}
