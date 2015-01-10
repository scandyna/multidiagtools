/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_USBTMC_CONTROL_TRANSFER_H
#define MDT_USBTMC_CONTROL_TRANSFER_H

#include "mdtUsbControlTransfer.h"
#include <QString>
#include <vector>
#include <cstdint>

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC control transfer
 */
class mdtUsbtmcControlTransfer : public mdtUsbControlTransfer
{
 public:

  /*! \brief USBTMC bRequest
   *
   * See Table 15 in USBTMC 1.0 specifications, section 4.2.1.
   */
  enum class USBTMCbRequest : uint8_t
  {
    None = 0,                         /*!< Marked as reserved in specification, but... */
    INITIATE_ABORT_BULK_OUT = 1,      /*!< Abort a Bulk-OUT transfer. */
    CHECK_ABORT_BULK_OUT_STATUS = 2,  /*!< Returns the status of the previously sent INITIATE_ABORT_BULK_OUT request. */
    INITIATE_ABORT_BULK_IN = 3,       /*!< Abort a Bulk-IN transfer. */
    CHECK_ABORT_BULK_IN_STATUS = 4,   /*!< Returns the status of the previously sent INITIATE_ABORT_BULK_IN request. */
    INITIATE_CLEAR = 5,               /*!< Clear bulk-OUT and bulk-IN transfers. */
    CHECK_CLEAR_STATUS = 6,           /*!< Returns the status of previously sent INITIATE_CLEAR request. */
    GET_CAPABILITIES = 7,             /*!< Returns attributes and capabilities of the USBTMC interface. */
    INDICATOR_PULSE = 64              /*!< A mechanism to turn on an activity indicator for identification purposes. */
  };

  /*! \brief USBTMC_status values
   *
   * See Table 16 in USBTMC 1.0 specifications, section 4.2.1.
   * For USB488 specific status, see USBTMC-USB488 1.0 specification, section 4.3
   */
  enum class USBTMCstatus : uint8_t
  {
    STATUS_SUCCESS = 0x01,                  /*!< Success */
    STATUS_PENDING = 0x02,                  /*!< Valid after a USBTMC CHECK_STATUS. See USBTMC 1.0, section 4.2.1.1 */
    STATUS_FAILED = 0x80,                   /*!< Failure, unspecified reason */
    STATUS_TRANSFER_NOT_IN_PROGRESS = 0x81, /*!< Valid after a INITIATE_ABORT_BULK_OUT or INITIATE_ABORT_BULK_IN request and specified transfer abort is not in progress */
    STATUS_SPLIT_NOT_IN_PROGRESS = 0x82,    /*!< Valid if device received a CHECK_STATUS request and device is not processing INITIATE request */
    STATUS_SPLIT_IN_PROGRESS = 0x83,        /*!< Valid if device received a new Class specific request and device is still processing INITIATE */
    STATUS_INTERRUPT_IN_BUSY = 0x20         /*!< USB488: Valid if device received READ_STATUS_BYTE request. */
  };

  /*! \brief Constructor
   *
   * \todo Fix a realistic bufferSize
   *
   * \pre bufferSize must be >= 8.
   */
  mdtUsbtmcControlTransfer(mdtUsbtmcTransferHandler & th, libusb_device_handle *dev_handle, int bufferSize = 1024);

  // Disable copy
  mdtUsbtmcControlTransfer(const mdtUsbtmcControlTransfer & rhs) = delete;
  mdtUsbtmcControlTransfer & operator=(const mdtUsbtmcControlTransfer & rhs) = delete;

  /*! \brief Setup a clear endpoint halt transfer
   *
   * See USBTMC specifications, section 4.1.1 (CLEAR_FEATURE request wValue = ENDPOINT_HALT)
   *
   * \param transferHandler This transferHandler will be passed to user_data in libusb_transfer.
   * \param endpointNumber Endpoint number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Check if this is a CLEAR_FEATURE request with wValue = ENDPOINT_HALT for given endpoint
   */
  bool isClearEndpointHaltRequest(uint8_t endpointNumber) const
  {
    return ( ( (bmRequestType() & 0b00011111) == static_cast<uint8_t>(mdtUsbControlTransfer::RequestRecipient_t::Endpoint) )
             && ( bRequest() == static_cast<uint8_t>(mdtUsbControlTransfer::StandardRequestCode_t::CLEAR_FEATURE) )
             && ( wValue() == static_cast<uint8_t>(mdtUsbControlTransfer::StandardFeatureSelector_t::ENDPOINT_HALT) )
             && ( wIndex() == static_cast<uint16_t>(endpointNumber) )
           );
  }

