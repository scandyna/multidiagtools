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
#ifndef MDT_USB_ENDPOINT_DESCRIPTOR_H
#define MDT_USB_ENDPOINT_DESCRIPTOR_H

#include <QtGlobal>
#include <QString>
#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <vector>

/*! \brief USB endpoint descriptor
 */
class mdtUsbEndpointDescriptor
{
 public:

  /*! \brief Construct a empty endpoint descriptor
   */
  mdtUsbEndpointDescriptor();

  /*! \brief Constructor
   *
   * Will copy all members of passed descriptor.
   *  This descriptor can be freed after construction was done.
   */
  mdtUsbEndpointDescriptor(libusb_endpoint_descriptor descriptor);

  /*! \brief Check if descriptor is empty
   */
  bool isEmpty() const{
    return pvIsEmpty;
  }

  ///~mdtUsbEndpointDescriptor();

  /*! \brief Ftech the endpoint descriptor's attributes
   *
   * \pre descriptor must be a valid pointer
   */
  ///void fetchAttributes(libusb_endpoint_descriptor *descriptor);

  /*! \brief Endpoint address (bEndpointAddress)
   */
  uint8_t address() const{
    return pvDescriptor.bEndpointAddress;
  }

  /*! \brief Get endpoint number
   *
   * Returns bits 3..0 extracted from bEndpointAddress.
   */
  uint8_t number() const{
    return (pvDescriptor.bEndpointAddress & 0x0F);
  }

  /*! \brief Endpoint direction
   */
  bool isDirectionIN() const{
    return (pvDescriptor.bEndpointAddress & 0x80);
  }

  /*! \brief Endpoint direction
   */
  bool isDirectionOUT() const{
    return !isDirectionIN();
  }

  /*! \brief Endpoint transfert type
   */
  bool isTransfertTypeControl();

  /*! \brief Endpoint transfert type
   */
  bool isTransfertTypeIsochronus();

  /*! \brief Endpoint transfert type
   */
  bool isTransferTypeBulk() const{
    return ((pvDescriptor.bmAttributes & 0x03) == 0x02);
  }

  /*! \brief Endpoint transfert type
   */
  bool isTransferTypeInterrupt() const{
    return ((pvDescriptor.bmAttributes & 0x03) == 0x03);
  }

  /*! \brief Endpoint synchronization type
   */
  bool isAsynchronous();

  /*! \brief Endpoint synchronization type
   */
  bool isAdaptative();

  /*! \brief Endpoint synchronization type
   */
  bool isSynchronous();

  /*! \brief Endpoint usage type
   */
  bool isDataEndpoint();

  /*! \brief Endpoint usage type
   */
  bool isFeedbackEndpoint();

  /*! \brief Endpoint usage type
   */
  bool isImplicitFeedbackEndpoint();

  /*! \brief Maximum packet size
   *
   * Note: this is the decoded size,
   *   i.e. bits 10..0 extracted from wMaxPacketSize
   */
  int maxPacketSize();

  /*! \brief Transactions per microframe
   *
   * Note: this is the decoded value + 1,
   *   i.e. bits 12..11 extracted from wMaxPacketSize
   *  Bit 12..11 give additionnal value, and this method
   *   returns the extracted value + 1
   *   (f.ex. if bit 12..11 give 00 as result, this method returns 1)
   */
  int transactionsCountPerMicroFrame();

  /*! \brief Interval for polling endpoint for data transfers
   *
   * Value is returned as read in bInterval field (in libusb strcture)
   */
  uint8_t bInterval();

  /*! \brief Extra (unknown) descriptors
   */
  std::vector<unsigned char> extra() const
  {
    return pvExtra;
  }

 private:

  libusb_endpoint_descriptor pvDescriptor;
  std::vector<unsigned char> pvExtra;
  bool pvIsEmpty;

  /// \todo Obselete
  /**
  quint8 pvbEndpointAddress;
  quint8 pvbmAttributes;
  quint16 pvwMaxPacketSize;
  quint8 pvbInterval;
  */
};

#endif  // #ifndef MDT_USB_ENDPOINT_DESCRIPTOR_H
