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
#ifndef MDT_USB_CONTROL_TRANSFER_H
#define MDT_USB_CONTROL_TRANSFER_H

#include "mdtUsbTransfer.h"
#include <QtGlobal>
#include <cstring>
#include <vector>
#include <cstdint>

/*! \brief USB control transfer
 */
class mdtUsbControlTransfer : public mdtUsbTransfer
{
 public:

  /*! \brief Request recipient
   *
   * Represents bits D4...0 in bmRequestType field.
   *
   * See Table 9-2 in USB 2.0 specifications, section 9.3,
   *  or Table 9-3 in USB 3.1 specifications,section 9.3 for details.
   */
  enum class RequestRecipient_t : uint8_t
  {
    Device = 0,     /*!< Recipient is device */
    Interface = 1,  /*!< Recipient is a interface */
    Endpoint = 2,   /*!< Recipient is a endpoint */
    Other = 3       /*!< Other recipient */
  };

  /*! \brief Standard Request Codes
   *
   * See Table 9-4 in USB 2.0 specifications, section 9.4,
   *  or Table 9-5 in USB 3.1 specifications,section 9.4 for details.
   *
   * \todo Complete this enum.
   */
  enum class StandardRequestCode_t : uint8_t
  {
    GET_STATUS = 0,             /*!<  */
    CLEAR_FEATURE = 1           /*!< Clear feature. USB 2.0/3.1 spec. section 9.4.1 */
  };

  /*! \brief Descriptor Types
   *
   * See Table 9-5 in USB 2.0 specifications, section 9.4,
   *  or Table 9-6 in USB 3.1 specifications,section 9.4 for details.
   *
   * \todo Complete this enum.
   */
  enum class DescriptorType_t : uint8_t
  {
    DEVICE = 1,             /*!<  */
    CONFIGURATION = 2,      /*!<  */
    STRING = 3,             /*!<  */
    INTERFACE = 4,          /*!<  */
    ENDPOINT = 5            /*!<  */
  };

  /*! \brief Standard Feature Selectors
   *
   * See Table 9-6 in USB 2.0 specifications, section 9.4,
   *  or Table 9-7 in USB 3.1 specifications,section 9.4 for details.
   *
   * \todo Complete this enum (USB 3.1 missing)
   */
  enum class StandardFeatureSelector_t : uint16_t
  {
    DEVICE_REMOTE_WAKEUP = 1, /*!< Recipient: Device */
    ENDPOINT_HALT = 0,        /*!< Recipient: Endpoint */
    TEST_MODE = 2             /*!< Recipient: Device */
  };

  /*! \brief Constructor
   *
   * \todo Fix a realistic bufferSize
   *
   * \pre bufferSize must be >= 8.
   */
  mdtUsbControlTransfer(libusb_device_handle *dev_handle, int bufferSize = 1024);

  // Disable copy
  mdtUsbControlTransfer(const mdtUsbControlTransfer & rhs) = delete;
  mdtUsbControlTransfer & operator=(const mdtUsbControlTransfer & rhs) = delete;

  /*! \brief Set value at offset
   *
   * \pre offset must be < internal buffer's capacity
   */
  void setByteValue(unsigned int offset, uint8_t value)
  {
    Q_ASSERT(offset < pvBuffer.capacity());
    pvBuffer[offset] = value;
  }

  /*! \brief Get value at offset
   *
   * \pre offset must be < internal buffer's capacity
   */
  uint8_t byteValue(unsigned int offset) const
  {
    Q_ASSERT(offset < pvBuffer.capacity());
    return pvBuffer[offset];
  }

  /*! \brief Set value at offset
   *
   * \pre offset+1 must be < internal buffer's capacity
   */
  void setWordValue(unsigned int offset, uint16_t value)
  {
    Q_ASSERT((offset + 1) < pvBuffer.capacity());
    pvBuffer[offset] = (value & 0x00FF);
    pvBuffer[offset+1] = (value >> 8);
  }

  /*! \brief Get value at offset
   *
   * \pre offset+1 must be < internal buffer's capacity
   */
  uint16_t wordValue(unsigned int offset) const
  {
    Q_ASSERT((offset + 1) < pvBuffer.capacity());
    return (pvBuffer[offset+1] << 8) + pvBuffer[offset];
  }

  /*! \brief Get bmRequestType part
   */
  uint8_t bmRequestType() const
  {
    return pvBuffer[0];
  }

  /*! \brief Get bRequest part
   */
  uint8_t bRequest() const
  {
    return pvBuffer[1];
  }

