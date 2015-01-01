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
#ifndef MDT_USBTMC_BULK_TRANSFER_H
#define MDT_USBTMC_BULK_TRANSFER_H

#include "mdtUsbTransfer.h"
#include "mdtUsbtmcFrame.h"
#include "mdtUsbEndpointDescriptor.h"
#include <QByteArray>
#include <cstdint>

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC bulk transfer
 */
class mdtUsbtmcBulkTransfer : public mdtUsbTransfer
{
 public:

  /*! \brief USBTMC split action
   *
   * Helps to deal with USBTMC split transaction
   *  defined in USBTMC 1.0 specification, section 4.2.1.1
   *
   * This is used by bulk transfer callbacks to know if they have
   *  something to do (submit a INITIATE or a CHECK control query).
   */
  enum class SplitAction_t
  {
    NoAction = 0,       /*!< No action to perform */
    INITIATE_ABORT,     /*!< Begin aborting transfer with INITIATE_ABORT_BULK_[OUT/IN] */
    CHECK_ABORT_STATUS, /*!< Check status with CHECK_ABORT_BULK_[OUT/IN]_STATUS */
    INITIATE_CLEAR,     /*!< Begin INITIATE_CLEAR */
    CHECK_CLEAR_STATUS, /*!< Send a CHECK_CLEAR_STATUS request */
    CLEAR_ENDPOINT_HALT /*!< Send a CLEAR_FEATURE request with wValue = ENDPOINT_HALT */
  };

  /*! \brief Constructor
   *
   * \param th Transfer handler
   * \param endpointDescriptor Endpoint descriptor
   * \param dev_handle A handle to a open device
   * \param bufferSize Internal USBTMC frame's buffer size.
   * \pre endpointDescriptor must not be empty
   * \pre bufferSize must be at least 512 and a multiple of 4
   */
  mdtUsbtmcBulkTransfer(mdtUsbtmcTransferHandler & th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle *dev_handle, int bufferSize = 4096);

  // Disable copy
  mdtUsbtmcBulkTransfer(const mdtUsbtmcBulkTransfer & rhs) = delete;
  mdtUsbtmcBulkTransfer & operator=(const mdtUsbtmcBulkTransfer & rhs) = delete;

  /*! \brief Setup DEV_DEP_MSG_OUT transfer
   *
   * See USBTMC 1.0 specifications, section 3.2.1.1
   *
   * \param bTag See USBTMC 1.0 specifications, Table 1 for details
   * \param message Message in witch data must be transferred.
   * \param responseExpected If message is a query that expects a response, like *IDN?, set this flag.
   * \param timeout Timeout [ms]
   * 
   * \pre bTag must be in range [1;255]
   */
  void setupDevDepMsgOut(uint8_t bTag, mdtUsbtmcTxMessage & message, bool responseExpected, unsigned int timeout);

  /*! \brief Setup REQUEST_DEV_DEP_MSG_IN transfer
   *
   * See USBTMC 1.0 specifications, section 3.2.1.2
   *
   * \param bTag See USBTMC 1.0 specifications, Table 1 for details
   * \param termCharEnabled Set true if required and device support it.
   * \param termChar Term char. Is ignored if termCharEnabled is false.
   * \param timeout Timeout [ms]
   * 
   * \pre bTag must be in range [1;255]
   */
  void setupRequestDevDepMsgIn(uint8_t bTag, bool termCharEnabled, char termChar , unsigned int timeout);

  /*! \brief Setup bulk-IN transfer
   *
   * See USBTMC 1.0 specifications, section 3.3
   *
   * \param timeout Timeout [ms]
   */
  void setupBulkInRequest(unsigned int timeout);

  /*! \brief Get bTag
   */
  inline uint8_t bTag() const
  {
    return pvFrame.bTag();
  }

  /*! \brief Check if bTagInverse is the innverse of bTag
   *
   * Uuseful to check bulk-IN transfers.
   */
  bool bTagIsOk() const
  {
    return (pvFrame.bTag() == static_cast<uint8_t>(~pvFrame.bTagInverse()));
  }

  /*! \brief Get MsgID
   *
   * See Table 2 in USBTMC 1.0 specifications for details.
   */
  inline mdtUsbtmcFrame::msgId_t msgID() const
  {
    return pvFrame.msgID();
  }

  /*! \brief Check if all data anounced in TransferSize by device are available
   *
   * \param rxLength The actual length that libusb set in transfer struct
   */
  bool receivedAllData(int rxLength)
  {
    return pvFrame.isComplete(rxLength);
  }

  /*! \brief Check if this transfer is the last in message
   */
  bool isEOM() const
  {
    return pvFrame.EOMset();
  }

  /*! \brief Get data
   *
   * Will write (by copy) data part to given message.
   */
  void getData(mdtUsbtmcRxMessage & message)
  {
    pvFrame.getData(message);
  }

  /*! \brief Get transfer handler
   */
  mdtUsbtmcTransferHandler & transferHandler()
  {
    return pvTransferHandler;
  }

  /*! \brief Check if a response is expected
   */
  bool responseExpected() const
  {
    return pvResponseExpected;
  }

  /*! \brief Set split action
   */
  void setSplitAction(SplitAction_t action){
    pvSplitAction = action;
  }

  /*! \brief Clear split action
   */
  void clearSplitAction()
  {
    pvSplitAction = SplitAction_t::NoAction;
  }

  /*! \brief Check if a split action is to be performed
   */
  bool splitActionRequired() const
  {
    return (pvSplitAction != SplitAction_t::NoAction);
  }

  /*! \brief Get split action to perform
   */
  SplitAction_t splitAction() const
  {
    return pvSplitAction;
  }

  /*! \brief Setup a custom transfer
   *
   * Used for debug and testing.
   * \pre transferBufferLength must be <= Internal frame capacity
   */
  void dbgSetupCustom(uint8_t msgID, uint8_t bTag, uint8_t bTagInverse, uint32_t transferSize, uint8_t bmTransferAttributes, uint8_t termChar,
                      bool responseExpected, int transferBufferLength, unsigned int timeout);

 private:

  mdtUsbtmcFrame pvFrame;
  mdtUsbtmcTransferHandler & pvTransferHandler;
  mdtUsbEndpointDescriptor pvEndpointDescriptor;
  bool pvResponseExpected;
  SplitAction_t pvSplitAction;
};

#endif // #ifndef MDT_USBTMC_BULK_TRANSFER_H
