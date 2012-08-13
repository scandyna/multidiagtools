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
#ifndef MDT_USB_CONFIG_DESCRIPTOR_H
#define MDT_USB_CONFIG_DESCRIPTOR_H

#include <QtGlobal>
#include <QList>
#include <libusb-1.0/libusb.h>

class mdtUsbInterfaceDescriptor;

/*! \brief USB configuration descriptor
 */
class mdtUsbConfigDescriptor
{
 public:

  mdtUsbConfigDescriptor();
  ~mdtUsbConfigDescriptor();

  /*! \brief Ftech the configuration descriptor's attributes
   *
   * \pre descriptor must be a valid pointer
   */
  void fetchAttributes(struct libusb_config_descriptor *descriptor);

  /*! \brief CONFIGURATION Descriptor Type
   */
  quint8 bDescriptorType() const;

  /*! \brief Value to use as an argument to the SetConfiguration() request to select this configuration
   */
  quint8 bConfigurationValue() const;

  /*! \brief Index of string descriptor describing this configuration
   */
  quint8 iConfiguration() const;

  /*! \brief Configuration characteristics
   *
   * \sa isSelfPowered()
   * \sa supportsRemoteWakeup()
   */
  quint8 bmAttributes() const;

  /*! \brief Return true if device is self powered with this configuration
   *
   * This is bit D6 from bmAttributes()
   * \sa bmAttributes()
   */
  bool isSelfPowered() const;

  /*! \brief Return true if device supports remote wakeup with this configuration
   *
   * This is bit D5 from bmAttributes()
   * \sa bmAttributes()
   */
  bool supportsRemoteWakeup() const;

  /*! \brief Max power in 2 [mA] units
   *
   * \sa maxPower()
   */
  quint8 bMaxPower() const;

  /*! \brief Max power [mA]
   */
  int maxPower() const;

  QList<mdtUsbInterfaceDescriptor*> interfaces();

 private:

  quint8 pvbDescriptorType;
  quint8 pvbConfigurationValue;
  quint8 pviConfiguration;
  quint8 pvbmAttributes;
  quint8 pvbMaxPower;
  QList<mdtUsbInterfaceDescriptor*> pvInterfaces;
};

#endif  // #ifndef MDT_USB_CONFIG_DESCRIPTOR_H
