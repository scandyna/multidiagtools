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
#include "mdtUsbDeviceDescriptor.h"
#include "mdtError.h"
#include <QString>

#include <QDebug>

mdtUsbDeviceDescriptor::mdtUsbDeviceDescriptor()
{
  pvbDescriptorType = 0;
  pvbcdUSB = 0;
  pvbDeviceClass = 0;
  pvbDeviceSubClass = 0;
  pvbDeviceProtocol = 0;
  pvbMaxPacketSize0 = 0;
  pvidVendor = 0;
  pvidProduct = 0;
  pvbcdDevice = 0;
  pviManufactuer = 0;
  pviProduct = 0;
  pviSerialNumber = 0;
}

mdtUsbDeviceDescriptor::~mdtUsbDeviceDescriptor()
{
  qDeleteAll(pvConfigs);
}

int mdtUsbDeviceDescriptor::fetchAttributes(libusb_device *device, bool fetchActiveConfigOnly)
{
  Q_ASSERT(device != 0);

  struct libusb_device_descriptor descriptor;
  struct libusb_config_descriptor *configDescriptor;
  int err;
  quint8 i;
  mdtUsbConfigDescriptor *config;

  // Open descriptor
  err = libusb_get_device_descriptor(device, &descriptor);
  if(err != 0){
    return err;
  }
  // Store attributes
  pvbDescriptorType = descriptor.bDescriptorType;
  pvbcdUSB = descriptor.bcdUSB;
  pvbDeviceClass = descriptor.bDeviceClass;
  pvbDeviceSubClass = descriptor.bDeviceSubClass;
  pvbDeviceProtocol = descriptor.bDeviceProtocol;
  pvbMaxPacketSize0 = descriptor.bMaxPacketSize0;
  pvidVendor = descriptor.idVendor;
  pvidProduct = descriptor.idProduct;
  pvbcdDevice = descriptor.bcdDevice;
  // Get configuration(s)
  qDeleteAll(pvConfigs);
  pvConfigs.clear();
  if(fetchActiveConfigOnly){
    err = libusb_get_active_config_descriptor(device, &configDescriptor);
    switch(err){
      case 0:
        config = new mdtUsbConfigDescriptor;
        Q_ASSERT(config != 0);
        config->fetchAttributes(configDescriptor);
        libusb_free_config_descriptor(configDescriptor);
        pvConfigs.append(config);
        break;
      case LIBUSB_ERROR_NOT_FOUND:
      {
        mdtError e(MDT_USB_IO_ERROR, "Unable to find get current configuration descriptor", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtUsbDeviceDescriptor");
        e.commit();
        break;
      }
      default:
        return err;
    }
  }else{
    for(i=0; i<descriptor.bNumConfigurations; i++){
      err = libusb_get_config_descriptor(device, i, &configDescriptor);
      switch(err){
        case 0:
          config = new mdtUsbConfigDescriptor;
          Q_ASSERT(config != 0);
          config->fetchAttributes(configDescriptor);
          libusb_free_config_descriptor(configDescriptor);
          pvConfigs.append(config);
          break;
        case LIBUSB_ERROR_NOT_FOUND:
        {
          mdtError e(MDT_USB_IO_ERROR, "Unable to find get configuration descriptor at index " + QString::number(i), mdtError::Warning);
          MDT_ERROR_SET_SRC(e, "mdtUsbDeviceDescriptor");
          e.commit();
          break;
        }
        default:
          qDeleteAll(pvConfigs);
          pvConfigs.clear();
          return err;
      }
    }
  }

  return 0;
}

quint8 mdtUsbDeviceDescriptor::bDescriptorType() const
{
  return pvbDescriptorType;
}

quint16 mdtUsbDeviceDescriptor::bcdUSB() const
{
  return pvbcdUSB;
}

quint8 mdtUsbDeviceDescriptor::bDeviceClass() const
{
  return pvbDeviceClass;
}

quint8 mdtUsbDeviceDescriptor::bDeviceSubClass() const
{
  return pvbDeviceSubClass;
}

quint8 mdtUsbDeviceDescriptor::bDeviceProtocol() const
{
  return pvbDeviceProtocol;
}

quint8 mdtUsbDeviceDescriptor::bMaxPacketSize0() const
{
  return pvbMaxPacketSize0;
}

quint16 mdtUsbDeviceDescriptor::idVendor() const
{
  return pvidVendor;
}

quint16 mdtUsbDeviceDescriptor::idProduct() const
{
  return pvidProduct;
}

quint16 mdtUsbDeviceDescriptor::bcdDevice() const
{
  return pvbcdDevice;
}

QList<mdtUsbConfigDescriptor*> &mdtUsbDeviceDescriptor::configurations()
{
  return pvConfigs;
}

mdtUsbInterfaceDescriptor *mdtUsbDeviceDescriptor::interface(int configIndex, int ifaceIndex)
{
  mdtUsbConfigDescriptor *configDescriptor;

  if(configIndex < 0){
    return 0;
  }
  if(configIndex >= pvConfigs.size()){
    return 0;
  }
  if(ifaceIndex < 0){
    return 0;
  }
  configDescriptor = pvConfigs.at(configIndex);
  Q_ASSERT(configDescriptor != 0);
  if(ifaceIndex >= configDescriptor->interfaces().size()){
    return 0;
  }

  return configDescriptor->interfaces().at(ifaceIndex);
}

mdtUsbEndpointDescriptor *mdtUsbDeviceDescriptor::endpoint(int configIndex, int ifaceIndex, int endpointIndex)
{
  mdtUsbInterfaceDescriptor *interfaceDescriptor;

  interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor == 0){
    return 0;
  }
  if(endpointIndex < 0){
    return 0;
  }
  if(endpointIndex >= interfaceDescriptor->endpoints().size()){
    return 0;
  }

  return interfaceDescriptor->endpoints().at(endpointIndex);
}

