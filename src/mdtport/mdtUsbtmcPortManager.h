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
#ifndef MDT_USBTMC_PORT_MANAGER_H
#define MDT_USBTMC_PORT_MANAGER_H

#include "mdtUsbPort.h"
#include "mdtUsbtmcPortThread.h"
#include "mdtUsbPortManager.h"
#include "mdtPortInfo.h"
#include <QObject>
#include <QStringList>
#include <QList>

/// \bug abortBulkIn() does not work properly

/*! \brief USBTMC port manager
 *
 * This is the easiest way to use the USBTMC port API.
 *
 * All needed object are created by constructor.
 *  To alter configuration, use config().
 *  To access the usb port object, use port().
 *
 * Use start() to begin read/write and stop to end.
 */
class mdtUsbtmcPortManager : public mdtUsbPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct a USBTMC port manager
   *
   * Create a thread and setup port.
   */
  mdtUsbtmcPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  ~mdtUsbtmcPortManager();

  /*! \brief Scan for available ports with a USBTMC compatible device attached
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Manager must be closed
   */
  QList<mdtPortInfo*> scan();

  /*! \brief Check if port manager is ready
   *
   * Internally, the currentState is used to check if port manager is ready.
   *  This implementation returns true if currentState is Ready.
   */
  bool isReady() const;

  /*! \brief Send a command to device
   *
   * At first, this method waits until the ready state is set calling isReady() , a frame is available in port's write frames pool,
   *  and until a transaction is possible (see waitTransactionPossible() ).
   *  This wait will not break event loop, so no GUI freeze occurs.
   *  If port manager is stopped during this wait (unhandled error, stop request),
   *  this method returns.
   *
   * Note that the wait will not break the GUI's event loop
   *  (see mdtPortManager::waitOnWriteReady() for details).
   *
   * \param command Command to send.
   * \return bTag on success, or a error < 0
   *          (see mdtUsbtmcPortManager::writeData() for details).
   */
  int sendCommand(const QByteArray &command);

  /*! \brief Send a query to device
   *
   * Will wait until it's possible to write.
   *  See sendData() and sendReadRequest() for details (used internally).
   *
   * \param query Query to send.
   * \return Result as string (empty string on error)
   *          Note that mdtFrameCodecScpi can be helpful to decode returned result.
   */
  QByteArray sendQuery(const QByteArray &query);

  /*! \brief Write data by copy
   *
   * At first, this method waits until the ready state is set calling isReady() , a frame is available in port's write frames pool,
   *  and until a transaction is possible (see waitTransactionPossible() ).
   *  This wait will not break event loop, so no GUI freeze occurs.
   *  If port manager is stopped during this wait (unhandled error, stop request),
   *  this method returns.
   *
   * Then, data will be encoded regarding USBTMC standard and passed to the mdtUsbPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * This method does not use transaction, because no response will be returned.
   *
   * \param data Data to write
   * \return bTag ID on success or mdtAbstractPort::WriteCanceled (< 0).
   * \pre Port must be set with setPort() before use of this method.
   */
  int sendData(const QByteArray &data);

  /*! \brief Send a read request to device
   *
   * USBTMC standard need that a read request is sent to device
   *  before we can read any data.
   *
   * At first, this method waits until the ready state is set calling isReady() , and a frame is available in port's write frames pool.
   *  This wait will not break event loop, so no GUI freeze occurs.
   *  If port manager is stopped during this wait (unhandled error, stop request),
   *  this method returns.
   *
   * \param transaction A pointer to a valid transaction, with queryReplyMode set.
   *                     (id will be set internally).
   * \return bTag ID on success or mdtAbstractPort::WriteCanceled (< 0).
   *          On failure, the transaction is restored to pool.
   * \pre port must be set
   * \pre transaction must be a valid pointer
   */
  int sendReadRequest(mdtPortTransaction *transaction);

  /*! \brief Send a read request to device
   *
   * USBTMC standard need that a read request is sent to device
   *  before we can read any data.
   *
   * \param enqueueResponse When data comes in, \todo SigName is emited.
   *                         In blocking mode, data must (additionally) be enqueued
   *                         until there are readen. So, for blocking mode ("script"),
   *                         set this parameter true.
   * \return bTag ID on success or value < 0 if write queue is full.
   */
  int sendReadRequest(bool enqueueResponse);

  /*! \brief Send a READ_STATUS_BYTE request thru the control endpoint
   *
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendReadStatusByteRequest();

  /*! \brief Abort bulk OUT
   *
   * \return 0 on success.
   * \pre port must be set before call of this method
   */
  int abortBulkOut(quint8 bTag);

  /*! \brief Send a INITIATE_ABORT_BULK_OUT request thru the control endpoint
   *
   * \see abortBulkOut()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendInitiateAbortBulkOutRequest(quint8 bTag);

  /*! \brief Send a CHECK_ABORT_BULK_OUT_STATUS request thru the control endpoint
   *
   * \see abortBulkOut()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendCheckAbortBulkOutStatusRequest(quint8 bTag);

 public slots:

  /*! \brief Called by the thread whenn a frame was readen
   */
  void fromThreadNewFrameReaden();

 private slots:

  /*! \brief Manage errors comming from usb port thread
   *
   * This implementation will handle some USB specific errors,
   *  change the current state emiting transistion signal.
   *  If state has changed, stateChanged() signal is emited.
   *
   * Some errors are handled here. For others, mdtUsbPortManager::onThreadsErrorOccured() is called.
   */
  void onThreadsErrorOccured(int error);

 private:

  /*! \brief Not usable here
   */
  int sendData(mdtPortTransaction *transaction);

  /*! \brief Not usable here
   */
  int sendData(const QByteArray &data, bool queryReplyMode);

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbtmcPortManager);
};

#endif  // #ifndef MDT_USBTMC_PORT_MANAGER_H