  /*! \brief Setup a INITIATE_ABORT_BULK_OUT transfer
   *
   * See USBTMC specifications, section 4.2.1.2
   *
   * \param bTag bTag of the bulk out transfer to abort.
   * \param endpointNumber Endpoint number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupInitiateAbortBulkOut(uint8_t bTag, uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Setup a CHECK_ABORT_BULK_OUT_STATUS transfer
   *
   * See USBTMC specifications, section 4.2.1.3
   *
   * \param endpointNumber Endpoint number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupCheckAbortBulkOutStatus(uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Setup a INITIATE_ABORT_BULK_IN transfer
   *
   * See USBTMC specifications, section 4.2.1.4
   *
   * \param bTag bTag of the bulk out transfer to abort.
   * \param endpointNumber Endpoint number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupInitiateAbortBulkIn(uint8_t bTag, uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Setup a CHECK_ABORT_BULK_IN_STATUS transfer
   *
   * See USBTMC specifications, section 4.2.1.5
   *
   * \param endpointNumber Endpoint number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupCheckAbortBulkInStatus(uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Setup a INITIATE_CLEAR transfer
   *
   * See USBTMC specifications, section 4.2.1.6
   *
   * \param interfaceNumber Interface number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupInitiateClear(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Setup a CHECK_CLEAR_STATUS transfer
   *
   * See USBTMC specifications, section 4.2.1.7
   *
   * \param interfaceNumber Interface number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupCheckClearStatus(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Setup a GET_CAPABILITIES transfer
   *
   * See USBTMC specifications, section 4.2.1.8
   *
   * \param interfaceNumber Interface number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupGetCapabilities(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Get bcdUSBTMC
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * BCD version number of the relevant USBTMC specification for this USBTMC interface.
   *  Format is Binary-Coded-Decimal. For example, 0x0210 represents version 2.10.
   *
   * See USBTMC specifications, section 4.2.1.8
   */
  uint16_t bcdUSBTMC() const
  {
    return wordValue(2);
  }

  /*! \brief Check if interface accepts INDICATOR_PULSE
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   */
  bool interfaceAcceptsIndicatorPulse() const
  {
    return (byteValue(4) & 0b00000100);
  }

  /*! \brief Check if interface is talk-only
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   */
  bool interfaceIsTalkOnly() const
  {
    return (byteValue(4) & 0b00000010);
  }

  /*! \brief Check if interface is listen-only
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   */
  bool interfaceIsListenOnly() const
  {
    return (byteValue(4) & 0b00000001);
  }

  /*! \brief Check if device supports TermChar
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   */
  bool deviceSupportsTermChar() const
  {
    return (byteValue(5) & 0b00000001);
  }

  /*! \brief Get bcdUSB488
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * BCD version number of the relevant USB488 specification for this USB488 interface.
   *  Format is Binary-Coded-Decimal. For example, 0x0210 represents version 2.10.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  uint16_t bcdUSB488() const
  {
    return wordValue(12);
  }

  /*! \brief Check if interface is a 488.2 USB488 one
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool interfaceIsUSB488() const
  {
    return (byteValue(14) & 0b00000100);
  }

  /*! \brief Check if interface accepts REN_CONTROL
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool interfaceAcceptsRenControl() const
  {
    return (byteValue(14) & 0b00000010);
  }

  /*! \brief Check if interface accepts MsgID = TRIGGER
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool interfaceAcceptsTrigger() const
  {
    return (byteValue(14) & 0b00000010);
  }

  /*! \brief Check if device supports all mandatory SCPI commands
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool deviceSupportsAllMandatoryScpiCommands() const
  {
    return (byteValue(15) & 0b00001000);
  }

  /*! \brief Check if device is SR1 capable
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool deviceIsSR1Capable() const
  {
    return (byteValue(15) & 0b00000100);
  }

  /*! \brief Check if device is RL1 capable
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool deviceIsRL1Capable() const
  {
    return (byteValue(15) & 0b00000010);
  }

  /*! \brief Check if device is DT1 capable
   *
   * Valid only for GET_CAPABILITIES response.
   *
   * See USBTMC specifications, section 4.2.1.8
   *  and USBTMC-USB488 specification, section 4.2.2
   */
  bool deviceIsDT1Capable() const
  {
    return (byteValue(15) & 0b00000001);
  }

  /*! \brief Setup a INDICATOR_PULSE transfer
   *
   * See USBTMC specifications, section 4.2.1.9
   *
   * \param interfaceNumber Interface number.
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupIndicatorPulse(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Get USBTMC bRequest
   *
   * Is valid just after setupXY() was called,
   *  and after transfer is done, thanks to libusb wich does not overwrite it.
   */
  inline USBTMCbRequest usbtmcRequest() const
  {
    return static_cast<USBTMCbRequest>(byteValue(1));
  }

  /*! \brief Get USBTMC bRequest text
   */
  QString usbtmcRequestText() const;

  /*! \brief Get USBTMC_status
   *
   * Notes:
   *  - No check is made about actualLength(). It is the caller responsability to be shure that response contains expected data length.
   *  - Returned status is simply a cast from value that device returned. Don't forget a default section in your switch()
   */
  inline USBTMCstatus usbtmcStatus() const
  {
    return static_cast<USBTMCstatus>(byteValue(8));
  }

  /*! \brief Get USBTMC_status text
   */
  QString usbtmcStatusText() const;

  /*! \brief Get transfer handler
   */
  mdtUsbtmcTransferHandler & transferHandler()
  {
    return pvTransferHandler;
  }

 private:

  mdtUsbtmcTransferHandler & pvTransferHandler;
};

#endif // #ifndef MDT_USBTMC_CONTROL_TRANSFER_H
