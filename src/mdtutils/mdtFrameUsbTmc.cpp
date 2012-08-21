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
#include "mdtFrameUsbTmc.h"

mdtFrameUsbTmc::mdtFrameUsbTmc()
{
  pvMsgID = 0;
  pvbTag = 0;
  pvbTagInverse = 0;
  pvTransferSize = 0;
  pvbmTransferAttributes = 0;
  pvTermChar = '\0';
  pvEOM = true;
}

mdtFrameUsbTmc::~mdtFrameUsbTmc()
{
}

void mdtFrameUsbTmc::clear()
{
  //pvMsgID = 0;
  //pvbTag = 0;
  //pvbTagInverse = 0;
  //pvTransferSize = 0;
  //pvbmTransferAttributes = 0;
  //pvTermChar = '\0';
  //pvEOM = true;
  mdtFrame::clear();
}

int mdtFrameUsbTmc::putData(char data, int maxLen)
{
  
}

void mdtFrameUsbTmc::setMsgID(mdtFrameUsbTmc::msg_id_t MsgID)
{
  pvMsgID = MsgID;
}

mdtFrameUsbTmc::msg_id_t mdtFrameUsbTmc::MsgID()
{
  return (msg_id_t)pvMsgID;
}

void mdtFrameUsbTmc::setbTag(quint8 bTag)
{
  pvbTag = bTag;
  pvbTagInverse = ~pvbTag;
}

quint8 mdtFrameUsbTmc::bTag()
{
  return pvbTag;
}

void mdtFrameUsbTmc::setEOM(bool EOM)
{
  pvEOM = EOM;
}

bool mdtFrameUsbTmc::isEOM()
{
  return pvEOM;
}

void mdtFrameUsbTmc::setMessageData(const QByteArray &data)
{
  pvMessageData = data;
}

QByteArray &mdtFrameUsbTmc::messageData()
{
  return pvMessageData;
}

void mdtFrameUsbTmc::encode()
{
  // Check minimal needed capacity
  if(capacity() < 16){
    reserve(16);
  }
  clear();
  // Calc size NOTE: padding
  pvTransferSize = pvMessageData.size();
  // Build USBTMC header
  append((char)pvMsgID);
  append((char)pvbTag);
  append((char)pvbTagInverse);
  append((char)0x00);
  append((char)pvTransferSize & 0x000F);        // TransferSize, LLSB
  append((char)((pvTransferSize & 0x00F0)>>1)); // TransferSize, LSB
  append((char)((pvTransferSize & 0x0F00)>>2)); // TransferSize, LSB
  append((char)((pvTransferSize & 0xF000)>>3)); // TransferSize, LSB
  if(pvEOM){
    append((char)0x01);                           // bmTransferAttributes
  }else{
    append((char)0x00);                           // bmTransferAttributes
  }
  append((char)0x00);                           // Reserved
  append((char)0x00);                           // Reserved
  append((char)0x00);                           // Reserved
  // Add data
  append(pvMessageData);
}

