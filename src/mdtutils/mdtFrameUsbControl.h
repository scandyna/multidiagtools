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
#ifndef MDT_FRAME_USB_CONTROL_H
#define MDT_FRAME_USB_CONTROL_H

#include "mdtFrame.h"

/*! \brief USB Control frame
 *
 * This is only a partial implementation of
 *  a USB control frame.
 * The USB port API (see mdtUsbPort) uses 
 *  libusb. libusb handles the most needed
 *  part of control transfers.
 * The goal of this class is to send application
 *  specific requests, for ex. USBTMC and USBTMC 488.
 *
 * See the USB 2.0 specifications, section 9.3 for details.
 * See the USB488 specifications for USBTMC details.
 */
class mdtFrameUsbControl : public mdtFrame
{
 public:

  /*! \brief Bits D6, D5 (Type) as defined for bmRequestType
   *
   * See USB 2.0 specifications, section 9.3 for details.
   */
  enum request_type_t {
                        RT_STANDARD = 0,  /*!< Standard */
                        RT_CLASS    = 1,  /*!< Class */
                        RT_VENDOR   = 2,  /*!< Vendor */
                        RT_RESERVED = 3   /*!< Reserved */
                      };

  /*! \brief Bits D4-D0 (Recipient) as defined for bmRequestType
   *
   * See USB 2.0 specifications, section 9.3 for details.
   */
  enum request_recipient_t {
                            RR_DEVICE = 0,      /*!< Device */
                            RR_INTERFACE = 1,   /*!< Interface */
                            RR_ENDPOINT = 2,    /*!< Endpoint */
                            RR_OTHER = 3        /*!< Other */
                           };

  mdtFrameUsbControl();
  ~mdtFrameUsbControl();

  /*! \brief mdtFrame subclass specific clear
   */
  void clearSub();

  /*! \brief Set direction
   *
   * Will write bit D7 in bmRequestType
   *
   * \param deviceToHost If true, direction is Device-to-host, else Host-to-device
   */
  void setDirectionDeviceToHost(bool deviceToHost);

  /*! \brief Get direction
   *
   * Will read bit D7 in bmRequestType
   *
   * \return true if direction is Device-to-host, else Host-to-device
   */
  bool directionIsDeviceToHost() const;

  /*! \brief Set the request type
   *
   * Will write bits D6 and D5 in bmRequestType
   */
  void setRequestType(request_type_t type);

  /*! \brief Get the request type
   *
   * Will read bits D6 and D5 in bmRequestType
   */
  request_type_t requestType() const;

  /*! \brief Set request recipient
   *
   * Will write bits D4 to D0 in bmRequestType
   */
  void setRequestRecipient(request_recipient_t recipient);

  /*! \brief Get request recipient
   *
   * Will read bits D4 to D0 in bmRequestType
   */
  request_recipient_t requestRecipient() const;

  /*! \brief Set bmRequestType
   */
  void setbmRequestType(quint8 requestType);

  /*! \brief Get bmRequestType
   */
  quint8 bmRequestType() const;

  /*! \brief Set the bRequest
   */
  void setbRequest(quint8 request);

  /*! \brief Get the bRequest
   */
  quint8 bRequest() const;

  /*! \brief Set the wValue
   */
  void setwValue(quint16 value);

  /*! \brief Get the wValue
   */
  quint16 wValue() const;

  /*! \brief Set the wIndex
   */
  void setwIndex(quint16 index);

  /*! \brief Get the wIndex
   */
  quint16 wIndex() const;

  /*! \brief Set wLength
   */
  void setwLength(quint16 length);

  /*! \brief Get wLength
   */
  quint16 wLength() const;

  /*! \brief Encode the control frame
   *
   * To use to build a frame that will be sent to device.
   *
   * Note: clear() is called internally.
   */
  void encode();

 private:

  quint8 pvbmRequestType;
  quint8 pvbRequest;
  quint16 pvwValue;
  quint16 pvwIndex;
  quint16 pvwLength;
};

#endif // #ifndef MDT_FRAME_USB_CONTROL_H
