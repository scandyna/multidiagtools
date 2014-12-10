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
 :  pvLibusbDevice(0),
    pvIsEmpty(false)
{
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));
}

void mdtUsbDeviceDescriptor::clear()
{
  ::memset(&pvDescriptor, 0, sizeof(pvDescriptor));
  pvIsEmpty = true;
  pvSerialNumber.clear();
  pvConfigs.clear();
  pvLibusbDevice = 0;
}

bool mdtUsbDeviceDescriptor::fetchAttributes(libusb_device *device, bool fetchActiveConfigOnly)
{
  Q_ASSERT(device != 0);

  struct libusb_config_descriptor *configDescriptor;
  libusb_device_handle *handle;
  int err;
  uint8_t i;
  unsigned char uSerialNumber[256] = {'\0'};
  char serialNumber[256] = {'\0'};
  int serialNumberLength;

  clear();
  pvLibusbDevice = device;
  // Open descriptor
  err = libusb_get_device_descriptor(pvLibusbDevice, &pvDescriptor);
  if(err != 0){
    pvLastError.setError(QObject::tr("Failed to get device descriptor."), mdtError::Error);
    pvLastError.setSystemError(err, libusb_error_name(err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceDescriptor");
    pvLastError.commit();
    return false;
  }
  // Try to get serial number
  handle = 0;
  if(libusb_open(pvLibusbDevice, &handle) == 0){
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
  if(fetchActiveConfigOnly){
    err = libusb_get_active_config_descriptor(pvLibusbDevice, &configDescriptor);
    if(err != 0){
      pvLastError.setError(QObject::tr("Failed to get active configuration descriptor."), mdtError::Error);
      pvLastError.setSystemError(err, libusb_error_name(err));
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceDescriptor");
      pvLastError.commit();
      return false;
    }
    Q_ASSERT(configDescriptor != 0);
    pvConfigs.append(mdtUsbConfigDescriptor(*configDescriptor));
    libusb_free_config_descriptor(configDescriptor);
  }else{
    for(i = 0; i < pvDescriptor.bNumConfigurations; ++i){
      err = libusb_get_config_descriptor(pvLibusbDevice, i, &configDescriptor);
      if(err == 0){
        Q_ASSERT(configDescriptor != 0);
        pvConfigs.append(mdtUsbConfigDescriptor(*configDescriptor));
        libusb_free_config_descriptor(configDescriptor);
      }else{
        pvLastError.setError(QObject::tr("Failed to get a configuration descriptor."), mdtError::Warning);
        pvLastError.setSystemError(err, libusb_error_name(err));
        MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceDescriptor");
        pvLastError.commit();
      }
    }
  }
  if(pvConfigs.isEmpty()){
    pvLastError.setError(QObject::tr("Failed to get any configuration descriptor."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceDescriptor");
    pvLastError.commit();
    return false;
  }
  pvIsEmpty = false;

  return true;
}

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

QString mdtUsbDeviceDescriptor::idString() const
{
  QString str;

  if(pvIsEmpty){
    return str;
  }
  // Currently, we format like lsusb
  str = "ID ";
  str += QString("0x%1:0x%2 ").arg(pvDescriptor.idVendor, 4, 16, QChar('0')).arg(pvDescriptor.idProduct, 4, 16, QChar('0'));
  str += vendorName() + " " + productName();
  if(!pvSerialNumber.isEmpty()){
    str += " (SN:" + pvSerialNumber + ")";
  }

  return str;
}

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
