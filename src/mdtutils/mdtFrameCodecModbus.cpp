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

QByteArray mdtFrameCodecModbus::encodeReadDiscreteInputs(quint16 startAddress, quint16 n)
{
  pvPdu.clear();

  // Check n (1 to 2000)
  if((n < 1)||(n > 2000)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request inputs count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }

  // Function code
  pvPdu.append(0x02);
  // Start address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Number of inputs
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);

  return pvPdu;
}

QByteArray mdtFrameCodecModbus::encodeReadHoldingRegisters(quint16 startAddress, quint16 n)
{
  pvPdu.clear();

  // Check n (1 to 125)
  if((n < 1)||(n > 125)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request registers count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }

  // Function code
  pvPdu.append(0x03);
  // Start address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Number of registers
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);

  return pvPdu;
}

QByteArray mdtFrameCodecModbus::encodeReadInputRegisters(quint16 startAddress, quint16 n)
{
  pvPdu.clear();

  // Check n (1 to 125)
  if((n < 1)||(n > 125)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request registers count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }

  // Function code
  pvPdu.append(0x04);
  // Start address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Number of registers
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);

  return pvPdu;
}

QByteArray mdtFrameCodecModbus::encodeWriteSingleCoil(quint16 address, bool state)
{
  qDebug() << "mdtFrameCodecModbus::encodeWriteSingleCoil(): address " << address << " , state: " << state;
  pvPdu.clear();

  // Function code
  pvPdu.append(0x05);
  // Coil address
  pvPdu.append(address >> 8);
  pvPdu.append(address & 0x00FF);
  // Coil state
  if(state){
    pvPdu.append(0xFF);
  }else{
    pvPdu.append((char)0);
  }
  pvPdu.append((char)0);

  return pvPdu;
}

QByteArray mdtFrameCodecModbus::encodeWriteSingleRegister(quint16 address, quint16 value)
{
  pvPdu.clear();

  // Function code
  pvPdu.append(0x06);
  // Register address
  pvPdu.append(address >> 8);
  pvPdu.append(address & 0x00FF);
  // Register value
  pvPdu.append(value >> 8);
  pvPdu.append(value & 0x00FF);

  return pvPdu;
}

QByteArray mdtFrameCodecModbus::encodeWriteMultipleCoils(quint16 startAddress, const QList<bool> &states)
{
  quint16 n = states.size();
  int i;
  quint8 nBytes;
  QList<bool> oneByteStates;

  pvPdu.clear();

  // Check qty (1 to 1968)
  if((n < 1)||(n > 1968)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request registers count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }
  // Calculate number of bytes needed
  if((n%8) == 0){
    nBytes = n/8;
  }else{
    nBytes = (n/8)+1;
  }
  // Function code
  pvPdu.append(0x0F);
  // Register starting address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Quantity of coils
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);
  // Bytes count
  pvPdu.append(nBytes);
  // states
  for(i=0; i<states.size(); i++){
    oneByteStates.append(states.at(i));
    // If we have 8 bits, append to PDU
    if(oneByteStates.size() == 8){
      pvPdu.append(byteFromBooleans(oneByteStates));
      oneByteStates.clear();
    }
  }
  // Remaining states
  if(oneByteStates.size() > 0){
    // See if we have to complete
    for(i=oneByteStates.size()-1; i<7; i++){
      oneByteStates.append(false);
    }
    // Add to PDU
    pvPdu.append(byteFromBooleans(oneByteStates));
  }

  return pvPdu;
}

