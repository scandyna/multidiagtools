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
#ifndef MDT_MODBUS_TCP_PORTMANAGER_H
#define MDT_MODBUS_TCP_PORTMANAGER_H

#include "mdtPortManager.h"

#include "mdtPortInfo.h"

#include "mdtDeviceAddress.h"
#include "mdtModbusHwNodeId.h"

#include "mdtTcpSocket.h"
#include <QStringList>
#include <QList>
#include <QHash>
#include <QNetworkInterface>

/*! \brief Little container for network host address and port
 */
struct mdtNetworkHost
{
  /*! \brief Construct a empty host
   */
  mdtNetworkHost()
   : port(0)
  {
  }

  /*! \brief Hosta name
   */
  QString hostName;

  /*! \brief Port
   */
  uint16_t port;
};

/*! \brief MODBUS/TCP port manager
 *
 * This is the simplest way to use the MODBUS/TCP API.
 *
 * All needed object are created by constructor.
 *  To alter configuration, use config().
 *  To access the tcp port object, use port().
 *
 * Use start() to begin read/write and stop to end.
 *
 * References:
 *  - MODBUS Application Protocol Specification V1.1b
 *  - http://www.Modbus-IDA.org
 */
class mdtModbusTcpPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct a MODBUS/TCP port manager
   *
   * Creates a mdtPortConfig, a mdtTcpSocket,
   *  and thread object.
   */
  mdtModbusTcpPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  ~mdtModbusTcpPortManager();

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param hosts A list of hosts to scan.
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const QList<mdtNetworkHost> & hosts, const mdtModbusHwNodeIdList & expectedHwNodeAddresses, int timeout = 500);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param iface Scan will be done for all IP addresses available for given intarface.
   *               Note that only IPv4 is implemented.
   * \param port Port. MODBUS/TCP default port is 502.
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const QNetworkInterface & iface, const mdtModbusHwNodeIdList & expectedHwNodeAddresses,
                            quint16 port = 502, int timeout = 500);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param iface Scan will be done for all IP addresses available for given intarface.
   *               Note that only IPv4 is implemented.
   * \param port Port. MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const QNetworkInterface & iface, quint16 port = 502, int timeout = 500)
  {
    return scan(iface, mdtModbusHwNodeIdList(), port, timeout);
  }

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param ifaces Scan will be done for all IP addresses available for all given intarfaces.
   *               Note that only IPv4 is implemented.
   * \param port Port. MODBUS/TCP default port is 502.
   * \param ignoreLoopback If true, loopback interface will be ignored.
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const QList<QNetworkInterface> & ifaces, const mdtModbusHwNodeIdList & expectedHwNodeAddresses,
                            quint16 port = 502, bool ignoreLoopback = true,
                            int timeout = 500);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param ifaces Scan will be done for all IP addresses available for all given intarfaces.
   *               Note that only IPv4 is implemented.
   * \param port Port. MODBUS/TCP default port is 502.
   * \param ignoreLoopback If true, loopback interface will be ignored.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const QList<QNetworkInterface> & ifaces, quint16 port = 502, bool ignoreLoopback = true, int timeout = 500)
  {
    return scan(ifaces, mdtModbusHwNodeIdList(), port, ignoreLoopback, timeout);
  }

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * Return a list of each device in address list that is a MODBUS/TCP
   *  device, that can be reached and that has matching hardware node ID.
   *
   * \param addressList Address list that contains hosts to try to connect to.
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scan(const mdtDeviceAddressList & addressList, int timeout = 500);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * Will read known hosts file and return list of devices that are attached
   *  at given host name, port and that have hardware node ID matching.
   *
   * \param timeout Maximum wait time [ms].
   * \return A list of device addresses that contains found hosts.
   *          A empty list means that no device was found, or other error happened,
   *          lastError() will also provide some informations in the case.
   * \pre Port manager must be stopped before scanning.
   */
  mdtDeviceAddressList scanFromKnownHostsFile(int timeout = 500);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param hosts A list of hosts to scan. Each host must be set with format hostname:port, or ip:port.
   *               Note that MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   *                                 See getHardwareNodeAddress() for details about HW node address.
   * \param bitsCount See getHardwareNodeAddress()
   * \param bitsCountStartFrom See getHardwareNodeAddress()
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Port manager must be stopped
   */
  ///QList<mdtPortInfo*> scan(const QStringList &hosts, int timeout = 500, const QList<int> &expectedHwNodeAddresses = QList<int>(), int bitsCount = 8, int bitsCountStartFrom = 0);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param iface Scan will be done for all IP addresses available for given intarface.
   *               Note that only IPv4 is implemented.
   * \param port Port. Note that MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   *                                 See getHardwareNodeAddress() for details about HW node address.
   * \param bitsCount See getHardwareNodeAddress()
   * \param bitsCountStartFrom See getHardwareNodeAddress()
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Port manager must be stopped
   */
  ///QList<mdtPortInfo*> scan(const QNetworkInterface &iface, quint16 port = 502, int timeout = 500, const QList<int> &expectedHwNodeAddresses = QList<int>(), int bitsCount = 8, int bitsCountStartFrom = 0);

  /*! \brief Scan for available hosts with a MODBUS/TCP compatible device attached
   *
   * \param ifaces Scan will be done for all IP addresses available for all given intarfaces.
   *               Note that only IPv4 is implemented.
   * \param port Port. Note that MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \param ignoreLoopback If true, loopback interface will be ignored.
   * \param expectedHwNodeAddresses If this list contains HW node addresses, scan will break when each one was found.
   *                                 See getHardwareNodeAddress() for details about HW node address.
   * \param bitsCount See getHardwareNodeAddress()
   * \param bitsCountStartFrom See getHardwareNodeAddress()
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Port manager must be stopped
   */
  ///QList<mdtPortInfo*> scan(const QList<QNetworkInterface> &ifaces, quint16 port = 502, int timeout = 500, bool ignoreLoopback = true, const QList<int> &expectedHwNodeAddresses = QList<int>(), int bitsCount = 8, int bitsCountStartFrom = 0);

  /*! \brief Try to connect to a host
   *
   * \param hostName Host name or a IP address
   * \param port Port. Note that MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True on successfull connection.
   * \pre Port manager must be stopped
   */
  bool tryToConnect(const QString &hostName, quint16 port, int timeout);

  /*! \brief Save device address list into the known hosts file
   *
   * The file will be written to the $HOME/.mdt/cache directory.
   * \pre Manager must no running
   */
  bool saveDeviceAddressList(mdtDeviceAddressList & addressList);

  /*! \brief Read device address list from known hosts file
   *
   * Will read file written by saveDeviceAddressList().
   */
  mdtDeviceAddressList readDeviceAddressList();

  /*! \brief Save a scan result into the known hosts file
   *
   * The file will be written to the $HOME/.mdt/cache directory,
   *  and contains lines with format host:port.
   *
   * \param scanResult List of port informations returned by a scan() method.
   * \return True on successfull write.
   * \pre Manager must no running
   * \deprecated Use saveDeviceAddressList()
   */
  bool saveScanResult(const QList<mdtPortInfo*> scanResult);

  /*! \brief Read scan result stored in cache file
   *
   * The file stored by saveScanResult() is readen (if exists)
   *  and content returned.
   *
   * \return List of lines or empty list if file not exists or on error.
   * \deprecated Use readDeviceAddressList()
   */
  QStringList readScanResult();

  /*! \brief Check if port manager is ready
   *
   * Internally, the currentState is used to check if port manager is ready.
   *  This implementation returns true if currentState is Ready (= connected to device).
   */
  bool isReady() const;

  /*! \brief Get the hardware node ID
   *
   * The concept of node ID is specific to MultiDiagTools.
   *  It was introduced to solve the problem of network IP configurations.
   *
   * Take a example: We have a application that needs several MODBUS/TCP nodes.
   *  Each node has several I/O's.
   *  Say that digital output 1 of node 1 is used to activate a lamp
   *  and output 1 of node 2 must activate heating register.
   *  We could choose to have static IP addresses in the network and configure the
   *  application to activate the correct output of correct node.
   *  But, this can be a problem when a node must be replaced.
   *  More cumbersome case is a modular application that could manage several nodes,
   *  and change his I/Os layout in a dynamic way regarding a context.
   *
   * A easy way to solve such problems is to have a DHCP server in the network and
   *  all nodes configured as DHCP clients
   *   (static addressing can still be a solution, but we not depend on it).
   *  Then, for a given application, we can fix a number of digital inputs to play
   *  the role of node address. Then, instead of using IP address as identification
   *  (as in the example), we use node address.
   *
   * \param bitsCount Number of digital inputs that represents the hardware node address.
   * \param startFrom First digital input that represents the hardware node address (is the LSB).
   * \return Node address. On error, a null hardware node ID is returned.
   * \pre Port manager must be connected and thread running before calling this method.
   * \pre bitsCount must be > 0
   * \pre startFrom must be >= 0
   */
  mdtModbusHwNodeId getHardwareNodeAddress(int bitsCount, int startFrom = 0);

  /*! \brief Get MODBUS hardware node ID that matches in given list
   *
   * Will get MODBUS harware node ID, regarding bitsCount and firstBit
   *  for each item in given list, and return the first item that matches.
   *  If no matching item was found, a null ID is returned.
   *
   * \pre Port manager must be connected and thread running before calling this method.
   *
   * \todo Should be protected.
   */
  mdtModbusHwNodeId findMatchingToHostModebusHwNodeId(const mdtModbusHwNodeIdList & hwNodeIdList);

  /*! \brief Helper method for register service
   *
   * Usefull to get resgister values (f.ex. configurations regsisters, ...).
   *
   * Note: to get analog I/O values, the mdtDevice API should be used.
   *
   * \return True on success. Values are the available with registerValues()
   * \pre address must be >= 0
   * \pre n must be > 0
   */
  bool getRegisterValues(int address, int n);

  /*! \brief Helper method for register service
   *
   * Return result set by getRegisterValues().
   * Note that values are keeped until next call of getRegisterValues().
   */
  const QList<int> &registerValues() const;

  /*! \brief Helper method for register service
   *
   * Usefull to set register values (f.ex. configurations regsisters, ...).
   *
   * Note: to set analog I/O values, the mdtDevice API should be used.
   *
   * \return True on success.
   * \pre startAddress must be >= 0
   */
  bool setRegisterValues(int startAddress, QList<int> &values);

  /*! \brief Send PDU on TCP port
   *
   * At first, this method waits until the ready state is set calling isReady() , a frame is available in port's write frames pool,
   *  and until a transaction is possible (see waitTransactionPossible() ).
   *  This wait will not break event loop, so no GUI freeze occurs.
   *  If port manager is stopped during this wait (unhandled error, stop request),
   *  this method returns.
   *
   * Then, data will be encoded regarding MODBUS/TCP standard and passed to the mdtTcpSocket write queue.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param transaction Transaction used to send data. Following members are used by this method:
   *                     - id : MODBUS/TCP transaction ID (will be set internally).
   *                     - data : PDU to send.
   *                     - isQueryReplyMode : if true, the transaction will be keeped in transactions done queue
   *                                          until readenFrame(int) is called.
   *                                           For all cases, signal newReadenFrame(mdtPortTransaction*)
   *                                            is emitted when data comes in.
   *
   * \return Transaction ID on success or value < 0 on error.
   *          Possible error is mdtAbstractPort::WriteCanceled,
   *          witch typically occurs when port manager stops.
   *          Note: on failure, the transaction is restored to pool.
   * \pre Port must be set with setPort() before use of this method.
   * \pre transaction must be a valid pointer, and not allready exists in transactions pending or transactions done queue.
   */
  int sendData(mdtPortTransaction *transaction);

  /*! \brief Send PDU by copy
   *
   * Will get a new transaction,
   *  setup it with pdu as data and queryReplyMode flag,
   *  then send it with sendData(mdtPortTransaction*).
   *
   * \param pdu MODBUS PDU (see the MODBUS Application Protocol Specification for details)
   */
  int sendData(const QByteArray &pdu, bool queryReplyMode = false);

 public slots:

  /*! \brief Abort the scan process
   */
  void abortScan();

  /*! \brief Called by connect timeout timer
   */
  void abortTryToConnect();

  /*! \brief Called by the thread whenn a complete frame was readen
   */
  void fromThreadNewFrameReaden();

 private:

  QString pvKnownHostsFileName;
  bool pvAbortScan;
  bool pvAbortTryToConnect;
  // Helper members for Register service
  QList<int> pvRegisterValues;

  // Diseable copy
  Q_DISABLE_COPY(mdtModbusTcpPortManager);
};

#endif  // #ifndef MDT_MODBUS_TCP_PORTMANAGER_H
