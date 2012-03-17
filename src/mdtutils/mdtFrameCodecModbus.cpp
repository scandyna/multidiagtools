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
#include "mdtFrameCodecModbus.h"
#include "mdtError.h"
#include <QString>

#include <QDebug>

mdtFrameCodecModbus::mdtFrameCodecModbus()
{
  pvLastModbusError = UNKNOW_ERROR;
}

mdtFrameCodecModbus::~mdtFrameCodecModbus()
{
}

QByteArray mdtFrameCodecModbus::encodeReadCoils(quint16 startAddress, quint16 n)
{
  pvPdu.clear();

  // Check n (1 to 2000)
  if((n < 1)||(n > 2000)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request coils count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }

  // Function code
  pvPdu.append(0x01);
  // Start address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Number of coils
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);

  return pvPdu;
}

int mdtFrameCodecModbus::decode(const QByteArray &pdu)
{
  pvPdu = pdu;
  pvValues.clear();
  pvLastModbusError = UNKNOW_ERROR;
  quint8 functionCode;

  // Case of empty PDU
  if(pvPdu.size() < 2){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Request a decode of a empty PDU (size < 2 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return -1;
  }
  // Get the function code
  functionCode = pvPdu.at(0);
  // Case of error code
  if(functionCode > 0x80){
    decodeModbusError(pvPdu.at(1));
    return functionCode;
  }
  // Decode PDU according functionCode
  switch(functionCode){
    // Read coils
    case 0x01:
      if(!decodeReadCoils()){
        return -1;
      }
      return 0x01;
      break;
    // Unknow code
    default:
      mdtError e(MDT_FRAME_DECODE_ERROR, "Unknow function code", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
      e.commit();
      return -1;
  }

  return functionCode;
}

mdtFrameCodecModbus::modbus_error_code_t mdtFrameCodecModbus::lastModbusError()
{
  return pvLastModbusError;
}

bool mdtFrameCodecModbus::decodeReadCoils()
{
  int bytesCount;
  int i;

  // Case of not enougth data
  if(pvPdu.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains no data (size < 3 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get bytes count and check it's validity NOTE: voir N* dans la spec !!
  bytesCount = pvPdu.at(1);
  if(bytesCount != (pvPdu.size() -2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get the values
  for(i=2; i<pvPdu.size(); i++){
    //qDebug() << "Byte[" << i << "]: " << (int)pvPdu.at(i);
    appendValuesBitsFromByte(pvPdu.at(i));
  }

  return true;
}

int mdtFrameCodecModbus::decodeModbusError(quint8 error)
{
  QString errorText;

  // We only keep the 4 LSBs
  switch(error & 0x0F){
    // Illegal function
    case 0x01:
      errorText = "Code 0x01: Function is not allowed for the server";
      pvLastModbusError = ILLEGAL_FUNCTION;
      break;
    case 0x02:
      errorText = "Code 0x02: Request address is not in range of the server";
      pvLastModbusError = ILLEGAL_DATA_ADDRESS;
      break;
    case 0x03:
      errorText = "Code 0x03: Request value is not in allowed from server";
      pvLastModbusError = ILLEGAL_DATA_VALUE;
      break;
    case 0x04:
      errorText = "Code 0x04: Unrecoverable failue on server";
      pvLastModbusError = SLAVE_DEVICE_FAILURE;
      break;
    case 0x05:
      errorText = "Code 0x05: Request accepted, but mor time is needed (server side) for response";
      pvLastModbusError = ACKNOWLEDGE;
      break;
    case 0x06:
      errorText = "Code 0x06: Server busy, client should try to resend the query later";
      pvLastModbusError = SLAVE_DEVICE_BUSY;
      break;
    case 0x07:
      errorText = "?? Error code not used in current specification";
      pvLastModbusError = RESERVE_ERROR_7;
      break;
    case 0x08:
      errorText = "Code 0x08: Parity error related to code function 20 and 21";
      pvLastModbusError = MEMORY_PARITY_ERROR;
      break;
    case 0x09:
      errorText = "?? Error code not used in current specification";
      pvLastModbusError = RESERVE_ERROR_9;
      break;
    case 0x0A:
      errorText = "Code 0x0A: Error from a gateway (misconfigured or overloaded)";
      pvLastModbusError = GATEWAY_PATH_UNAVAILABLE;
      break;
    case 0x0B:
      errorText = "Code 0x0B: Error from a gateway: target device not present on network";
      pvLastModbusError = GATEWAY_TARGET_RESPONSE;
      break;
    default:
      errorText = "Codec's unknow error code";
      pvLastModbusError = UNKNOW_ERROR;
  }
  // Append error
  mdtError e(MDT_FRAME_DECODE_ERROR, "PDU returned from server contains a exception code", mdtError::Error);
  e.setSystemError(pvLastModbusError, errorText);
  MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
  e.commit();
}

void mdtFrameCodecModbus::appendValuesBitsFromByte(quint8 byte)
{
  bool state;

  // Get bit states from LSB to MSB
  state = byte & 0x01;
  pvValues.append(state);
  state = byte & 0x02;
  pvValues.append(state);
  state = byte & 0x04;
  pvValues.append(state);
  state = byte & 0x08;
  pvValues.append(state);
  state = byte & 0x10;
  pvValues.append(state);
  state = byte & 0x20;
  pvValues.append(state);
  state = byte & 0x40;
  pvValues.append(state);
  state = byte & 0x80;
  pvValues.append(state);
}
