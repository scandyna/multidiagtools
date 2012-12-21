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
#include "mdtUsbtmcPortManager.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameUsbControl.h"
#include "mdtFrameUsbTmc.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbConfigDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtFrameCodecScpi.h"
#include <QString>
#include <QApplication>
#include <libusb-1.0/libusb.h>

#include <QDebug>

mdtUsbtmcPortManager::mdtUsbtmcPortManager(QObject *parent)
 : mdtUsbPortManager(parent)
{
  Q_ASSERT(pvPort != 0);

  // Port setup
  pvPort->config().setFrameType(mdtFrame::FT_USBTMC);
  pvPort->config().setReadFrameSize(512);
  pvPort->config().setWriteFrameSize(512);
  pvPort->config().setWriteQueueSize(1);

  // USBTMC specific
  pvCurrentWritebTag = 0;

  // Enable transactions support
  setTransactionsEnabled();
}

mdtUsbtmcPortManager::~mdtUsbtmcPortManager()
{
}

QList<mdtPortInfo*> mdtUsbtmcPortManager::scan()
{
  QList<mdtPortInfo*> portInfoList;
  mdtDeviceInfo *deviceInfo;
  int i, j;

  // Search all USBTMC devices (bInterfaceClass == 0xFE and bInterfaceSubClass == 0x03)
  portInfoList = mdtUsbPortManager::scan(0xFE, 0x03, -1);

  // Update some informations in found devices interfaces
  for(i=0; i<portInfoList.size(); i++){
    for(j=0; j<portInfoList.at(i)->deviceInfoList().size(); j++){
      deviceInfo = portInfoList.at(i)->deviceInfoList().at(j);
      Q_ASSERT(deviceInfo != 0);
      if(deviceInfo->protocolId() == 0x00){
        deviceInfo->setDisplayText("USBTMC");
      }else if(deviceInfo->protocolId() == 0x01){
        deviceInfo->setDisplayText("USBTMC USB488");
      }else{
        deviceInfo->setDisplayText("Unknow");
      }
    }
  }

  return portInfoList;
}

int mdtUsbtmcPortManager::sendCommand(const QByteArray &command, int timeout)
{
  // Wait until data can be sent
  if(!waitOnWriteReady(timeout)){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Send query
  return writeData(command);
}

QByteArray mdtUsbtmcPortManager::sendQuery(const QByteArray &query, int writeTimeout, int readTimeout)
{
  int bTag;
  ///int i = readTimeout / 50;
  mdtPortTransaction *transaction;

  // Clear previous response
  ///pvGenericData.clear();
  // Wait until data can be sent
  if(!waitOnWriteReady(writeTimeout)){
    return QByteArray();
  }
  // Send query
  bTag = writeData(query);
  if(bTag < 0){
    return QByteArray();
  }
  // Wait until more data can be sent
  if(!waitOnWriteReady(writeTimeout)){
    return QByteArray();
  }
  // Setup transaction
  transaction = getNewTransaction();
  transaction->setType(MDT_FC_SCPI_UNKNOW);
  transaction->setQueryReplyMode(true);
  // Send read request
  bTag = sendReadRequest(transaction);
  if(bTag < 0){
    return QByteArray();
  }
  // Wait on response
  if(!waitOnFrame(bTag, readTimeout)){
    return QByteArray();
  }

  return readenFrame(bTag);
  // Remember query type.
  /// \note It seems that U3606A does allways return a bTag 0 , bug in mdt lib ?
  ///pvCodec->addTransaction(0, MDT_FC_SCPI_UNKNOW);
  // Wait on response
  /**
  while(pvGenericData.size() < 1){
    // Check timeout
    if(i <= 0){
      return QByteArray();
    }
    pvPortManager->wait(50, 50);
    i--;
  }
  */

  return QByteArray();
}

int mdtUsbtmcPortManager::writeData(QByteArray data)
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->setWaitAnAnswer(false);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  qDebug() << "mdtUsbtmcPortManager::writeData() - bTag: " << pvCurrentWritebTag;
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData(data);
  frame->setEOM(true);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return pvCurrentWritebTag;
}

int mdtUsbtmcPortManager::sendReadRequest(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);

  mdtFrameUsbTmc *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data, encode and add frame to write queue
  frame->setWaitAnAnswer(true);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  qDebug() << "mdtUsbtmcPortManager::sendReadRequest() - bTag: " << pvCurrentWritebTag;
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData("");
  frame->setTransferSize(config().readFrameSize()-1);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  // Add transaction
  transaction->setId(pvCurrentWritebTag);
  addTransaction(transaction);
  ///addTransaction(pvCurrentWritebTag, enqueueResponse);
  pvPort->unlockMutex();

  return pvCurrentWritebTag;
}

int mdtUsbtmcPortManager::sendReadRequest(bool enqueueResponse)
{
  mdtPortTransaction *transaction;

  // Get a transaction from pool and setup it
  transaction = getNewTransaction();
  transaction->setQueryReplyMode(enqueueResponse);

  return sendReadRequest(transaction);
}


/*
void mdtUsbtmcPortManager::abort()
{
  Q_ASSERT(pvPort != 0);

  ///mdtUsbPort *port = dynamic_cast<mdtUsbPort*> (pvPort);
  ///Q_ASSERT(port != 0);
  qDebug() << "mdtUsbtmcPortManager::abort() ...";
  pvPort->flushIn();
  pvPort->flushOut();
}
*/

/// \todo finish !
int mdtUsbtmcPortManager::sendReadStatusByteRequest()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbControl frame;
  int retVal;

  frame.setDirectionDeviceToHost(true);
  frame.setRequestType(mdtFrameUsbControl::RT_CLASS);
  frame.setRequestRecipient(mdtFrameUsbControl::RR_INTERFACE);
  frame.setbRequest(128); // READ_STATUS_BYTE
  // Ajust bTag to:  2 <= bTag <= 127
  if(pvCurrentWritebTag > 127){
    pvCurrentWritebTag = 2;
  }
  while(pvCurrentWritebTag < 2){
    pvCurrentWritebTag++;
  }
  frame.setwValue(pvCurrentWritebTag);
  frame.setwLength(0x3);
  frame.encode();
  // In this query, wIndex is the interface number, let USB port fill it.
  retVal = sendControlRequest(frame, true);
  if(retVal < 0){
    return retVal;
  }
  return pvCurrentWritebTag;
  ///m.sendControlRequest(f);
  /// End Sandbox !

}

void mdtUsbtmcPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;
  mdtPortTransaction *transaction;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    /// \todo Error on incomplete frame
    if(frame->isComplete()){
      // If we have a pending transaction, remove it
      transaction = pendingTransaction(frame->bTag());
      if(transaction == 0){
        mdtError e(MDT_USB_IO_ERROR, "Received a frame with unexpected bTag", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
        e.commit();
      }else{
        transaction->setId(frame->bTag());
        transaction->setData(frame->messageData());
        enqueueTransactionRx(transaction);
        ///pvTransactionsDone.insert(transaction->id(), transaction);
      }
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  // Commit
  commitFrames();
}
