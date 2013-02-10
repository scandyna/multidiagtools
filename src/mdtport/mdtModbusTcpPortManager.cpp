/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtApplication.h"
#include <QString>
#include <QApplication>
#include <QTcpSocket>
#include <QHashIterator>
#include <QNetworkAddressEntry>
#include <QHostAddress>
#include <QDir>
#include <QFile>

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
  pvKnownHostsFileName = "modbus_tcp_konown_hosts";
  pvAbortScan = false;

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

  for(i=0; i<hosts.size(); i++){
    if(pvAbortScan){
      portInfoList.clear();
      pvAbortScan = false;
      return portInfoList;
    }
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
    if(tryToConnect(hostName, hostPort, timeout)){
      portInfo = new mdtPortInfo;
      portInfo->setPortName(hosts.at(i));
      portInfoList.append(portInfo);
    }
  }

  return portInfoList;
}

QList<mdtPortInfo*> mdtModbusTcpPortManager::scan(const QNetworkInterface &iface, quint16 port, int timeout)
{
  Q_ASSERT(!isRunning());

  quint32 subNet;
  quint32 bCast;
  QHostAddress firstIp;
  quint32 firstIpV4;
  QHostAddress lastIp;
  quint32 lastIpV4;
  QHostAddress currentIp;
  QNetworkAddressEntry entry;
  QList<QNetworkAddressEntry> entries;
  QList<mdtPortInfo*> portInfoList;
  mdtPortInfo *portInfo;
  int i;
  quint32 j;

  // Get address entries of interface
  entries = iface.addressEntries();
  if(entries.size() < 1){
    mdtError e(MDT_TCP_IO_ERROR, "Interface " + iface.name() + " has no IP address", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
    e.commit();
    return portInfoList;
  }
  // Build the list of IPs
  for(i=0; i<entries.size(); i++){
    entry = entries.at(i);
    // Check protocol
    if(entry.broadcast().protocol() != QAbstractSocket::IPv4Protocol){
      continue;
    }
    bCast = entry.broadcast().toIPv4Address();
    subNet = bCast & entry.netmask().toIPv4Address();
    // We need min. place for 2 machines in subnet
    if((bCast-subNet-4) < 0){
      mdtError e(MDT_TCP_IO_ERROR, "Interface " + iface.name() + " has a subnet for less than 2 machines", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
      e.commit();
      continue;
    }
    // Extract first and last IPs
    firstIp.setAddress(subNet+1);
    lastIp.setAddress(bCast-1);
    firstIpV4 = firstIp.toIPv4Address();
    lastIpV4 = lastIp.toIPv4Address();
    // Scan this range and add to list
    for(j=firstIpV4; j<=lastIpV4; j++){
      if(pvAbortScan){
        portInfoList.clear();
        pvAbortScan = false;
        return portInfoList;
      }
      currentIp.setAddress(j);
      if(tryToConnect(currentIp.toString(), port, timeout)){
        portInfo = new mdtPortInfo;
        portInfo->setPortName(currentIp.toString() + ":" + QString::number(port));
        portInfoList.append(portInfo);
      }
    }
  }
  // If no entry was found, we warn that possible error is due to no IPv4 configuration
  if(entries.size() < 1){
    mdtError e(MDT_TCP_IO_ERROR, "Interface " + iface.name() + " has no IPv4 addresse (possibly it's configured with IPv6 only)", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
    e.commit();
  }

  return portInfoList;
}

QList<mdtPortInfo*> mdtModbusTcpPortManager::scan(const QList<QNetworkInterface> &ifaces, quint16 port, int timeout, bool ignoreLoopback)
{
  Q_ASSERT(!isRunning());

  QList<mdtPortInfo*> portInfoList;
  int i;

  for(i=0; i<ifaces.size(); i++){
    if(pvAbortScan){
      portInfoList.clear();
      pvAbortScan = false;
      return portInfoList;
    }
    // Check about loopback
    if(ignoreLoopback){
      if(ifaces.at(i).flags() & QNetworkInterface::IsLoopBack){
        continue;
      }
    }
    portInfoList.append(scan(ifaces.at(i), port, timeout));
  }

  return portInfoList;
}

bool mdtModbusTcpPortManager::tryToConnect(const QString &hostName, quint16 port, int timeout)
{
  Q_ASSERT(!isRunning());

  QTcpSocket socket;
  int maxIter;
  bool ok = false;

  emit(errorStateChanged(mdtAbstractPort::Connecting, tr("Trying ") + hostName));
  socket.connectToHost(hostName, port);
  maxIter = timeout / 50;
  while(socket.state() != QAbstractSocket::ConnectedState){
    if(pvAbortScan){
      return false;
    }
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
      ok = true;
    }
    // Disconnect
    socket.abort();
    while((socket.state() != QAbstractSocket::ClosingState)&&(socket.state() != QAbstractSocket::UnconnectedState)){
      qApp->processEvents();
      msleep(50);
    }
  }

  return ok;
}

bool mdtModbusTcpPortManager::saveScanResult(const QList<mdtPortInfo*> scanResult)
{
  ///Q_ASSERT(!isRunning());

  QFile file;
  int i;
  QByteArray line;

  // Try to open file
  file.setFileName(mdtApplication::cacheDir().absolutePath() + "/" + pvKnownHostsFileName);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
    mdtError e(MDT_FILE_IO_ERROR, "Cannot write to " + file.fileName(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
    e.commit();
    return false;
  }
  // Save items
  for(i=0; i<scanResult.size(); i++){
    Q_ASSERT(scanResult.at(i) != 0);
    line = scanResult.at(i)->portName().toAscii();
    line += "\n";
    if(file.write(line) < 0){
      mdtError e(MDT_FILE_IO_ERROR, "Write error occured during write to " + file.fileName(), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
      e.commit();
      file.close();
      return false;
    }
  }
  file.close();

  return true;
}

QStringList mdtModbusTcpPortManager::readScanResult()
{
  Q_ASSERT(!isRunning());

  QFile file;
  QStringList scanResult;

  // Try to open file
  file.setFileName(mdtApplication::cacheDir().absolutePath() + "/" + pvKnownHostsFileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    return scanResult;
  }
  // Read entries
  while(!file.atEnd()){
    scanResult.append(file.readLine());
  }
  file.close();

  return scanResult;
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

void mdtModbusTcpPortManager::abortScan()
{
  pvAbortScan = true;
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
