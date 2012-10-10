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
#ifndef MDT_FRAME_CODEC_MODBUS_H
#define MDT_FRAME_CODEC_MODBUS_H

#include "mdtFrameCodec.h"
#include <QList>

/*! \brief Encode and decode MODBUS PDU  NOTE: stockage des adresses durant Q/R ?
 * 
 * This class works only on MODBUS PDU.
 * To transmit MODBUS/TCP frame, the resulting PDU can be
 * sent using mdtFrameModbusTcp class, 
 * or received PDU decoded here.
 * 
 * References:
 *  - MODBUS Application Protocol Specification V1.1b
 *  - http://www.Modbus-IDA.org
 * 
 * \sa mdtFrameCodec
 * \sa mdtFrameModbusTcp
 */
class mdtFrameCodecModbus : public mdtFrameCodec
{
 public:

  /*! \brief MODBUS response error code
   */
  enum modbus_error_code_t{
    UNKNOW_ERROR,             /*!< Error is unknow from codec */
    ILLEGAL_FUNCTION = 1,     /*!< Code 0x01: Function is not allowed for the server */
    ILLEGAL_DATA_ADDRESS,     /*!< Code 0x02: Request address is not in range of the server */
    ILLEGAL_DATA_VALUE,       /*!< Code 0x03: Request value is not in allowed from server */
    SLAVE_DEVICE_FAILURE,     /*!< Code 0x04: Unrecoverable failue on server */
    ACKNOWLEDGE,              /*!< Code 0x05: Request accepted, but mor time is needed (server side) for response */
    SLAVE_DEVICE_BUSY,        /*!< Code 0x06: Server busy, client should try to resend the query later */
    RESERVE_ERROR_7,          /*!< Not used in current specification */
    MEMORY_PARITY_ERROR,      /*!< Code 0x08: Parity error related to code function 20 and 21 */
    RESERVE_ERROR_9,          /*!< Not used in current specification */
    GATEWAY_PATH_UNAVAILABLE, /*!< Code 0x0A: Error from a gateway (misconfigured or overloaded) */
    GATEWAY_TARGET_RESPONSE   /*!< Code 0x0B: Error from a gateway: target device not present on network */
  };

  mdtFrameCodecModbus();
  ~mdtFrameCodecModbus();

  /*! \brief Encode a PDU according the ReadCoils function (0x01)
   * 
   * Please take a look at MODBUS specifications for more details.
   * \return A QByteArray containing the PDU, or a empty QByteArray on error.
   */
  QByteArray encodeReadCoils(quint16 startAddress, quint16 n);

  /*! \brief Encode a PDU according the WriteSingleCoil function (0x05)
   * 
   * Please take a look at MODBUS specifications for more details.
   * \return A QByteArray containing the PDU, or a empty QByteArray on error.
   */
  QByteArray encodeWriteSingleCoil(quint16 address, bool state);

  /*! \brief Decode a MODBUS PDU
   * 
   * This should reconize the function code in PDU,
   * and call the appropriate decode methode (internall).
   * Once decode is done, the values are available with mdtFrameCodec::values().
   * To know what was decoded, the function code is returned.<br>
   * The values are stored in a QList\<QVariant\> , with format depending of the
   * function code:
   *  - For a ReadCoils response, values will be bool, sorted by ascending addresses.
   *
   * If PDU contains a error code, the list of values will be empty, and this error code will be returned.<br>
   * If a MODBUS error code is decoded, this error code is returned as in frame (for. ex: 0x81 for a ReadCoils error)
   * To know whitch MODBUS error was returned, use lastModbusError()
   *
   * \param pdu The MODBUS PDU to decode
   * \return Decoded function code, MODBUS error code or value < 0 on other error.
   */
  int decode(const QByteArray &pdu);

  /*! \brief Get the last MODBUS error code returned
   * 
   * Returns the error code according to the specification, part function/error codes (same value).
   * \sa decode()
   */
  modbus_error_code_t lastModbusError();

 private:

  bool decodeReadCoils();

  bool decodeWriteSingleCoil();

  // Append exception code to mdtError , and store it to pvLastModbusError
  int decodeModbusError(quint8 error);
  
  void appendValuesBitsFromByte(quint8 byte);
  
  // Build a byte with states - First state will be stored in LSB, last state is MSB
  // Precondition: states must contain exactly 8 values
  quint8 byteFromBooleans(QList<bool> &states);
  
  QByteArray pvPdu;
  modbus_error_code_t pvLastModbusError;
};

#endif  // #ifndef MDT_FRAME_CODEC_MODBUS_H
