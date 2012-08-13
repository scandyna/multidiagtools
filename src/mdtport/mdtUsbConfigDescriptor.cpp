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
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"

mdtUsbConfigDescriptor::mdtUsbConfigDescriptor()
{
  pvbDescriptorType = 0;
  pvbConfigurationValue = 0;
  pviConfiguration = 0;
  pvbmAttributes = 0;
  pvbMaxPower = 0;
}

mdtUsbConfigDescriptor::~mdtUsbConfigDescriptor()
{
  qDeleteAll(pvInterfaces);
}

void mdtUsbConfigDescriptor::fetchAttributes(struct libusb_config_descriptor *descriptor)
{
  Q_ASSERT(descriptor != 0);

  quint8 i;
  int j;
  mdtUsbInterfaceDescriptor *ifaceDescriptor;

  // Store attributes
  pvbDescriptorType = descriptor->bDescriptorType;
  pvbConfigurationValue = descriptor->bConfigurationValue;
  pviConfiguration = descriptor->iConfiguration;
  pvbmAttributes = descriptor->bmAttributes;
  pvbMaxPower = descriptor->MaxPower;
  // Get interfaces
  qDeleteAll(pvInterfaces);
  pvInterfaces.clear();
  for(i=0; i<descriptor->bNumInterfaces; i++){
    Q_ASSERT(descriptor->interface != 0);
    Q_ASSERT(descriptor->interface[i].altsetting != 0);
    // Get interface descriptors
    for(j=0; j<descriptor->interface[i].num_altsetting; j++){
      ifaceDescriptor = new mdtUsbInterfaceDescriptor;
      Q_ASSERT(ifaceDescriptor != 0);
      ifaceDescriptor->fetchAttributes((libusb_interface_descriptor*)&descriptor->interface[i].altsetting[j]);
      pvInterfaces.append(ifaceDescriptor);
    }
  }
}

quint8 mdtUsbConfigDescriptor::bDescriptorType() const
{
  return pvbDescriptorType;
}

quint8 mdtUsbConfigDescriptor::bConfigurationValue() const
{
  return pvbConfigurationValue;
}

quint8 mdtUsbConfigDescriptor::iConfiguration() const
{
  return pviConfiguration;
}

quint8 mdtUsbConfigDescriptor::bmAttributes() const
{
  return pvbmAttributes;
}

bool mdtUsbConfigDescriptor::isSelfPowered() const
{
  return (pvbmAttributes & 0x40);
}

bool mdtUsbConfigDescriptor::supportsRemoteWakeup() const
{
  return (pvbmAttributes & 0x20);
}

quint8 mdtUsbConfigDescriptor::bMaxPower() const
{
  return pvbMaxPower;
}

int mdtUsbConfigDescriptor::maxPower() const
{
  return 2*pvbMaxPower;
}

QList<mdtUsbInterfaceDescriptor*> &mdtUsbConfigDescriptor::interfaces()
{
  return pvInterfaces;
}
