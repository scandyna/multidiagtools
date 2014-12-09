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
#ifndef MDT_USB_INTERFACE_DESCRIPTOR_H
#define MDT_USB_INTERFACE_DESCRIPTOR_H

#include "mdtUsbEndpointDescriptor.h"
#include <libusb-1.0/libusb.h>
#include <QtGlobal>
#include <QList>
#include <cstdint>

/*! \brief USB interface descriptor
 */
class mdtUsbInterfaceDescriptor
{
 public:

  /*! \brief Construct a empty interface descriptor
   */
  mdtUsbInterfaceDescriptor();

  /*! \brief Constructor
   *
   * Will copy all members of passed descriptor.
   *  This given descriptor can be freed after construction was done.
   */
  mdtUsbInterfaceDescriptor(libusb_interface_descriptor descriptor);

  ///~mdtUsbInterfaceDescriptor();

  /*! \brief Check if descriptor is empty
   */
  bool isEmpty() const{
    return pvIsEmpty;
  }

  /*! \brief Ftech the interface descriptor's attributes
   *
   * \pre descriptor must be a valid pointer
   */
  //void fetchAttributes(libusb_interface_descriptor *descriptor);

  /*! \brief INTERFACE Descriptor Type
   */
  uint8_t bDescriptorType() const
  {
    return pvDescriptor.bDescriptorType;
  }

  /*! \brief Class code (assigned by the USB-IF)
   */
  uint8_t bInterfaceClass() const
  {
    return pvDescriptor.bInterfaceClass;
  }

  /*! \brief Subclass code (assigned by the USB-IF)
   */
  uint8_t bInterfaceSubClass() const
  {
    return pvDescriptor.bInterfaceSubClass;
  }

  /*! \brief Protocol code (assigned by the USB)
   */
  uint8_t bInterfaceProtocol() const
  {
    return pvDescriptor.bInterfaceProtocol;
  }

  /*! \brief Index of string descriptor describing this interface
   */
  uint8_t iInterface() const
  {
    return pvDescriptor.iInterface;
  }

  /*! \brief List of avaliable enpoints
   */
  ///QList<mdtUsbEndpointDescriptor*> &endpoints();
  QList<mdtUsbEndpointDescriptor> endpoints() const{
    return pvEndpoints;
  }

 private:

  /**
  quint8 pvbDescriptorType;
  quint8 pvbInterfaceClass;
  quint8 pvbInterfaceSubClass;
  quint8 pvbInterfaceProtocol;
  quint8 pviInterface;
  */
  
  libusb_interface_descriptor pvDescriptor;
  QList<mdtUsbEndpointDescriptor> pvEndpoints;
  bool pvIsEmpty;
};

#endif  // #ifndef MDT_USB_INTERFACE_DESCRIPTOR_H
