/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include <QCoreApplication>
#include <QTcpSocket>
#include <QHashIterator>
#include <QNetworkAddressEntry>
#include <QHostAddress>
#include <QDir>
#include <QFile>
#include <QTimer>

#include <QDebug>

mdtModbusTcpPortManager::mdtModbusTcpPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Port setup
  mdtPortConfig *config = new mdtPortConfig;
  config->setFrameType(mdtFrame::FT_MODBUS_TCP);
  config->setReadFrameSize(260);
  config->setReadTimeout(3000);
  config->setWriteFrameSize(260);
  config->setWriteQueueSize(3);
  mdtTcpSocket *port = new mdtTcpSocket;
  port->setConfig(config);
  setPort(port);

  // Threads setup
  addThread(new mdtTcpSocketThread);

  // Some flags
  pvKnownHostsFileName = "modbus_tcp_known_hosts";
  pvAbortScan = false;

  // We not want to keep each incomming frame
  setKeepTransactionsDone(false);
}

mdtModbusTcpPortManager::~mdtModbusTcpPortManager()
{
  Q_ASSERT(pvPort != 0);

  delete &pvPort->config();
  detachPort(true, true);
}

mdtDeviceAddressList mdtModbusTcpPortManager::scan(const QList<mdtNetworkHost> & hosts, const mdtModbusHwNodeIdList & expectedHwNodeAddresses, int timeout)
{
  Q_ASSERT(isClosed());

  mdtDeviceAddressList deviceAddressList;

  // Try each host ...
  for(const auto & host : hosts){
    if(pvAbortScan){
      pvAbortScan = false;
      return deviceAddressList; /// \todo clear list ?
    }
    // Try to connect
    if(tryToConnect(host.hostName, host.port, timeout)){
      mdtModbusHwNodeId nid;
      mdtDeviceAddress deviceAddress;
      // Check about HW node addresss if requested
      if(!expectedHwNodeAddresses.isEmpty()){
        setPortName(host.hostName + ":" + QString::number(host.port));  /// \todo update when setPortName() is fixed..
        if(!start()){
          continue;
        }
        // Check if current host's MODBUS HW node ID matches in a item of expected ones
        nid = findMatchingToHostModebusHwNodeId(expectedHwNodeAddresses);
      }
      stop();
      // Setup device address and add it to list
      deviceAddress.setModbusTcpIdentification(host.hostName, host.port, nid);
      deviceAddressList.append(deviceAddress);
      // If we found all expected devices, we can finish scanning here
      if(deviceAddressList.size() == expectedHwNodeAddresses.size()){
        return deviceAddressList;
      }
    }
  }

  return deviceAddressList;
}

