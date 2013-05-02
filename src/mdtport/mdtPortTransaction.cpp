/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtPortTransaction.h"

mdtPortTransaction::mdtPortTransaction()
{
  clear();
}

mdtPortTransaction::~mdtPortTransaction()
{
}

void mdtPortTransaction::clear()
{
  pvId = 0;
  pvAnalogIo = 0;
  pvDigitalIo = 0;
  pvType = 0;
  ///pvForMultipleIos = false;
  pvIoCount = 0;
  pvIsInput = false;
  pvIsOutput = false;
  pvData.clear();
  pvIsQueryReplyMode = false;
  pvAddress = 0;
}

void mdtPortTransaction::setId(int id)
{
  pvId = id;
}

int mdtPortTransaction::id() const
{
  return pvId;
}

void mdtPortTransaction::setType(int type)
{
  pvType = type;
}

int mdtPortTransaction::type() const
{
  return pvType;
}

void mdtPortTransaction::setIo(mdtAnalogIo *io, bool isInput)
{
  pvAnalogIo = io;
  pvDigitalIo = 0;
  pvIsInput = isInput;
  pvIsOutput = !isInput;
  pvIoCount = 1;
}

void mdtPortTransaction::setIo(mdtDigitalIo *io, bool isInput)
{
  pvDigitalIo = io;
  pvAnalogIo = 0;
  pvIsInput = isInput;
  pvIsOutput = !isInput;
}

mdtAnalogIo *mdtPortTransaction::analogIo()
{
  return pvAnalogIo;
}

mdtDigitalIo *mdtPortTransaction::digitalIo()
{
  return pvDigitalIo;
}

/**
void mdtPortTransaction::setForMultipleIos(bool forMultipleIos)
{
  pvForMultipleIos = forMultipleIos;
}
*/

/**
bool mdtPortTransaction::forMultipleIos() const
{
  return pvForMultipleIos;
}
*/

void mdtPortTransaction::setIsInput(bool isInput)
{
  pvIsInput = isInput;
  pvIsOutput = !isInput;
}

bool mdtPortTransaction::isInput() const
{
  return pvIsInput;
}

bool mdtPortTransaction::isOutput() const
{
  return pvIsOutput;
}

void mdtPortTransaction::setData(const QByteArray &data)
{
  pvData.clear();
  pvData.append(data.data(), data.size());
}

QByteArray mdtPortTransaction::data() const
{
  return pvData;
}

void mdtPortTransaction::setQueryReplyMode(bool mode)
{
  pvIsQueryReplyMode = mode;
}

bool mdtPortTransaction::isQueryReplyMode() const
{
  return pvIsQueryReplyMode;
}

void mdtPortTransaction::setAddress(int address)
{
  pvAddress = address;
}

int mdtPortTransaction::address() const
{
  return pvAddress;
}

void mdtPortTransaction::setIoCount(int n)
{
  pvIoCount = n;
}

int mdtPortTransaction::ioCount() const
{
  return pvIoCount;
}
