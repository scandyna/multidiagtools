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
#include "mdtDevice.h"
#include "mdtError.h"

mdtDevice::mdtDevice(QObject *parent)
 : QObject(parent)
{
  pvIos = 0;
}

mdtDevice::~mdtDevice()
{
}

void mdtDevice::setIos(mdtDeviceIos *ios)
{
  Q_ASSERT(ios != 0);

  pvIos = ios;
}

bool mdtDevice::readAnalogInput(int address)
{
  return false;
}

int mdtDevice::readAnalogInputs()
{
  return -1;
}

void mdtDevice::decodeReadenFrames()
{
}