mdtDeviceAddressList mdtModbusTcpPortManager::scan(const QNetworkInterface & iface, const mdtModbusHwNodeIdList& expectedHwNodeAddresses,
                                                   quint16 port, int timeout )
{
  Q_ASSERT(isClosed());

  quint32 subNet;
  quint32 bCast;
  QHostAddress firstIp;
  quint32 firstIpV4;
  QHostAddress lastIp;
  quint32 lastIpV4;
  QHostAddress currentIp;
  QList<QNetworkAddressEntry> entries;
  QList<mdtNetworkHost> hosts;
  quint32 j;

  // Get address entries of interface
  entries = iface.addressEntries();
  if(entries.size() < 1){
    pvLastError.setError(tr("Interface ") + iface.name() + tr(" has no IP address"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
    pvLastError.commit();
    return mdtDeviceAddressList();
  }
  // Build the list of IPs
  for(const auto & entry : entries){
    // Check protocol
    if(entry.broadcast().protocol() != QAbstractSocket::IPv4Protocol){
      continue;
    }
    bCast = entry.broadcast().toIPv4Address();
    subNet = bCast & entry.netmask().toIPv4Address();
    // We need min. place for 2 machines in subnet
    if((bCast-subNet-4) < 0){
      pvLastError.setError(tr("Interface '") + iface.name() + tr("' has a subnet for less than 2 machines"), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
      pvLastError.commit();
      continue;
    }
    // Extract first and last IPs
    firstIp.setAddress(subNet+1);
    lastIp.setAddress(bCast-1);
    firstIpV4 = firstIp.toIPv4Address();
    lastIpV4 = lastIp.toIPv4Address();
    // Check that we have not to many hosts to scan
    if((lastIpV4-firstIpV4) > 1000){
      pvLastError.setError(tr("Interface '") + iface.name() + ("' has a subnet for more than 1000 machines (do you try to scan the planet ?)"), mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
      pvLastError.commit();
      return mdtDeviceAddressList();
    }
    // Build list of IP addresses and add to hosts
    for(j = firstIpV4; j <= lastIpV4; ++j){
      mdtNetworkHost host;
      currentIp.setAddress(j);
      host.hostName = currentIp.toString();
      host.port = port;
      hosts.append(host);
    }
  }

  return scan(hosts, expectedHwNodeAddresses, timeout);
}

mdtDeviceAddressList mdtModbusTcpPortManager::scan(const QList<QNetworkInterface> & ifaces, const mdtModbusHwNodeIdList & expectedHwNodeAddresses,
                                                   quint16 port, bool ignoreLoopback, int timeout)
{
  Q_ASSERT(isClosed());

  mdtDeviceAddressList deviceAddressList;

  for(const auto & iface : ifaces){
    // Check about loopback
    if(ignoreLoopback){
      if(iface.flags() & QNetworkInterface::IsLoopBack){
        continue;
      }
    }
    // Scan interface and add to list
    deviceAddressList.append(scan(iface, expectedHwNodeAddresses, port, timeout));
  }

  return deviceAddressList;
}

mdtDeviceAddressList mdtModbusTcpPortManager::scan(const mdtDeviceAddressList & addressList, int timeout)
{
  mdtDeviceAddressList connectedDeviceAddressList;
  mdtModbusHwNodeId nid, connectedDeviceNid;

  for(const auto & address : addressList.internalVector()){
    if( (address.portType() == mdtDeviceAddress::PortType_t::TCPIP) && (address.deviceType() == mdtDeviceAddress::DeviceType_t::MODBUS) ){
      if(pvAbortScan){
        pvAbortScan = false;
        return connectedDeviceAddressList; /// \todo clear list ?
      }
      // Try to connect
      if(tryToConnect(address.tcpIpHostName(), address.tcpIpPort(), timeout)){
        // If given device address contains a hardware node ID, check that device's one matches
        nid = address.modbusHwNodeId();
        if(!nid.isNull()){
          setPortName(address.tcpIpHostName() + ":" + QString::number(address.tcpIpPort()));
          if(!start()){
            continue;
          }
          connectedDeviceNid = getHardwareNodeAddress(nid.bitsCount(), nid.firstBit());
          if(connectedDeviceNid.isNull()){
          stop();
            return connectedDeviceAddressList; /// \todo clear list ?
          }
          if(connectedDeviceNid.id() == nid.id()){
            connectedDeviceAddressList.append(address);
          }
          stop();
        }else{
          // No hardware node ID checking needed, simply add to list
          connectedDeviceAddressList.append(address);
        }
        // If we found all expected devices, we can return here
        if(connectedDeviceAddressList.size() == addressList.size()){
          return connectedDeviceAddressList;
        }
      }
    }
  }

  return connectedDeviceAddressList;
}

mdtDeviceAddressList mdtModbusTcpPortManager::scanFromKnownHostsFile(int timeout)
{
  mdtDeviceAddressList addressList;

  addressList = readDeviceAddressList();
  if(addressList.isEmpty()){
    return addressList;
  }

  return scan(addressList, timeout);
}

bool mdtModbusTcpPortManager::tryToConnect(const QString &hostName, quint16 port, int timeout)
{
  Q_ASSERT(isClosed());

  QTcpSocket socket;
  bool ok = false;
  QTimer timeoutTimer;

  timeoutTimer.setSingleShot(true);
  connect(&timeoutTimer, SIGNAL(timeout()), this, SLOT(abortTryToConnect()));
  emit(statusMessageChanged(tr("Trying host ") + hostName + "  , port " + QString::number(port), "", 500));
  ///qDebug() << "Trying host " << hostName << ":" << port << " (time: " << timeout << "[ms]) " << " ...";
  socket.connectToHost(hostName, port);
  pvAbortTryToConnect = false;
  timeoutTimer.start(timeout);
  while(socket.state() != QAbstractSocket::ConnectedState){
    if(pvAbortTryToConnect){
      timeoutTimer.stop();
      return false;
    }
    if(pvAbortScan){
      timeoutTimer.stop();
      return false;
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  timeoutTimer.stop();
  if(socket.state() != QAbstractSocket::UnconnectedState){
    // Check if connection was successfull
    if(socket.state() == QAbstractSocket::ConnectedState){
      ok = true;
    }
    // Disconnect
    socket.abort();
    while((socket.state() != QAbstractSocket::ClosingState)&&(socket.state() != QAbstractSocket::UnconnectedState)){
      QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    }
  }

  return ok;
}

bool mdtModbusTcpPortManager::saveDeviceAddressList(mdtDeviceAddressList& addressList)
{
  if(!addressList.saveToFile(mdtApplication::cacheDir().absolutePath() + "/" + pvKnownHostsFileName)){
    pvLastError = addressList.lastError();
  }
  return true;
}

mdtDeviceAddressList mdtModbusTcpPortManager::readDeviceAddressList()
{
  mdtDeviceAddressList addressList;

  if(!addressList.readFromFile(mdtApplication::cacheDir().absolutePath() + "/" + pvKnownHostsFileName)){
    pvLastError = addressList.lastError();
  }

  return addressList;
}

bool mdtModbusTcpPortManager::saveScanResult(const QList<mdtPortInfo*> scanResult)
{
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
  QFile file;
  QStringList scanResult;

  // Try to open file
  file.setFileName(mdtApplication::cacheDir().absolutePath() + "/" + pvKnownHostsFileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    return scanResult;
  }
  // Read entries
  while(!file.atEnd()){
    scanResult.append(file.readLine().trimmed());
  }
  file.close();

  return scanResult;
}

bool mdtModbusTcpPortManager::isReady() const
{
  return (currentState() == Ready);
}

mdtModbusHwNodeId mdtModbusTcpPortManager::getHardwareNodeAddress(int bitsCount, int startFrom)
{
  Q_ASSERT(isReady());
  Q_ASSERT(bitsCount > 0);
  Q_ASSERT(startFrom >= 0);

  mdtModbusHwNodeId nid;
  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;
  mdtFrameCodecModbus codec;
  ///int i, id;

  // Setup MODBUS PDU
  pdu = codec.encodeReadDiscreteInputs(startFrom, bitsCount);
  if(pdu.isEmpty()){
    pvLastError.setError(tr("Could not get MODBUS hardware node ID (PDU encoding failed)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
    pvLastError.commit();
    return nid;
    ///return (int)mdtAbstractPort::UnhandledError;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  // Send query
  transaction->setQueryReplyMode(true);
  transaction->setData(pdu);
  transactionId = sendData(transaction);
  if(transactionId < 0){
    /// \todo Port manager must generate error itslef..
    return nid;
    ///return transactionId;
  }
  // Wait on result (use device's defined timeout)
  if(!waitTransactionDone(transactionId)){
    /// \todo Port manager must generate error itslef..
    return nid;
    ///return (int)mdtAbstractPort::ReadTimeout;
  }
  // Get reply and decode
  if(codec.decode(readenFrame(transactionId)) != 0x02){
    pvLastError.setError(tr("Could not get MODBUS hardware node ID (PDU decoding failed)"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
    pvLastError.commit();
    return nid;
    ///return (int)mdtAbstractPort::UnhandledError;
  }
  // Check that we have enough states (can be more, because digital inputs are returned as multiple of 8)
  if(codec.values().size() < bitsCount){
    pvLastError.setError(tr("Have not received enough input states"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtModbusTcpPortManager");
    pvLastError.commit();
    return nid;
    ///return (int)mdtAbstractPort::UnhandledError;
  }
  // Ok, have the states, let's decode ID
  /// \todo Current version of codec is based on QVariant list, so copy it to a std::vector
  std::vector<bool> bits;
  bits.reserve(codec.values().size());
  for(auto & var : codec.values()){
    Q_ASSERT(!var.isNull());
    bits.push_back(var.toBool());
  }
  nid.setId(bits, bitsCount);
  /**
  id = 0;
  for(i=0; i<bitsCount; i++){
    if((codec.values().at(i).isValid())&&(codec.values().at(i).toBool() == true)){
      id |= (1<<i);
    }
  }
  */

  return nid;
  ///return id;
}

mdtModbusHwNodeId mdtModbusTcpPortManager::findMatchingToHostModebusHwNodeId(const mdtModbusHwNodeIdList & hwNodeIdList)
{
  Q_ASSERT(isReady());

  mdtModbusHwNodeId deviceNid;

  for(const auto & nid : hwNodeIdList.internalVector()){
    // Get device HW node ID
    deviceNid = getHardwareNodeAddress(nid.bitsCount(), nid.firstBit());
    if(deviceNid.isNull()){
      return deviceNid;
    }
    // Return item if it matches device ID
    if(nid.id() == deviceNid.id()){
      return nid;
    }
  }
  // No item in list matches device node ID
  return mdtModbusHwNodeId();
}

bool mdtModbusTcpPortManager::getRegisterValues(int address, int n)
{
  Q_ASSERT(address >= 0);
  Q_ASSERT(n > 0);

  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;
  mdtFrameCodecModbus codec;
  int i;

  // Clear previous results
  pvRegisterValues.clear();
  // Setup MODBUS PDU
  pdu = codec.encodeReadInputRegisters(address, n);
  if(pdu.isEmpty()){
    return false;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  // Send query
  transaction->setQueryReplyMode(true);
  transaction->setData(pdu);
  transactionId = sendData(transaction);
  if(transactionId < 0){
    return false;
  }
  // Wait on result (use device's defined timeout)
  if(!waitTransactionDone(transactionId)){
    return false;
  }
  // Get reply and decode
  if(codec.decode(readenFrame(transactionId)) != 4){
    mdtError e(MDT_DEVICE_ERROR, "Received unexptected response code, expeced 0x04 (FC4)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  // Store values
  if(codec.values().size() != n){
    mdtError e(MDT_DEVICE_ERROR, "Received unexptected count of values", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }
  for(i=0; i<n; i++){
    pvRegisterValues.append(codec.values().at(i).toInt());
  }

  return true;
}

const QList<int> &mdtModbusTcpPortManager::registerValues() const
{
  return pvRegisterValues;
}

bool mdtModbusTcpPortManager::setRegisterValues(int startAddress, QList<int> &values)
{
  Q_ASSERT(startAddress >= 0);

  int transactionId;
  mdtPortTransaction *transaction;
  QByteArray pdu;
  mdtFrameCodecModbus codec;

  // Setup MODBUS PDU
  qDebug() << "mdtModbusTcpPortManager::setRegisterValues() - start address: " << startAddress << " - end address: " << startAddress + values.size() - 1;
  pdu = codec.encodeWriteMultipleRegisters(startAddress, values);
  if(pdu.isEmpty()){
    return false;
  }
  // Get a new transaction
  transaction = getNewTransaction();
  // Send query
  transaction->setQueryReplyMode(true);
  transaction->setData(pdu);
  transactionId = sendData(transaction);
  if(transactionId < 0){
    return false;
  }
  // Wait on result (use device's defined timeout)
  if(!waitTransactionDone(transactionId)){
    return false;
  }
  // Get reply and decode
  if(codec.decode(readenFrame(transactionId)) != 16){
    mdtError e(MDT_DEVICE_ERROR, "Received unexptected response code, expeced 0x10 (FC16)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceModbusWago");
    e.commit();
    return false;
  }

  return true;
}

int mdtModbusTcpPortManager::sendData(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(transaction != 0);
  Q_ASSERT(!transactionsDoneContains(transaction->id()));

  mdtFrameModbusTcp *frame = 0;

  // Wait until we can write
  while(1){
    // If port manager was stopped, we return
    if(isClosed()){
      restoreTransaction(transaction);
      return mdtAbstractPort::WriteCanceled;
    }
    if(isReady()){
      lockPortMutex();
      if(pvPort->writeFramesPool().size() > 0){
        frame = dynamic_cast<mdtFrameModbusTcp*> (pvPort->writeFramesPool().dequeue());
        Q_ASSERT(frame != 0);
        // Here we are ready to write - we keep port mutext locked
        break;
      }
      unlockPortMutex();
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  if(!waitTransactionPossible()){
    restoreTransaction(transaction);
    unlockPortMutex();
    return mdtAbstractPort::WriteCanceled;
  }
  // We are ready to write
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  incrementCurrentTransactionId(0, 65535);
  frame->setTransactionId(currentTransactionId());
  frame->setUnitId(0xFF);    /// \todo Handle this ?
  frame->setPdu(transaction->data());
  frame->encode();
  // We enable waitAnAnswer , used by mdtTcpSocketThread for timeout detection
  frame->setWaitAnAnswer(true);
  pvPort->addFrameToWrite(frame);
  transaction->setId(currentTransactionId());
  addTransactionPending(transaction);
  unlockPortMutex();

  return transaction->id();
}

int mdtModbusTcpPortManager::sendData(const QByteArray &pdu, bool queryReplyMode)
{
  Q_ASSERT(pvPort != 0);

  mdtPortTransaction *transaction;

  transaction = getNewTransaction();
  Q_ASSERT(transaction != 0);
  transaction->setData(pdu);
  transaction->setQueryReplyMode(queryReplyMode);

  return sendData(transaction);
}

void mdtModbusTcpPortManager::abortScan()
{
  pvAbortScan = true;
}

void mdtModbusTcpPortManager::abortTryToConnect()
{
  pvAbortTryToConnect = true;
}

void mdtModbusTcpPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameModbusTcp *frame;
  mdtPortTransaction *transaction;
  int framesCount = 0;

  // Get frames in readen queue
  lockPortMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameModbusTcp*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    /// \todo Error on incomplete frame
    if(frame->isComplete()){
      // If we have a pending transaction, remove it
      transaction = transactionPending(frame->transactionId());
      if(transaction == 0){
        mdtError e(MDT_TCP_IO_ERROR, "Received a frame with unexpected transaction ID", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtModbusTcpPortManager");
        e.commit();
      }else{
        /// \todo check about transaction ID ? Should be a assertion ?
        transaction->setId(frame->transactionId());
        transaction->setData(frame->getPdu());
        // Add to transactions done queue
        addTransactionDone(transaction);
        ++framesCount;
      }
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  unlockPortMutex();
  // Commit
  if(framesCount > 0){
    commitFrames();
  }
}
