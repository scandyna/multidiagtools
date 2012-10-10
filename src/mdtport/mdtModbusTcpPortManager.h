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
#ifndef MDT_MODBUS_TCP_PORTMANAGER_H
#define MDT_MODBUS_TCP_PORTMANAGER_H

#include "mdtPortManager.h"
#include "mdtPortInfo.h"
#include "mdtTcpSocket.h"
#include <QStringList>
#include <QList>
#include <QHash>

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
   * \param hosts A list of hosts to scan. Each host must be set with format hostname:port, or ip:port.
   *               Note that MODBUS/TCP default port is 502.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Manager must no running
   */
  QList<mdtPortInfo*> scan(const QStringList &hosts, int timeout = 500);

  /*! \brief Write data by copy
   *
   * Data will be encoded regarding MODBUS/TCP standard and passed to the mdtTcpSocket's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * Note that the transaction ID is incremented a each write request.
   *  If you want to specify it, use writeData(QByteArray, quint16) .
   *
   * \param pdu MODBUS PDU (see the MODBUS Application Protocol Specification for details)
   * \return True on success. If the maximum of authorized transactions are reached, false is returned.
   *          Note: internally, the writeFramesPool size of mdtAbstractPort is used to fix this, and
   *                this is configurable in mdtPortConfig. See mdtPortManager::config() .
   * \pre Port must be set with setPort() before use of this method.
   */
  bool writeData(QByteArray pdu);

  /*! \brief Write data by copy
   *
   * This is the same as writeData(QByteArray) , but you can specify
   *  a transaction ID.
   *
   * \param pdu MODBUS PDU (see the MODBUS Application Protocol Specification for details)
   * \param transactionId Transaction identifier. Will be encoded in MBAP header.
   * \return True on success.
   * \pre Port must be set with setPort() before use of this method.
   */
  bool writeData(const QByteArray &pdu, quint16 transactionId);

  /*! \brief Wait until a complete frame is available
   *
   * This method will return when a complete frame was readen.
   *  This is usefull for query/answer protocols.
   *
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   *
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True if Ok, false on timeout
   */
  bool waitReadenFrame(int timeout = 500);

  /*! \brief Get all readen data (PDUs)
   *
   * Get a copy of all currently available data.
   *  Note: the list of data must be cleared explicitly
   *   with QHash::clear() after data are used.
   *   (or remove each item with, for.ex. QHash::take() )
   */
  QHash<quint16, QByteArray> &readenFrames();

 public slots:

  /*! \brief Called by the thread whenn a complete frame was readen
   */
  void fromThreadNewFrameReaden();

 private:

  quint16 pvTransactionId;
  QHash<quint16, QByteArray> pvReadenPdus;
  QHash<quint16, QByteArray> pvReadenPdusCopy;

  // Diseable copy
  Q_DISABLE_COPY(mdtModbusTcpPortManager);
};

#endif  // #ifndef MDT_MODBUS_TCP_PORTMANAGER_H
