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
#ifndef MDT_USBTMC_FRAME_H
#define MDT_USBTMC_FRAME_H

#include "mdtUsbtmcMessage.h"
///#include <QByteArray>
#include <cstdint>
#include <vector>

/**
 * \todo Check header whenn receiving data.
*/

/*! \brief USBTMC frame (i.e. USBTMC bulk transfer)
 *
 * Used by USBTMC API to send en receive frames in USBTMC standard
 *  over bulk pipes. The name frame was choosen because
 *  transfer is close to libusb_transfer, and that part is handled by mdtUsbtmcBulkTransfer.
 *
 * References:
 *  - USBTMC Revision 1.0 (2003)
 *  - http://www.usb.org/developers/devclass_docs#approved
 *  - http://www.usb.org/developers/devclass_docs/USBTMC_1_006a.zip
 *
 * To avoid continious alloc/free, the frame has internal fixed capacity.
 * Different capacity and sizes are available. To clarify them,
 *  see a USBTMC frame (bulk transfer) format:
 *
 * -------------------------------------------------------
 * | USBTMC |             Data bytes         | Alignment |
 * | header |       (USBTMC TransferSize)    |  bytes    |
 * -------------------------------------------------------
 *
 * Capacity of the frame is defined as below:
 *  - capacity: USBTMC header + Data bytes + alignemnt bytes (is simply the capacity of internal buffer)
 *
 * Size of the frame is defined as below:
 *  - transferSize: data bytes only (without header and without alignment bytes)
 *  - bufferLength: USBTMC header + data bytes + alignment bytes (useful for Bulk-OUT transfer)
 *
 * Note:
 *  - For Bulk-OUT transfer, use bufferLength to tell libusb how many bytes must be sent to device.
 *  - For Bulk-IN transfer, use capacity to tell libusb how many bytes it can put into buffer.
 */
class mdtUsbtmcFrame
{
 public:

  /*! \brief USBTMC MsgID
   *
   * See Table 2 in USBTMC specifications for details.
   */
  enum class msgId_t : uint8_t{
                                MSG_ZERO = 0,               /*!< Reserved. */
                                DEV_DEP_MSG_OUT = 1,        /*!< USBTMC device dependent command message */
                                DEV_DEP_MSG_IN = 2,         /*!< USBTMC response message.
                                                                  Represent the REQUEST_DEV_DEP_MSG_IN if frame is sent from host (OUT) */
                                VENDOR_SPECIFIC_OUT = 126,  /*!< USBTMC vendor specific command message */
                                VENDOR_SPECIFIC_IN = 127    /*!< USBTMC vendor specific response message on the Bulk-IN endpoint.
                                                                  Represent the REQUEST_VENDOR_SPECIFIC_IN if frame is sent from host (OUT) */
                              };

  /*! \brief Constructor
   *
   * \param bufferSize Internal buffer size.
   * \pre bufferSize must be at least 512 and a multiple of 4
   */
  mdtUsbtmcFrame(int bufferSize);

  /*! \brief Clear frame
   *
   * Will clear header and reset data size
   */
  void clear();

  /*! \brief Get internal buffer capacity
   */
  inline int capacity() const
  {
    return pvBuffer.capacity();
  }

  /*! \brief Access internal buffer
   */
  inline uint8_t *buffer(){
    return pvBuffer.data();
  }

  /*! \brief Get buffer length
   *
   * Returns length of USBTMC header + data + alignment bytes.
   */
  inline int bufferLength() const
  {
    return pvBufferLength;
  }

  /*! \brief Check if tranfer is complete (valid for bulk IN transfer)
   *
   * After a bulk IN transfer completed, tell how many data was written
   *  to buffer. This is typically available from libusb_transfer::actual_length.
   *
   * Note: we assume that libusb handles transfer in one shot,
   *       i.e. that, when transfer is completed, all data that device
   *       has queued are available.
   *
   * Assuming this note and that clear() is called before each transfer,
   *  we can assume that frame is complete if (written - 12) >= transferSize.
   */
  bool isComplete(int written) const
  {
    return (written-12) >= (int)transferSize();
  }

  /*! \brief Set MsgID
   *
   * See Table 2 in USBTMC specifications for details.
   */
  void setMsgID(msgId_t id)
  {
    pvBuffer[0] = static_cast<uint8_t>(id);
  }

  /*! \brief Get MsgID
   *
   * See Table 2 in USBTMC specifications for details.
   */
  msgId_t msgID() const{
    return static_cast<msgId_t>(pvBuffer[0]);
  }

