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
#ifndef MDT_FRAME_MODBUS_TCP_H
#define MDT_FRAME_MODBUS_TCP_H

#include "mdtFrame.h"
#include <QByteArray>

/*! \brief MODBUS/TCP Frame
 * 
 * This encode/decode a MODBUS/TCP frame.
 * Note that function codes, ... , are not
 * handled here. This application layer functions
 * must be done with a frame codec.
 *
 * Ref:
 *  - MODBUS Messaging on TCP/IP Implementation Guide V1.0b
 *  - MODBUS Application Protocol Specification V1.1b
 *  - http://www.Modbus-IDA.org
 */
class mdtFrameModbusTcp : public mdtFrame
{
 public:

  mdtFrameModbusTcp();
  ~mdtFrameModbusTcp();

  /*! \brief mdtFrame subclass specific clear
   */
  void clearSub();

  /*! \brief Overload of mdtFrame::putData()
   * \pre Capacity must be >= 7
   * \sa mdtFrame
   */
  int putData(const char *data, int maxLen);

  /*! \brief Set the transaction ID
   */
  void setTransactionId(quint16 id);

  /*! \brief Get the transaction ID
   */
  quint16 transactionId();

  /*! \brief Get the length of Unit ID + PDU
   */
  quint16 modbusLength();
  
  /*! \brief Set the Unit ID
   */
  void setUnitId(quint8 id);

  /*! \brief Get the Unit ID
   */
  quint8 unitId();

  /*! \brief Set the MODBUS PDU
   */
  void setPdu(const QByteArray &pdu);

  /*! \brief Get the MODBUS PDU
   */
  QByteArray getPdu();

  /*! \brief Encode the MODBUS/TCP frame
   * 
   * This will build the frame with MBAP Header.
   * \pre Capacity must be >= 7
   */
  void encode();

 private:

  // MBAP Header
  quint16 pvTransactionId;
  quint16 pvProtocolId;
  quint8 pvUnitId;
  QByteArray pvPdu;
};

#endif  // #ifndef MDT_FRAME_MODBUS_TCP_H
