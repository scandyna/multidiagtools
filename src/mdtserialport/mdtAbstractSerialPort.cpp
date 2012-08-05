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
#include "mdtAbstractSerialPort.h"

#include <QDebug>

mdtAbstractSerialPort::mdtAbstractSerialPort(QObject *parent)
 : mdtAbstractPort(parent)
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

void mdtAbstractSerialPort::close()
{
  qDebug() << "mdtAbstractSerialPort::close()";
  pvAvailableBaudRates.clear();
  pvUartType = UT_UNKNOW;
  mdtAbstractPort::close();
}

mdtSerialPortConfig &mdtAbstractSerialPort::config()
{
  Q_ASSERT(pvConfig != 0);

  mdtSerialPortConfig *c = dynamic_cast<mdtSerialPortConfig*>(pvConfig);
  Q_ASSERT(c != 0);

  return *c;
}

mdtAbstractSerialPort::sp_uart_type_t mdtAbstractSerialPort::uartType()
{
  if((pvUartType == UT_UNKNOW)&&(isOpen())){
    mapUartType();
  }

  return pvUartType;
}

QString mdtAbstractSerialPort::uartTypeStr()
{
  switch(uartType()){
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

QList<int> &mdtAbstractSerialPort::availableBaudRates()
{
  if((pvAvailableBaudRates.isEmpty())&&(isOpen())){
    buildAvailableBaudRates();
  }

  return pvAvailableBaudRates;
}

bool mdtAbstractSerialPort::carIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCarIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtAbstractSerialPort::dsrIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvDsrIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtAbstractSerialPort::ctsIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCtsIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtAbstractSerialPort::rngIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRngIsOn;
  pvMutex.unlock();

  return retVal;
}