  /*! \brief Get wValue
   */
  inline uint16_t wValue() const
  {
    return wordValue(2);
    ///return (pvBuffer[3] << 8) + pvBuffer[2];
  }

  /*! \brief Get wIndex
   */
  inline uint16_t wIndex() const
  {
    return wordValue(4);
    ///return (pvBuffer[5] << 8) + pvBuffer[4];
  }

  /*! \brief Get wLength
   */
  inline uint16_t wLength() const
  {
    return wordValue(6);
    ///return (pvBuffer[7] << 8) + pvBuffer[6];
  }

  /*! \brief Setup a Clear feature transfer
   *
   * \param transferHandler Application specific transfer handler.
   *                         Must provide a static function called controlTransferCallback(),
   *                         with the libusb_transfer_cb_fn signature.
   *                         This transferHandler will be passed to user_data in libusb_transfer.
   * 
   * \param callback A static function that will be called (by libusb) when transfer fihishes.
   *                  As documented in libusb API, this callback will receive a instance of libusb_transfer
   *                  struct. The user_data part of this struct will also contain current transfer class pointer (this pointer).
   * \param recipient Recipient, can be Device, Interface or Endpoint.
   * \param feature Supported feature depends on recipient (see USB 2.0 standards, section 9.4.1).
   * \param recipientNumber Index/number of recipient (f.ex. endpoint number if recipient is a endpoint).
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupClearFeature(libusb_transfer_cb_fn callback, RequestRecipient_t recipient, StandardFeatureSelector_t feature, uint16_t recipientNumber, unsigned int timeout)
  {
    Q_ASSERT((int)pvBuffer.capacity() >= 8);
    fillTransfer(callback,
                 static_cast<uint8_t>(recipient), static_cast<uint8_t>(StandardRequestCode_t::CLEAR_FEATURE), static_cast<uint16_t>(feature), recipientNumber, 0,
                 timeout);
  }

 protected:

  /*! \brief Fill transfer
   *
   * This version is mostly usefull for queries without data,
   *  or with data sended from device to host.
   *
   * For details about USB specific arguments, see USB 3.1 spec, chap. 9.3 .
   *
   * \param transferHandler Application specific transfer handler.
   *                         Must provide a static function called controlTransferCallback(),
   *                         with the libusb_transfer_cb_fn signature.
   *                         This transferHandler will be passed to user_data in libusb_transfer.
   *
   * \param callback A static function that will be called (by libusb) when transfer fihishes.
   *                  As documented in libusb API, this callback will receive a instance of libusb_transfer
   *                  struct. The user_data part of this struct will also contain current transfer class pointer (this pointer).
   * \param bmRequestType Request characteristics (direction, type, recipient). Must be set correctly by caller.
   * \param bRequest Specific request (see USB spec.)
   * \param wValue See USB spec.
   * \param wIndex See USB spec.
   * \param wLength Number of bytes to transfer. Has only sense for direction device-to-host (should be 0 else).
   * \pre Internal buffer size (witch was given to constructor) must be >= 8 + wLength.
   */
  void fillTransfer(libusb_transfer_cb_fn callback,
                    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength,
                    unsigned int timeout);

  /*! \brief Fill transfer
   *
   * This version is mostly usefull for queries with data sended from host to device.
   *
   * For details about USB specific arguments, see USB 3.1 spec, chap. 9.3 .
   *
   * \param transferHandler Application specific transfer handler.
   *                         Must provide a static function called controlTransferCallback(),
   *                         with the libusb_transfer_cb_fn signature.
   *                         This transferHandler will be passed to user_data in libusb_transfer.
   *
   * \param callback A static function that will be called (by libusb) when transfer fihishes.
   *                  As documented in libusb API, this callback will receive a instance of libusb_transfer
   *                  struct. The user_data part of this struct will also contain current transfer class pointer (this pointer).
   * \param bmRequestType Request characteristics (direction, type, recipient). Must be set correctly by caller.
   * \param bRequest Specific request (see USB spec.)
   * \param wValue See USB spec.
   * \param wIndex See USB spec.
   * \param data Data that will be transferred to the device. Note: will also set wLength regarding data length.
   * \pre Internal buffer size (witch was given to constructor) must be >= 8 + data length.
   */
  void fillTransfer(libusb_transfer_cb_fn callback,
                    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, const std::vector<unsigned char> & data,
                    unsigned int timeout);

 private:

  std::vector<unsigned char> pvBuffer;
};

#endif // #ifndef MDT_USB_CONTROL_TRANSFER_H