mdtUsbEndpointDescriptor *mdtUsbDeviceDescriptor::firstBulkOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbInterfaceDescriptor *interfaceDescriptor;
  mdtUsbEndpointDescriptor *endpointDescriptor;
  int i;

  interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor == 0){
    return 0;
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor->endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor->endpoints().at(i);
    Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction OUT and Bulk transfert
    if((endpointDescriptor->isDirectionOUT())&&(endpointDescriptor->isTransfertTypeBulk())){
      // Here we found a bulk out endpoint. If needed, check if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor->isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found, return a null pointer
  return 0;
}

mdtUsbEndpointDescriptor *mdtUsbDeviceDescriptor::firstBulkInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbInterfaceDescriptor *interfaceDescriptor;
  mdtUsbEndpointDescriptor *endpointDescriptor;
  int i;

  interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor == 0){
    return 0;
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor->endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor->endpoints().at(i);
    Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction IN and Bulk transfert
    if((endpointDescriptor->isDirectionIN())&&(endpointDescriptor->isTransfertTypeBulk())){
      // Here we found a bulk in endpoint. If needed, check if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor->isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found, return a null pointer
  return 0;
}

mdtUsbEndpointDescriptor *mdtUsbDeviceDescriptor::firstInterruptOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbInterfaceDescriptor *interfaceDescriptor;
  mdtUsbEndpointDescriptor *endpointDescriptor;
  int i;

  interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor == 0){
    return 0;
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor->endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor->endpoints().at(i);
    Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction OUT and Interrupt transfert
    if((endpointDescriptor->isDirectionOUT())&&(endpointDescriptor->isTransfertTypeInterrupt())){
      // Here we found a interrupt out endpoint. If needed, chack if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor->isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found, return a null pointer
  return 0;
}

mdtUsbEndpointDescriptor *mdtUsbDeviceDescriptor::firstInterruptInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbInterfaceDescriptor *interfaceDescriptor;
  mdtUsbEndpointDescriptor *endpointDescriptor;
  int i;

  interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor == 0){
    return 0;
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor->endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor->endpoints().at(i);
    Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction IN and Interrupt transfert
    if((endpointDescriptor->isDirectionIN())&&(endpointDescriptor->isTransfertTypeInterrupt())){
      // Here we found a interrupt in endpoint. If needed, chack if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor->isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found, return a null pointer
  return 0;
}