  /*! \brief Set bTag
   *
   * See Table 1 in USBTMC specifications for details.
   * Note: will also set bTagInverse
   */
  void setbTag(uint8_t bTag){
    pvBuffer[1] = bTag;
    pvBuffer[2] = ~bTag;
  }

  /*! \brief Get bTag
   *
   * See Table 1 in USBTMC specifications for details.
   */
  uint8_t bTag() const{
    return pvBuffer[1];
  }

  /*! \brief Get bTagInverse
   *
   * See Table 1 in USBTMC specifications for details.
   */
  uint8_t bTagInverse() const{
    return pvBuffer[2];
  }

  /*! \brief Set TransferSize
   *
   * See Table 3 in USBTMC specifications for details.
   * Note that setting TransferSize manually has only sense
   *  when setting up a frame for DEV_DEP_MSG_IN or VENDOR_SPECIFIC_IN query.
   *  When setting up a frame for DEV_DEP_MSG_OUT or VENDOR_SPECIFIC_OUT,
   *  use setData() witch will also ajust TransferSize.
   *
   * \pre size must be > 0 and <= totalCapacity-12
   */
  void setTransferSize(uint32_t size);

  /*! \brief Set TransferSize to maximum possible size
   *
   * Useful when setting up frame for DEV_DEP_MSG_IN or VENDOR_SPECIFIC_IN query.
   */
  void setTransferSizeToMax()
  {
    // Because buffer size must be multiple of 4 (see constructor), we can simply set capacity()-12
    setTransferSize(capacity() - 12);
  }

  /*! \brief Get TransferSize
   *
   * Returns number of message bytes,
   *  without USBTMC header and without extra alignment bytes.
   *
   * See, for example, Table 3 in USBTMC 1.0 specifications for details.
   */
  uint32_t transferSize() const{
    return pvBuffer[4] + (pvBuffer[5] << 8) + (pvBuffer[6] << 16) + (pvBuffer[7] << 24);
  }

  /*! \brief Set EOM
   *
   * See Table 3 in USBTMC specifications for details.
   */
  void setEOM(bool set){
    if(set){
      pvBuffer[8] |= 0b00000001;
    }else{
      pvBuffer[8] &= 0b11111110;
    }
  }

  /*! \brief Check if EOM is set
   *
   * This has sense when response of DEV_DEP_MSG_IN was received from device.
   * See Table 9 in USBTMC specifications for details.
   */
  bool EOMset() const{
    return (pvBuffer[8] & 0b00000001);
  }

  /*! \brief Set TermChar
   *
   * See Table 4 in USBTMC specifications for details.
   * Note: will also set TermCharEnabled bit in bmTransferAttributes
   */
  void setTermChar(char c){
    pvBuffer[8] |= 0b00000010;
    pvBuffer[9] = static_cast<uint8_t>(c);
  }

  /*! \brief Check if TermChar is set
   *
   * This has sense when response of DEV_DEP_MSG_IN was received from device.
   * See Table 9 in USBTMC specifications for details.
   */
  bool TermCharset() const{
    return (pvBuffer[8] & 0b00000010);
  }

  /*! \brief Reset TermChar
   *
   * See Table 4 in USBTMC specifications for details.
   * Note: will also clear TermCharEnabled bit in bmTransferAttributes
   */
  void resetTermChar(){
    pvBuffer[8] &= 0b11111101;
    pvBuffer[9] = 0;
  }

  /*! \brief Set data
   *
   * Will read data from given message and copy them to internal buffer.
   *  If all data of message can be hold in tranfer, EOM bit will be set, else not.
   *
   * TransferSize will also be adjusted.
   *
   * \pre message must have at least 1 byte of data to read from
   */
  void setData(mdtUsbtmcMessage & message);

  /*! \brief Get data
   *
   * Will write (by copy) data part to given message.
   */
  void getData(mdtUsbtmcMessage & message)
  {
    message.write(pvBuffer.data()+12, transferSize());
  }

  /*! \brief Set data
   *
   * Will copy given ba to internal buffer.
   * Note: will also set TransferSize.
   *
   * \pre ba must not be empty, and its size must be <= capacity-12-alignemnt bytes
   */
  ///void setData(const QByteArray & ba);

  /*! \brief Get data
   *
   * Returns a copy of data part from internal buffer
   */
  ///QByteArray data() const;

 private:

  std::vector<uint8_t> pvBuffer;
  /*
   * Because vector::resize() will erase data,
   * and writing directly to vector::data() or with operator[] will not affect vector::size(),
   * we handle really available size with a separate attribute.
   */
  int pvBufferLength;
};

#endif // #ifndef MDT_USBTMC_FRAME_H
