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
#include "mdtUsbInterfaceDescriptor.h"
#include <cstring>

mdtUsbInterfaceDescriptor::mdtUsbInterfaceDescriptor()
 : pvIsEmpty(true)
{
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));
  /**
  pvbDescriptorType = 0;
  pvbInterfaceClass = 0;
  pvbInterfaceSubClass = 0;
  pvbInterfaceProtocol = 0;
  pviInterface = 0;
  */
}

mdtUsbInterfaceDescriptor::mdtUsbInterfaceDescriptor ( libusb_interface_descriptor descriptor )
{
  uint8_t i;

  pvDescriptor = descriptor;
  for(i = 0; i < pvDescriptor.bNumEndpoints; ++i){
    pvEndpoints.append(mdtUsbEndpointDescriptor(pvDescriptor.endpoint[i]));
  }
  pvIsEmpty = false;
}

void mdtUsbInterfaceDescriptor::clear()
{
  pvEndpoints.clear();
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));
  pvIsEmpty = true;
}

/**
mdtUsbInterfaceDescriptor::~mdtUsbInterfaceDescriptor()
{
  qDeleteAll(pvEndpoints);
}
*/

// void mdtUsbInterfaceDescriptor::fetchAttributes(libusb_interface_descriptor *descriptor)
// {
//   Q_ASSERT(descriptor != 0);
// 
//   quint8 i;
//   ///mdtUsbEndpointDescriptor *endpointDescriptor;
// 
//   // Get attributes
//   pvbDescriptorType = descriptor->bDescriptorType;
//   pvbInterfaceClass = descriptor->bInterfaceClass;
//   pvbInterfaceSubClass = descriptor->bInterfaceSubClass;
//   pvbInterfaceProtocol = descriptor->bInterfaceProtocol;
//   pviInterface = descriptor->iInterface;
//   // Get endpoints
//   Q_ASSERT(descriptor->endpoint != 0);
//   ///qDeleteAll(pvEndpoints);
//   pvEndpoints.clear();
//   for(i=0; i<descriptor->bNumEndpoints; i++){
//     pvEndpoints.append(mdtUsbEndpointDescriptor(descriptor->endpoint[i]));
//     ///endpointDescriptor = new mdtUsbEndpointDescriptor(descriptor->endpoint[i]);
//     ///endpointDescriptor = new mdtUsbEndpointDescriptor;
//     ///Q_ASSERT(endpointDescriptor != 0);
//     ///endpointDescriptor->fetchAttributes((libusb_endpoint_descriptor*)&descriptor->endpoint[i]);
//     ///pvEndpoints.append(endpointDescriptor);
//   }
// }

/**
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
*/

/**
QList<mdtUsbEndpointDescriptor*> &mdtUsbInterfaceDescriptor::endpoints()
{
  return pvEndpoints;
}
*/

QList< mdtUsbEndpointDescriptor > mdtUsbInterfaceDescriptor::bulkInEndpoints() const
{
  QList<mdtUsbEndpointDescriptor> lst;

  for(auto epd : pvEndpoints){
    if((epd.isDirectionIN())&&(epd.isTransferTypeBulk())){
      lst.append(epd);
    }
  }

  return lst;
}

QList< mdtUsbEndpointDescriptor > mdtUsbInterfaceDescriptor::bulkOutEndpoints() const
{
  QList<mdtUsbEndpointDescriptor> lst;

  for(auto epd : pvEndpoints){
    if((epd.isDirectionOUT())&&(epd.isTransferTypeBulk())){
      lst.append(epd);
    }
  }

  return lst;
}

QList< mdtUsbEndpointDescriptor > mdtUsbInterfaceDescriptor::interruptInEndpoints() const
{
  QList<mdtUsbEndpointDescriptor> lst;

  for(auto epd : pvEndpoints){
    if((epd.isDirectionIN())&&(epd.isTransferTypeInterrupt())){
      lst.append(epd);
    }
  }

  return lst;
}

QList< mdtUsbEndpointDescriptor > mdtUsbInterfaceDescriptor::interruptOutEndpoints() const
{
  QList<mdtUsbEndpointDescriptor> lst;

  for(auto epd : pvEndpoints){
    if((epd.isDirectionOUT())&&(epd.isTransferTypeInterrupt())){
      lst.append(epd);
    }
  }

  return lst;
}
