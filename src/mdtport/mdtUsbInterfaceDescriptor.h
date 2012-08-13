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
#ifndef MDT_USB_INTERFACE_DESCRIPTOR_H
#define MDT_USB_INTERFACE_DESCRIPTOR_H

#include <libusb-1.0/libusb.h>
#include <QtGlobal>
#include <QList>

class mdtUsbEndpointDescriptor;

/*! \brief USB interface descriptor
 * \todo list by ref
 */
class mdtUsbInterfaceDescriptor
{
 public:

  mdtUsbInterfaceDescriptor();
  ~mdtUsbInterfaceDescriptor();

  /*! \brief Ftech the interface descriptor's attributes
   *
   * \pre descriptor must be a valid pointer
   */
  void fetchAttributes(libusb_interface_descriptor *descriptor);

  /*! \brief INTERFACE Descriptor Type
   */
  quint8 bDescriptorType() const;

  /*! \brief Class code (assigned by the USB-IF)
   */
  quint8 bInterfaceClass() const;

  /*! \brief Subclass code (assigned by the USB-IF)
   */
  quint8 bInterfaceSubClass() const;

  /*! \brief Protocol code (assigned by the USB)
   */
  quint8 bInterfaceProtocol() const;

  /*! \brief Index of string descriptor describing this interface
   */
  quint8 iInterface() const;

  /*! \brief List of avaliable enpoints
   */
  QList<mdtUsbEndpointDescriptor*> endpoints();

 private:

  quint8 pvbDescriptorType;
  quint8 pvbInterfaceClass;
  quint8 pvbInterfaceSubClass;
  quint8 pvbInterfaceProtocol;
  quint8 pviInterface;
  QList<mdtUsbEndpointDescriptor*> pvEndpoints;
};

#endif  // #ifndef MDT_USB_INTERFACE_DESCRIPTOR_H
