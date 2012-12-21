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
#include "mdtModbusTcpPortManager.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtTcpSocketThread.h"
#include "mdtFrame.h"
#include "mdtFrameModbusTcp.h"
#include "mdtFrameCodecModbus.h"
#include <QString>
#include <QApplication>
#include <QTcpSocket>
#include <QHashIterator>

#include <QDebug>

mdtModbusTcpPortManager::mdtModbusTcpPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Port setup
  mdtPortConfig *config = new mdtPortConfig;
  config->setFrameType(mdtFrame::FT_MODBUS_TCP);
  config->setReadFrameSize(260);
  config->setWriteFrameSize(260);
  config->setWriteQueueSize(3);
  mdtTcpSocket *port = new mdtTcpSocket;
  port->setConfig(config);
  port->setUnknownReadSize(false);
  setPort(port);

  // Threads setup
  addThread(new mdtTcpSocketThread);

  // Some flags
  pvTransactionId = 0;

  // Enable transactions support
  setTransactionsEnabled();
}

mdtModbusTcpPortManager::~mdtModbusTcpPortManager()
{
  Q_ASSERT(pvPort != 0);

  delete &pvPort->config();
  detachPort(true, true);
}

QList<mdtPortInfo*> mdtModbusTcpPortManager::scan(const QStringList &hosts, int timeout)
{
  Q_ASSERT(!isRunning());

  QList<mdtPortInfo*> portInfoList;
  mdtPortInfo *portInfo;
  int i;
  QStringList host;
  QString hostName;
  quint16 hostPort;
  bool ok;
  QTcpSocket socket;
  int maxIter;

  for(i=0; i<hosts.size(); i++){
    qDebug() << "mdtModbusTcpPortManager::scan(): trying host " << hosts.at(i) << " ...";
    // Extract hostname/ip and port
    host = hosts.at(i).split(":");
    if(host.size() != 2){
      mdtError e(MDT_TCP_IO_ERROR, "Cannot extract host and port in " + hosts.at(i), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
      e.commit();
      continue;
    }
    hostName = host.at(0);
    hostPort = host.at(1).toUShort(&ok);
    if(!ok){
      mdtError e(MDT_TCP_IO_ERROR, "Cannot extract port in " + hosts.at(i), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
      e.commit();
      continue;
    }
    // Try to connect
    socket.connectToHost(hostName, hostPort);
    maxIter = timeout / 50;
    while(socket.state() != QAbstractSocket::ConnectedState){
      if(maxIter <= 0){
        break;
      }
      qApp->processEvents();
      msleep(50);
      maxIter--;
    }
    if(socket.state() != QAbstractSocket::UnconnectedState){
      // Check if connection was successfull
      if(socket.state() == QAbstractSocket::ConnectedState){
        qDebug() << "Found device at " << host;
        portInfo = new mdtPortInfo;
        portInfo->setPortName(hosts.at(i));
        portInfoList.append(portInfo);
      }
      // Disconnect
      //socket.disconnectFromHost();
      socket.abort();
      while((socket.state() != QAbstractSocket::ClosingState)&&(socket.state() != QAbstractSocket::UnconnectedState)){
        qApp->processEvents();
        msleep(50);
      }
    }
  }

  return portInfoList;
}

int mdtModbusTcpPortManager::writeData(QByteArray pdu, bool enqueueResponse)
{
  Q_ASSERT(pvPort != 0);

  mdtFrameModbusTcp *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameModbusTcp*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  ///frame->setWaitAnAnswer(false);
  pvTransactionId++;
  frame->setTransactionId(pvTransactionId);
  frame->setUnitId(0);    /// \todo Handle this ?
  frame->setPdu(pdu);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  addTransaction(pvTransactionId, enqueueResponse);
  ///addTransaction(transaction);
  pvPort->unlockMutex();

  return pvTransactionId;
}

int mdtModbusTcpPortManager::writeData(QByteArray pdu, mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);

  mdtFrameModbusTcp *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
    e.commit();
    return mdtAbstractPort::WritePoolEmpty;
  }
  frame = dynamic_cast<mdtFrameModbusTcp*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  pvTransactionId++;
  frame->setTransactionId(pvTransactionId);
  frame->setUnitId(0);    /// \todo Handle this ?
  frame->setPdu(pdu);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  transaction->setId(pvTransactionId);
  addTransaction(transaction);
  pvPort->unlockMutex();

  return pvTransactionId;
}


void mdtModbusTcpPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameModbusTcp *frame;
  mdtPortTransaction *transaction;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameModbusTcp*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    /// \todo Error on incomplete frame
    if(frame->isComplete()){
      // If we have a pending transaction, remove it
      transaction = pendingTransaction(frame->transactionId());
      if(transaction == 0){
        mdtError e(MDT_TCP_IO_ERROR, "Received a frame with unexpected transaction ID", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
        e.commit();
      }else{
        transaction->setId(frame->transactionId());
        transaction->setData(frame->getPdu());
        enqueueTransactionRx(transaction);
      }
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  // Commit
  commitFrames();
}
