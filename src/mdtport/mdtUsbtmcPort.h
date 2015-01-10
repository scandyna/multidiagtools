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
#ifndef MDT_USBTMC_PORT_H
#define MDT_USBTMC_PORT_H

#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtError.h"
#include <QObject>
#include <QList>
#include <QByteArray>
#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <mutex>

class mdtUsbtmcPortThreadNew;
class mdtUsbtmcTransferHandler;
class mdtUsbDeviceList;

/*! \brief Port to communicate with a USBTMC device
 */
class mdtUsbtmcPort : public QObject
{
 Q_OBJECT

 public:

  /*! \brief USBTMC transaction state
   */
  enum class TransactionState_t
  {
    Pending = 0,        /*!< Transaction is in progress */
    Done,               /*!< Transaction done without error */
    Aborted,            /*!< Transaction aborted */
    Error,              /*!< Transaction finished because a error occured */
    DeviceDisconnected  /*!< Device has been disconnected (note: currently not properly reported) */
  };

  /*! \brief Constructor
   */
  mdtUsbtmcPort(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtUsbtmcPort();

  /*! \brief Enumerate attached USBTMC devices
   */
  QList<mdtUsbDeviceDescriptor> scan();

  /*! \brief Open device represented by given descriptor
   *
   * \param deviceDescriptor USB device descriptor to open. Must be a item returned by scan().
   * \param bInterfaceNumber Interface number to use.
   * \param clearDeviceList If true, internal device descriptors list will be cleared once device was open.
   */
  bool openDevice(const mdtUsbDeviceDescriptor & deviceDescriptor, uint8_t bInterfaceNumber = 0, bool clearDeviceList = true);

  /*! \brief Close
   */
  void close();

  /*! \brief Send a command to device
   *
   * \param command Command to send
   * \param timeout Timeout [ms]
   */
  bool sendCommand(const QByteArray & command, int timeout = 30000);

  /*! \brief Send a query to device
   *
   * \param query Query to send
   * \param timeout Timeout [ms]
   */
  QByteArray sendQuery(const QByteArray & query, int timeout = 30000);

  /*! \brief Wait some time without breaking main event loop
   *
   * \param t time [ms]
   */
  void wait(int t);

  /*! \brief Clear bulk I/O
   *
   * Note: is allready called by openDevice().
   */
  bool clearBulkIo();

  /*! \brief Get last error
   */
  mdtError lastError()
  {
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    return pvLastError;
  }

  /*! \brief Signal bulk transaction done
   *
   * Note: used by mdtUsbtmcTransferHandler
   */
  void thSetTransactionDone()
  {
    emit transactionFinished(TransactionState_t::Done);
  }

  /*! \brief Signal bulk transaction aborted
   *
   * Note: used by mdtUsbtmcTransferHandler
   */
  void thSetTransactionAborted()
  {
    emit transactionFinished(TransactionState_t::Aborted);
  }

  /*! \brief Signal device disconnected
   *
   * Note: used by mdtUsbtmcTransferHandler
   */
  void thSetDeviceDisconnected()
  {
    emit transactionFinished(TransactionState_t::DeviceDisconnected);
  }

  /*! \brief Signal error
   *
   * Note: used by mdtUsbtmcTransferHandler
   */
//   void thSetError(const mdtError & error)
//   {
//     pvLastErrorMutex.lock();
//     pvLastError = error;
//     pvLastErrorMutex.unlock();
//     emit transactionFinished(TransactionState_t::Error);
//   }

 signals:

  /*! \brief Signal that transaction is finished
   * \sa setTransactionState().
   */
  void transactionFinished(TransactionState_t s);

 public slots:

  /*! \brief Set error
   *
   * Will also generate a event in main thread and wake up QCoreApplication::processEvents()
   *
   * Note: used by mdtUsbtmcTransferHandler and mdtUsbtmcPortThread
   */
  void setError(const mdtError & error);

 private slots:

  /*! \brief Set transaction state
   *
   * Will also generate a event in main thread and wake up QCoreApplication::processEvents()
   */
  void setTransactionState(TransactionState_t s);

  /*! \brief Set pvWaitTimeReached flag
   *
   * See wait()
   */
  void setWaitTimeReached();

 private:

  /*! \brief Wait until transaction is finished
   *
   * After this function returns,
   *  caller must check pvTransactionState
   */
  void waitTransactionFinished();

  /*! \brief Build port thread
   *
   * \pre must be called only once
   */
  bool initThread();

  /*! \brief Init libusb context
   *
   * \pre must be called only once
   */
  bool initLibusbConext();

  Q_DISABLE_COPY(mdtUsbtmcPort);

  libusb_context *pvUsbContext;
  mdtUsbtmcTransferHandler *pvTransferHandler;
  mdtUsbtmcPortThreadNew *pvThread;
  mdtUsbDeviceList *pvDeviceList;
  mdtError pvLastError;
  std::mutex pvLastErrorMutex;
  TransactionState_t pvTransactionState;
  bool pvWaitTimeReached; // Used by wait()
};

#endif // #ifndef MDT_USBTMC_PORT_H
