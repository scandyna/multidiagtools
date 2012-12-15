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

#include <QDebug>

mdtFrameUsbTmc::mdtFrameUsbTmc()
 : mdtFrame()
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

void mdtFrameUsbTmc::clearSub()
{
  pvMsgID = 0;
  pvbTag = 0;
  pvbTagInverse = 0;
  pvTransferSize = 0;
  pvbmTransferAttributes = 0;
  pvTermChar = '\0';
  pvEOM = true;
  pvMessageData.clear();
}

int mdtFrameUsbTmc::putData(const char *data, int maxLen)
{
  Q_ASSERT(capacity() > 12);

  int frameSize = 0;
  int stored = 0;

  // If frame is allready full, return
  if(bytesToStore() < 1){
    return 0;
  }

  // Check wat's possible to store
  if(maxLen > remainCapacity()){
    maxLen = remainCapacity();
  }
  // store
  append(data, maxLen);
  stored = maxLen;

  // Check if we have enougth data to decode header
  if(size() >= 12){
    // Check if header was not allready decoded
    if(pvTransferSize == 0){
      // We have a header here, decode it
      pvMsgID = at(0);
      pvbTag = at(1);
      pvbTagInverse = at(2);
      // Check bTag
      if(pvbTagInverse != (quint8)(~pvbTag)){
        /// \todo Handle error
        qDebug() << "mdtFrameUsbTmc::putData(): bTag/bTagInverse incoherent, bTag: " << pvbTag << " , expected inverse: " << (quint8)~pvbTag << " , rx inverse: " << pvbTagInverse;
      }
      pvTransferSize = at(4);
      pvTransferSize += (at(5) << 8);
      pvTransferSize += (at(6) << 16);
      pvTransferSize += (at(7) << 24);
      // MsgID specific part
      if(pvMsgID == DEV_DEP_MSG_IN){
        // EOM
        if(at(8) & 0x01){
          pvEOM = true;
        }else{
          pvEOM = false;
        }
        /// \todo other bmTransferAttributes
      }else{
        qDebug() << "mdtFrameUsbTmc::putData(): MsgID " << pvMsgID << " not supported";
      }
    }
    // Calc total frame size, without alignment bytes
    frameSize = pvTransferSize + 12;
    // Check if we have a complete frame
    if(size() >= frameSize){
      pvEOFcondition = true;
      pvMessageData += right(size()-12).left(pvTransferSize);
    }
  }

  return stored;
}

void mdtFrameUsbTmc::setMsgID(mdtFrameUsbTmc::msg_id_t MsgID)
{
  pvMsgID = MsgID;
}

mdtFrameUsbTmc::msg_id_t mdtFrameUsbTmc::MsgID() const
{
  return (msg_id_t)pvMsgID;
}

void mdtFrameUsbTmc::setbTag(quint8 bTag)
{
  pvbTag = bTag;
  pvbTagInverse = ~pvbTag;
}

quint8 mdtFrameUsbTmc::bTag() const
{
  return pvbTag;
}

void mdtFrameUsbTmc::setEOM(bool EOM)
{
  pvEOM = EOM;
}

bool mdtFrameUsbTmc::isEOM() const
{
  return pvEOM;
}

void mdtFrameUsbTmc::setMessageData(const QByteArray &data)
{
  pvMessageData = data;
  pvTransferSize = pvMessageData.size();
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
  mdtFrame::clear();
  // Get message data size
  // Transfer size
  if(pvMsgID == DEV_DEP_MSG_OUT){
    pvTransferSize = pvMessageData.size();
  }else if(pvMsgID == DEV_DEP_MSG_IN){
    pvTransferSize = remainCapacity() - 12 - 3; /// \todo Check > 0, ...
  }else{
    qDebug() << "mdtFrameUsbTmc::encode(): Unknow pvMsgID, transfer size will be set to 0";
    pvTransferSize = 0;
  }
  // Build USBTMC header
  append((char)pvMsgID);
  append((char)pvbTag);
  append((char)pvbTagInverse);
  append((char)0x00);
  append((char)pvTransferSize & 0xFF);            // TransferSize, LLSB
  append((char)((pvTransferSize >> 8) & 0xFF));   // TransferSize, LSB
  append((char)((pvTransferSize >> 16) & 0xFF));  // TransferSize, LSB
  append((char)((pvTransferSize >> 24) & 0xFF));  // TransferSize, LSB
  if(pvEOM){
    append((char)0x01);                           // bmTransferAttributes
  }else{
    append((char)0x00);                           // bmTransferAttributes
  }
  append((char)0x00);                             // Reserved
  append((char)0x00);                             // Reserved
  append((char)0x00);                             // Reserved
  // Add data
  append(pvMessageData);
  // Add alignment bytes if requierd
  while((size() % 4) != 0){
    append((char)0x00);
  }
}