///QByteArray mdtFrameCodecModbus::encodeWriteMultipleRegisters(quint16 startAddress, const QList<quint16> &values)
QByteArray mdtFrameCodecModbus::encodeWriteMultipleRegisters(quint16 startAddress, const QList<int> &values)
{
  quint16 n = values.size();
  int i;

  pvPdu.clear();

  // Check qty (1 to 123)
  if((n < 1)||(n > 123)){
    mdtError e(MDT_FRAME_ENCODE_ERROR, "Request registers count out of allowed range", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return pvPdu;
  }

  // Function code
  pvPdu.append(0x10);
  // Register starting address
  pvPdu.append(startAddress >> 8);
  pvPdu.append(startAddress & 0x00FF);
  // Quantity of registers
  pvPdu.append(n >> 8);
  pvPdu.append(n & 0x00FF);
  // Bytes count
  pvPdu.append(2*n);
  // Add values
  for(i=0; i<values.size(); i++){
    pvPdu.append(((quint16)values.at(i) >> 8));
    pvPdu.append(((quint16)values.at(i) & 0x00FF));
  }

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
    // Read discrete inputs
    case 0x02:
      if(!decodeReadDiscreteInputs()){
        return -1;
      }
      return 0x02;
      break;
    // ReadHoldingRegisters
    case 0x03:
      if(!decodeReadHoldingRegisters()){
        return -1;
      }
      return 0x03;
      break;
    // ReadInputRegisters
    case 0x04:
      if(!decodeReadInputRegisters()){
        return -1;
      }
      return 0x04;
      break;
    // Write single coil
    case 0x05:
      if(!decodeWriteSingleCoil()){
        return -1;
      }
      return 0x05;
      break;
    // Write single register
    case 0x06:
      if(!decodeWriteSingleRegister()){
        return -1;
      }
      return 0x06;
      break;
    // WriteMultipleCoils
    case 0x0F:
      if(!decodeWriteMultipleCoils()){
        return -1;
      }
      return 0x0F;
      break;
    // WriteMultipleRegisters
    case 0x10:
      if(!decodeWriteMultipleRegisters()){
        return -1;
      }
      return 0x10;
      break;
    // Unknow code
    default:
      QByteArray num;
      /// NOTE: \todo Base 16 ??
      num.setNum(functionCode);
      mdtError e(MDT_FRAME_DECODE_ERROR, "Unknow function code: " + num, mdtError::Error);
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
  quint8 bytesCount;
  int i;

  // Case of not enougth data
  if(pvPdu.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains no data (size < 3 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get bytes count and check it's validity
  bytesCount = pvPdu.at(1);
  if(bytesCount != (pvPdu.size() -2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get the values
  for(i=2; i<pvPdu.size(); i++){
    appendValuesBitsFromByte(pvPdu.at(i));
  }

  qDebug() << "mdtFrameCodecModbus::decodeReadCoils(): values: " << pvValues;
  
  return true;
}

bool mdtFrameCodecModbus::decodeReadDiscreteInputs()
{
  quint8 bytesCount;
  int i;

  // Case of not enougth data
  if(pvPdu.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains no data (size < 3 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get bytes count and check it's validity
  bytesCount = pvPdu.at(1);
  if(bytesCount != (pvPdu.size() -2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get the values
  for(i=2; i<pvPdu.size(); i++){
    appendValuesBitsFromByte(pvPdu.at(i));
  }

  return true;
}

bool mdtFrameCodecModbus::decodeReadHoldingRegisters()
{
  quint8 bytesCount;
  int i;
  quint16 value;

  // Case of not enougth data
  if(pvPdu.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains no data (size < 3 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get bytes count and check it's validity
  bytesCount = pvPdu.at(1);
  // Check that bytesCount is mod 2
  if((bytesCount%2) != 0){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count (not MOD 2)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Check that PDU contains right amount of data
  if(bytesCount != (pvPdu.size() -2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get the values
  for(i=2; i<pvPdu.size(); i++){
    value = (quint8)pvPdu.at(i) << 8;
    i++;
    value |= (quint8)pvPdu.at(i);
    pvValues.append(value);
  }

  return true;
}

bool mdtFrameCodecModbus::decodeReadInputRegisters()
{
  quint8 bytesCount;
  int i;
  quint16 value;

  // Case of not enougth data
  if(pvPdu.size() < 3){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains no data (size < 3 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get bytes count and check it's validity
  bytesCount = pvPdu.at(1);
  // Check that bytesCount is mod 2
  if((bytesCount%2) != 0){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count (not MOD 2)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Check that PDU contains right amount of data
  if(bytesCount != (pvPdu.size() -2)){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU contains a invalid bytes count", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get the values
  for(i=2; i<pvPdu.size(); i++){
    value = (quint8)pvPdu.at(i) << 8;
    i++;
    value |= (quint8)pvPdu.at(i);
    pvValues.append(value);
  }

  return true;
}

bool mdtFrameCodecModbus::decodeWriteSingleCoil()
{
  quint16 address;

  // Case of invalid bytes count
  if(pvPdu.size() != 5){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU size not valid (size <> 5 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get address
  address = (quint8)pvPdu.at(1) << 8;
  address |= (quint8)pvPdu.at(2);
  pvValues.append(address);
  // Get value
  if((quint8)pvPdu.at(3) == 0xFF){
    pvValues.append(true);
  }else{
    pvValues.append(false);
  }
  
  qDebug() << "mdtFrameCodecModbus::decodeWriteSingleCoil(): values: " << pvValues;

  return true;
}

bool mdtFrameCodecModbus::decodeWriteSingleRegister()
{
  quint16 value;

  // Case of invalid bytes count
  if(pvPdu.size() != 5){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU size not valid (size <> 5 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get address
  value = (quint8)pvPdu.at(1) << 8;
  value |= (quint8)pvPdu.at(2);
  pvValues.append(value);
  // Get value
  value = (quint8)pvPdu.at(3) << 8;
  value |= (quint8)pvPdu.at(4);
  pvValues.append(value);

  return true;
}

bool mdtFrameCodecModbus::decodeWriteMultipleCoils()
{
  quint16 value;

  // Case of invalid bytes count
  if(pvPdu.size() != 5){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU size not valid (size <> 5 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get starting address
  value = (quint8)pvPdu.at(1) << 8;
  value |= (quint8)pvPdu.at(2);
  pvValues.append(value);
  // Get value
  value = (quint8)pvPdu.at(3) << 8;
  value |= (quint8)pvPdu.at(4);
  pvValues.append(value);

  return true;
}

bool mdtFrameCodecModbus::decodeWriteMultipleRegisters()
{
  quint16 value;

  // Case of invalid bytes count
  if(pvPdu.size() != 5){
    mdtError e(MDT_FRAME_DECODE_ERROR, "PDU size not valid (size <> 5 Bytes)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecModbus");
    e.commit();
    return false;
  }
  // Get starting address
  value = (quint8)pvPdu.at(1) << 8;
  value |= (quint8)pvPdu.at(2);
  pvValues.append(value);
  // Get value
  value = (quint8)pvPdu.at(3) << 8;
  value |= (quint8)pvPdu.at(4);
  pvValues.append(value);

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

  return pvLastModbusError;
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

quint8 mdtFrameCodecModbus::byteFromBooleans(QList<bool> &states)
{
  Q_ASSERT(states.size() == 8);

  quint8 byte = 0;

  // Begin with first state, and store to LSB
  if(states.at(0)){
    byte |= 1;
  }
  if(states.at(1)){
    byte |= (1<<1);
  }
  if(states.at(2)){
    byte |= (1<<2);
  }
  if(states.at(3)){
    byte |= (1<<3);
  }
  if(states.at(4)){
    byte |= (1<<4);
  }
  if(states.at(5)){
    byte |= (1<<5);
  }
  if(states.at(6)){
    byte |= (1<<6);
  }
  if(states.at(7)){
    byte |= (1<<7);
  }

  return byte;
}
