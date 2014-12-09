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
#include "mdtUsbDeviceDescriptor.h"
#include "mdtError.h"
#include <QString>
#include <cstring>

#include <QDebug>

mdtUsbDeviceDescriptor::mdtUsbDeviceDescriptor()
 : pvIsEmpty(false)
{
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));

  /**
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
  */
  ///pviSerialNumber = 0;
}

mdtUsbDeviceDescriptor::~mdtUsbDeviceDescriptor()
{
  ///qDeleteAll(pvConfigs);
}

int mdtUsbDeviceDescriptor::fetchAttributes(libusb_device *device, bool fetchActiveConfigOnly)
{
  Q_ASSERT(device != 0);

  ///struct libusb_device_descriptor descriptor;
  struct libusb_config_descriptor *configDescriptor;
  libusb_device_handle *handle;
  int err;
  quint8 i;
  ///mdtUsbConfigDescriptor *config;
  unsigned char uSerialNumber[256] = {'\0'};
  char serialNumber[256] = {'\0'};
  int serialNumberLength;

  pvIsEmpty = true;
  // Open descriptor
  err = libusb_get_device_descriptor(device, &pvDescriptor);
  if(err != 0){
    return err;
  }
  // Store attributes
  /**
  pvbDescriptorType = descriptor.bDescriptorType;
  pvbcdUSB = descriptor.bcdUSB;
  pvbDeviceClass = descriptor.bDeviceClass;
  pvbDeviceSubClass = descriptor.bDeviceSubClass;
  pvbDeviceProtocol = descriptor.bDeviceProtocol;
  pvbMaxPacketSize0 = descriptor.bMaxPacketSize0;
  pvidVendor = descriptor.idVendor;
  pvidProduct = descriptor.idProduct;
  pvbcdDevice = descriptor.bcdDevice;
  */
  // Try to get serial number
  handle = 0;
  if(libusb_open(device, &handle) == 0){
    Q_ASSERT(handle != 0);
    serialNumberLength = libusb_get_string_descriptor_ascii(handle, pvDescriptor.iSerialNumber, uSerialNumber, 255);
    if(serialNumberLength > 0){
      Q_ASSERT(serialNumberLength < 256);
      for(i=0; i<serialNumberLength; i++){
        serialNumber[i] = (char)uSerialNumber[i];
      }
      pvSerialNumber = serialNumber;
    }
    libusb_close(handle);
  }
  // Get configuration(s)
  ///qDeleteAll(pvConfigs);
  pvConfigs.clear();
  if(fetchActiveConfigOnly){
    err = libusb_get_active_config_descriptor(device, &configDescriptor);
    switch(err){
      case 0:
        Q_ASSERT(configDescriptor != 0);
        pvConfigs.append(mdtUsbConfigDescriptor(*configDescriptor));
        libusb_free_config_descriptor(configDescriptor);
        /**
        config = new mdtUsbConfigDescriptor;
        Q_ASSERT(config != 0);
        config->fetchAttributes(configDescriptor);
        libusb_free_config_descriptor(configDescriptor);
        pvConfigs.append(config);
        */
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
    for(i = 0; i < pvDescriptor.bNumConfigurations; i++){
      err = libusb_get_config_descriptor(device, i, &configDescriptor);
      switch(err){
        case 0:
          Q_ASSERT(configDescriptor != 0);
          pvConfigs.append(mdtUsbConfigDescriptor(*configDescriptor));
          libusb_free_config_descriptor(configDescriptor);
          /**
          config = new mdtUsbConfigDescriptor;
          Q_ASSERT(config != 0);
          config->fetchAttributes(configDescriptor);
          libusb_free_config_descriptor(configDescriptor);
          pvConfigs.append(config);
          */
          break;
        case LIBUSB_ERROR_NOT_FOUND:
        {
          mdtError e(MDT_USB_IO_ERROR, "Unable to find get configuration descriptor at index " + QString::number(i), mdtError::Warning);
          MDT_ERROR_SET_SRC(e, "mdtUsbDeviceDescriptor");
          e.commit();
          break;
        }
        default:
          ///qDeleteAll(pvConfigs);
          pvConfigs.clear();
          return err;
      }
    }
  }
  pvIsEmpty = false;

  return 0;
}

/**
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
*/

QString mdtUsbDeviceDescriptor::vendorName() const
{
  switch(pvDescriptor.idVendor){
    case 2391:
      return "Agilent Technologies, Inc.";
    case 1133:
      return "Logitech Inc.";
    case 14627:
      return "National Instruments";
    case 1470:
      return "Tyco Electronics";
    default:
      return "VID: 0x" + QString::number(pvDescriptor.idVendor, 16);
  }
}

/**
quint16 mdtUsbDeviceDescriptor::idProduct() const
{
  return pvidProduct;
}
*/

QString mdtUsbDeviceDescriptor::productName() const
{
  QString defStr = "PID: 0x" + QString::number(pvDescriptor.idProduct, 16);

  switch(pvDescriptor.idVendor){
    // Agilent products
    case 2391:
      switch(pvDescriptor.idProduct){
        case 0x0588:
          return "DSO1000 series oscilloscope";
        case 0x4d18:
          return "U3606A Multimeter/DC Power Supply";
        default:
          return defStr;
      }
    // Unknown vendor
    default:
      return defStr;
  }
}

QString mdtUsbDeviceDescriptor::serialNumber() const
{
  return pvSerialNumber;
}

/**
quint16 mdtUsbDeviceDescriptor::bcdDevice() const
{
  return pvbcdDevice;
}
*/

/**
QList<mdtUsbConfigDescriptor*> &mdtUsbDeviceDescriptor::configurations()
{
  return pvConfigs;
}
*/

mdtUsbInterfaceDescriptor mdtUsbDeviceDescriptor::interface(int configIndex, int ifaceIndex)
{
  if(configIndex < 0){
    return mdtUsbInterfaceDescriptor();
  }
  if(configIndex >= pvConfigs.size()){
    return mdtUsbInterfaceDescriptor();
  }
  if(ifaceIndex < 0){
    return mdtUsbInterfaceDescriptor();
  }
  mdtUsbConfigDescriptor configDescriptor = pvConfigs.at(configIndex);
  ///Q_ASSERT(configDescriptor != 0);
  if(ifaceIndex >= configDescriptor.interfaces().size()){
    return mdtUsbInterfaceDescriptor();
  }

  return configDescriptor.interfaces().at(ifaceIndex);
}

mdtUsbEndpointDescriptor mdtUsbDeviceDescriptor::endpoint(int configIndex, int ifaceIndex, int endpointIndex)
{
  mdtUsbInterfaceDescriptor interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor.isEmpty()){
    return mdtUsbEndpointDescriptor();
  }
  if(endpointIndex < 0){
    return mdtUsbEndpointDescriptor();
  }
  if(endpointIndex >= interfaceDescriptor.endpoints().size()){
    return mdtUsbEndpointDescriptor();
  }

  return interfaceDescriptor.endpoints().at(endpointIndex);
}

