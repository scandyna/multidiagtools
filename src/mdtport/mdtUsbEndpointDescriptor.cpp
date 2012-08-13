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
#include "mdtUsbEndpointDescriptor.h"

#include <QDebug>

mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor()
{
  pvbEndpointAddress = 0;
  pvbmAttributes = 0;
  pvwMaxPacketSize = 0;
  pvbInterval = 0;
}

mdtUsbEndpointDescriptor::~mdtUsbEndpointDescriptor()
{
}

void mdtUsbEndpointDescriptor::fetchAttributes(libusb_endpoint_descriptor *descriptor)
{
  Q_ASSERT(descriptor != 0);

  // Get attributes
  pvbEndpointAddress = descriptor->bEndpointAddress;
  pvbmAttributes = descriptor->bmAttributes;
  pvwMaxPacketSize = descriptor->wMaxPacketSize;
  pvbInterval = descriptor->bInterval;
}

quint8 mdtUsbEndpointDescriptor::address()
{
  return (pvbEndpointAddress & 0x0F);
}

bool mdtUsbEndpointDescriptor::isDirectionIN()
{
  return (pvbEndpointAddress & 0x80);
}

bool mdtUsbEndpointDescriptor::isDirectionOUT()
{
  return !isDirectionIN();
}

bool mdtUsbEndpointDescriptor::isTransfertTypeControl()
{
  return ((pvbmAttributes & 0x03) == 0x00);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeIsochronus()
{
  return ((pvbmAttributes & 0x03) == 0x01);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeBulk()
{
  return ((pvbmAttributes & 0x03) == 0x02);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeInterrupt()
{
  return ((pvbmAttributes & 0x03) == 0x03);
}

bool mdtUsbEndpointDescriptor::isAsynchronous()
{
  return ((pvbmAttributes & 0x0C) == 0x04);
}

bool mdtUsbEndpointDescriptor::isAdaptative()
{
  return ((pvbmAttributes & 0x0C) == 0x08);
}

bool mdtUsbEndpointDescriptor::isSynchronous()
{
  return ((pvbmAttributes & 0x0C) == 0x0C);
}

bool mdtUsbEndpointDescriptor::isDataEndpoint()
{
  return ((pvbmAttributes & 0x30) == 0x00);
}

bool mdtUsbEndpointDescriptor::isFeedbackEndpoint()
{
  return ((pvbmAttributes & 0x30) == 0x10);
}

bool mdtUsbEndpointDescriptor::isImplicitFeedbackEndpoint()
{
  return ((pvbmAttributes & 0x30) == 0x20);
}

int mdtUsbEndpointDescriptor::maxPacketSize()
{
  return pvwMaxPacketSize & 0x07FF;
}

int mdtUsbEndpointDescriptor::transactionsCountPerMicroFrame()
{
  return ((pvwMaxPacketSize & 0x1800) >> 10) + 1;
}

quint8 mdtUsbEndpointDescriptor::bInterval()
{
  return pvbInterval;
}

