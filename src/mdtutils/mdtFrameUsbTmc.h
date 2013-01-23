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
#ifndef MDT_FRAME_USB_TMC_H
#define MDT_FRAME_USB_TMC_H

#include "mdtFrame.h"
#include <QByteArray>

/*! \brief USBTMC frame
 *
 * Used by mdtport API to send en receive frames
 *  in USBTMC standard.
 *
 * References:
 *  - USBTMC Revision 1.0 (2003)
 *  - http://www.usb.org/developers/devclass_docs#approved
 *  - http://www.usb.org/developers/devclass_docs/USBTMC_1_006a.zip
 */
class mdtFrameUsbTmc : public mdtFrame
{
 public:

  /*! \brief USBTMC MsgID
   */
  enum msg_id_t{
                MSG_ZERO = 0,               /*!< Reserved. */
                DEV_DEP_MSG_OUT = 1,        /*!< USBTMC device dependent command message */
                DEV_DEP_MSG_IN = 2,         /*!< USBTMC response message.
                                                  Represent the REQUEST_DEV_DEP_MSG_IN if frame is sent from host (OUT) */
                VENDOR_SPECIFIC_OUT = 126,  /*!< USBTMC vendor specific command message */
                VENDOR_SPECIFIC_IN = 127    /*!< USBTMC vendor specific response message on the Bulk-IN endpoint.
                                                  Represent the REQUEST_VENDOR_SPECIFIC_IN if frame is sent from host (OUT) */
               };

  mdtFrameUsbTmc();
  ~mdtFrameUsbTmc();

  /*! \brief mdtFrame subclass specific clear
   */
  void clearSub();

  /*! \brief Put data into the frame
   *
   * This method is called by the read thread, and should not be used directly.
   */
  int putData(const char *data, int maxLen);

  /*! \brief Set the MsgID
   *
   * To use to build a frame that will be sent to device.
   */
  void setMsgID(msg_id_t MsgID);

  /*! \brief Get the MsgID
   *
   * Usefull for frames received from device.
   *  Is only valid when the frame is complete.
   */
  msg_id_t MsgID() const;

  /*! \brief Set the bTag
   *
   * The bTag should be incremented each time a new USBTMC message is sent,
   *  (each time a new USBTMC header is sent).
   *
   * To use to build a frame that will be sent to device.
   *
   * \pre bTag must be <> 0
   */
  void setbTag(quint8 bTag);

  /*! \brief Get the bTag
   *
   * Usefull for frames received from device.
   *  Is only valid when the frame is complete.
   */
  quint8 bTag() const;

  /*! \brief Set the EOM flag
   *
   * EOM: End Of (USBTMC) Message.
   *
   * If true, tells the device that this USBTMC transfer is the last in message.
   */
  void setEOM(bool EOM);

  /*! \brief Get the EOM flag
   *
   * If true, this transfer is the last in current message.
   *  Is only valid when the frame is complete.
   */
  bool isEOM() const;

  /*! \brief Set transfer size
   *
   * To use with DEV_DEP_MSG_IN request.
   */
  void setTransferSize(int size);

  /*! \brief Set the message data
   *
   * To use to build a frame that will be sent to device.
   */
  void setMessageData(const QByteArray &data);

  /*! \brief Get the message data
   *
   * Usefull for frames received from device.
   *  Is only valid when the frame is complete.
   */
  QByteArray &messageData();

  /*! \brief Encode the USBTMC frame
   *
   * To use to build a frame that will be sent to device.
   *
   * Note: clear() is called internally.
   */
  void encode();

 private:

  quint8 pvMsgID;
  quint8 pvbTag;
  quint8 pvbTagInverse;
  quint32 pvTransferSize;
  quint8 pvbmTransferAttributes;
  char pvTermChar;
  bool pvEOM;
  QByteArray pvMessageData;
};

#endif  // #ifndef MDT_FRAME_USB_TMC_H
