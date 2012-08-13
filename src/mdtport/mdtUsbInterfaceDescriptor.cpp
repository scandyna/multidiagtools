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
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"

mdtUsbInterfaceDescriptor::mdtUsbInterfaceDescriptor()
{
  pvbDescriptorType = 0;
  pvbInterfaceClass = 0;
  pvbInterfaceSubClass = 0;
  pvbInterfaceProtocol = 0;
  pviInterface = 0;
}

mdtUsbInterfaceDescriptor::~mdtUsbInterfaceDescriptor()
{
  qDeleteAll(pvEndpoints);
}

void mdtUsbInterfaceDescriptor::fetchAttributes(libusb_interface_descriptor *descriptor)
{
  Q_ASSERT(descriptor != 0);

  quint8 i;
  mdtUsbEndpointDescriptor *endpointDescriptor;

  // Get attributes
  pvbDescriptorType = descriptor->bDescriptorType;
  pvbInterfaceClass = descriptor->bInterfaceClass;
  pvbInterfaceSubClass = descriptor->bInterfaceSubClass;
  pvbInterfaceProtocol = descriptor->bInterfaceProtocol;
  pviInterface = descriptor->iInterface;
  // Get endpoints
  Q_ASSERT(descriptor->endpoint != 0);
  qDeleteAll(pvEndpoints);
  pvEndpoints.clear();
  for(i=0; i<descriptor->bNumEndpoints; i++){
    endpointDescriptor = new mdtUsbEndpointDescriptor;
    Q_ASSERT(endpointDescriptor != 0);
    endpointDescriptor->fetchAttributes((libusb_endpoint_descriptor*)&descriptor->endpoint[i]);
    pvEndpoints.append(endpointDescriptor);
  }
}

quint8 mdtUsbInterfaceDescriptor::bDescriptorType() const
{
  return pvbDescriptorType;
}

quint8 mdtUsbInterfaceDescriptor::bInterfaceClass() const
{
  return pvbInterfaceClass;
}

quint8 mdtUsbInterfaceDescriptor::bInterfaceSubClass() const
{
  return pvbInterfaceSubClass;
}

quint8 mdtUsbInterfaceDescriptor::bInterfaceProtocol() const
{
  return pvbInterfaceProtocol;
}

quint8 mdtUsbInterfaceDescriptor::iInterface() const
{
  return pviInterface;
}

QList<mdtUsbEndpointDescriptor*> &mdtUsbInterfaceDescriptor::endpoints()
{
  return pvEndpoints;
}