mdtUsbEndpointDescriptor mdtUsbDeviceDescriptor::firstBulkOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbEndpointDescriptor endpointDescriptor;
  int i;

  mdtUsbInterfaceDescriptor interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor.isEmpty()){
    return mdtUsbEndpointDescriptor();
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor.endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor.endpoints().at(i);
    ///Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction OUT and Bulk transfert
    if((endpointDescriptor.isDirectionOUT())&&(endpointDescriptor.isTransfertTypeBulk())){
      // Here we found a bulk out endpoint. If needed, check if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor.isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found
  return mdtUsbEndpointDescriptor();
}

mdtUsbEndpointDescriptor mdtUsbDeviceDescriptor::firstBulkInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbEndpointDescriptor endpointDescriptor;
  int i;

  mdtUsbInterfaceDescriptor interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor.isEmpty()){
    return mdtUsbEndpointDescriptor();
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor.endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor.endpoints().at(i);
    ///Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction IN and Bulk transfert
    if((endpointDescriptor.isDirectionIN())&&(endpointDescriptor.isTransfertTypeBulk())){
      // Here we found a bulk in endpoint. If needed, check if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor.isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found
  return mdtUsbEndpointDescriptor();
}

mdtUsbEndpointDescriptor mdtUsbDeviceDescriptor::firstInterruptOutEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbEndpointDescriptor endpointDescriptor;
  int i;

  mdtUsbInterfaceDescriptor interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor.isEmpty()){
    return mdtUsbEndpointDescriptor();
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor.endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor.endpoints().at(i);
    ///Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction OUT and Interrupt transfert
    if((endpointDescriptor.isDirectionOUT())&&(endpointDescriptor.isTransfertTypeInterrupt())){
      // Here we found a interrupt out endpoint. If needed, chack if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor.isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found
  return mdtUsbEndpointDescriptor();
}

mdtUsbEndpointDescriptor mdtUsbDeviceDescriptor::firstInterruptInEndpoint(int configIndex, int ifaceIndex, bool dataEndpointOnly)
{
  mdtUsbEndpointDescriptor endpointDescriptor;
  int i;

  mdtUsbInterfaceDescriptor interfaceDescriptor = interface(configIndex, ifaceIndex);
  if(interfaceDescriptor.isEmpty()){
    return mdtUsbEndpointDescriptor();
  }
  // Search in available enpoints
  for(i=0; i<interfaceDescriptor.endpoints().size(); i++){
    endpointDescriptor = interfaceDescriptor.endpoints().at(i);
    ///Q_ASSERT(endpointDescriptor != 0);
    // Check if current endpoint is direction IN and Interrupt transfert
    if((endpointDescriptor.isDirectionIN())&&(endpointDescriptor.isTransfertTypeInterrupt())){
      // Here we found a interrupt in endpoint. If needed, chack if it's a data endpoint
      if(dataEndpointOnly){
        if(endpointDescriptor.isDataEndpoint()){
          return endpointDescriptor;
        }
      }else{
        return endpointDescriptor;
      }
    }
  }

  // Nothing found
  return mdtUsbEndpointDescriptor();
}
