/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include <cstring>

#include <QDebug>

/**
mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor()
{
  pvbEndpointAddress = 0;
  pvbmAttributes = 0;
  pvwMaxPacketSize = 0;
  pvbInterval = 0;
}
*/

mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor()
 : pvIsEmpty(true)
{
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));
  qDebug() << "mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor() - bLength: " << pvDescriptor.bLength;
  qDebug() << "mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor() - bSynchAddress: " << pvDescriptor.bSynchAddress;
  qDebug() << "mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor() - extra_length: " << pvDescriptor.extra_length;
}

mdtUsbEndpointDescriptor::mdtUsbEndpointDescriptor ( libusb_endpoint_descriptor descriptor )
{
  int i;

  pvDescriptor = descriptor;
  for(i = 0; i < descriptor.extra_length; ++i){
    Q_ASSERT(descriptor.extra != 0);
    pvExtra.push_back(descriptor.extra[i]);
  }
  pvIsEmpty = false;
}

/**
mdtUsbEndpointDescriptor::~mdtUsbEndpointDescriptor()
{
}
*/

/**
void mdtUsbEndpointDescriptor::fetchAttributes(libusb_endpoint_descriptor *descriptor)
{
  Q_ASSERT(descriptor != 0);

  // Get attributes
  pvbEndpointAddress = descriptor->bEndpointAddress;
  pvbmAttributes = descriptor->bmAttributes;
  pvwMaxPacketSize = descriptor->wMaxPacketSize;
  pvbInterval = descriptor->bInterval;
}
*/

uint8_t mdtUsbEndpointDescriptor::address()
{
  ///return (pvbEndpointAddress & 0x0F);
  return (pvDescriptor.bEndpointAddress & 0x0F);
}

bool mdtUsbEndpointDescriptor::isDirectionIN()
{
  ///return (pvbEndpointAddress & 0x80);
  return (pvDescriptor.bEndpointAddress & 0x80);
}

bool mdtUsbEndpointDescriptor::isDirectionOUT()
{
  return !isDirectionIN();
}

bool mdtUsbEndpointDescriptor::isTransfertTypeControl()
{
  ///return ((pvbmAttributes & 0x03) == 0x00);
  return ((pvDescriptor.bmAttributes & 0x03) == 0x00);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeIsochronus()
{
  ///return ((pvbmAttributes & 0x03) == 0x01);
  return ((pvDescriptor.bmAttributes & 0x03) == 0x01);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeBulk()
{
  ///return ((pvbmAttributes & 0x03) == 0x02);
  return ((pvDescriptor.bmAttributes & 0x03) == 0x02);
}

bool mdtUsbEndpointDescriptor::isTransfertTypeInterrupt()
{
  ///return ((pvbmAttributes & 0x03) == 0x03);
  return ((pvDescriptor.bmAttributes & 0x03) == 0x03);
}

bool mdtUsbEndpointDescriptor::isAsynchronous()
{
  ///return ((pvbmAttributes & 0x0C) == 0x04);
  return ((pvDescriptor.bmAttributes & 0x0C) == 0x04);
}

bool mdtUsbEndpointDescriptor::isAdaptative()
{
  ///return ((pvbmAttributes & 0x0C) == 0x08);
  return ((pvDescriptor.bmAttributes & 0x0C) == 0x08);
}

bool mdtUsbEndpointDescriptor::isSynchronous()
{
  ///return ((pvbmAttributes & 0x0C) == 0x0C);
  return ((pvDescriptor.bmAttributes & 0x0C) == 0x0C);
}

bool mdtUsbEndpointDescriptor::isDataEndpoint()
{
  ///return ((pvbmAttributes & 0x30) == 0x00);
  return ((pvDescriptor.bmAttributes & 0x30) == 0x00);
}

bool mdtUsbEndpointDescriptor::isFeedbackEndpoint()
{
  ///return ((pvbmAttributes & 0x30) == 0x10);
  return ((pvDescriptor.bmAttributes & 0x30) == 0x10);
}

bool mdtUsbEndpointDescriptor::isImplicitFeedbackEndpoint()
{
  ///return ((pvbmAttributes & 0x30) == 0x20);
  return ((pvDescriptor.bmAttributes & 0x30) == 0x20);
}

int mdtUsbEndpointDescriptor::maxPacketSize()
{
  ///return pvwMaxPacketSize & 0x07FF;
  return (pvDescriptor.wMaxPacketSize & 0x07FF);
}

int mdtUsbEndpointDescriptor::transactionsCountPerMicroFrame()
{
  ///return ((pvwMaxPacketSize & 0x1800) >> 10) + 1;
  return ((pvDescriptor.wMaxPacketSize & 0x1800) >> 10) + 1;
}

uint8_t mdtUsbEndpointDescriptor::bInterval()
{
  ///return pvbInterval;
  return pvDescriptor.bInterval;
}

