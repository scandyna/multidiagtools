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
#include "mdtFrameModbusTcp.h"

#include <QDebug>

mdtFrameModbusTcp::mdtFrameModbusTcp()
 : mdtFrame()
{
  pvTransactionId = 0;
  pvProtocolId = 0;   // MODBUS: 0
  pvUnitId = 0;
}

mdtFrameModbusTcp::~mdtFrameModbusTcp()
{
}

void mdtFrameModbusTcp::clear()
{
  pvTransactionId = 0;
  pvProtocolId = 0;   // MODBUS: 0
  pvUnitId = 0;
  pvPdu.clear();
  mdtFrame::clear();
}

int mdtFrameModbusTcp::putData(const char *data, int maxLen)
{
  Q_ASSERT(capacity() >= 7);

  int frameSize = 0;
  int stored = 0;

  // If frame is allready full, return
  if(bytesToStore() < 1){
    return 0;
  }
  ///qDebug() << "mdtFrameModbusTcp::putData(): maxLen (1): " << maxLen;

  // Check if it's possible to get size of incomming frame
  if((size()+maxLen) >= 6){
    // Check if some data must be stored before we can get the frame size
    if(size() < 6){
      stored = 6-size();
      append(data, stored);
      Q_ASSERT((data + stored) <= (data + maxLen));
      data += stored;
      maxLen -= stored;
      ///qDebug() << "mdtFrameModbusTcp::putData(): stored(1): " << stored << " , maxLen (2): " << maxLen;
    }
    frameSize = (at(4) << 8) + at(5);
    // We have MODBUS frame size , = Unit ID + PDU. Add missing size
    frameSize += 6;
    // Check if it is possible to store this frame
    if(frameSize > capacity()){
      // Problem here: fill the frame with null values, so bytesToStore() will return 0 next time
      fill(0, capacity());
      stored = capacity();
      return stored;
    }
  }
  // Check wat's possible to store
  ///qDebug() << "mdtFrameModbusTcp::putData(): maxLen (3): " << maxLen;
  if(maxLen > remainCapacity()){
    maxLen = remainCapacity();
  }
  ///qDebug() << "mdtFrameModbusTcp::putData(): maxLen (4): " << maxLen;
  // Store
  append(data, maxLen);
  stored += maxLen;
  ///qDebug() << "mdtFrameModbusTcp::putData(): stored: " << stored;
  // Check if frame is complete
  if(size() == frameSize){
    pvEOFcondition = true;
    // Store members
    pvTransactionId = (at(0) << 8) + at(1);
    pvProtocolId = (at(2) << 8) + at(3);
    pvUnitId = at(6);
    // Store PDU if exists
    if(size() > 7){
      pvPdu = right(size()-7);
    }
  }
  
  return stored;
}

void mdtFrameModbusTcp::setTransactionId(quint16 id)
{
  pvTransactionId = id;
}

quint16 mdtFrameModbusTcp::transactionId()
{
  return pvTransactionId;
}

quint16 mdtFrameModbusTcp::modbusLength()
{
  if(size() < 6){
    return 0;
  }
  return (size() - 6);
}

void mdtFrameModbusTcp::setUnitId(quint8 id)
{
  pvUnitId = id;
}

quint8 mdtFrameModbusTcp::unitId()
{
  return pvUnitId;
}

void mdtFrameModbusTcp::setPdu(const QByteArray &pdu)
{
  pvPdu = pdu;
}

QByteArray mdtFrameModbusTcp::getPdu()
{
  return pvPdu;
}

void mdtFrameModbusTcp::parseMbapHeader()
{
}

void mdtFrameModbusTcp::encode()
{
  Q_ASSERT(capacity() >= 7);

  quint16 length = pvPdu.size() + 1;

  mdtFrame::clear();
  // Transaction ID
  append(pvTransactionId >> 8);
  append(pvTransactionId & 0x00FF);
  // Protocol ID
  append(pvProtocolId >> 8);
  append(pvProtocolId & 0x00FF);
  // Length
  append(length >> 8);
  append(length & 0x00FF);
  // Unit ID
  append(pvUnitId);
  // PDU
  append(pvPdu);
}
