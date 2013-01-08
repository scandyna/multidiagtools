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
#ifndef MDT_USB_PORT_H
#define MDT_USB_PORT_H

#include "mdtAbstractPort.h"
#include <libusb-1.0/libusb.h>
#include "mdtFrameUsbControl.h"

/*! \brief USB port I/O port class
 *
 * Built on top of libusb-1.x API
 *
 * Note about the port name (set with setPortName() ):<br>
 *  Because it can happen that multiple devices with same vendor ID and product ID
 *   can be attached to system, this class must provide a unique identifier when
 *   (re-)open the port.
 *  Because of this, the choosen format is: VID=0x1234:PID=0x1234:SID=0x1234:bInterfaceNumber=0.
 *   VID is the Vendor ID, PID the Product ID, SID the Serial ID (also called Serial Number) and bInterfaceNumber the interface number
 *   (some devices have mor than one interface, f.ex. sound cards, keboard/mouse receiver, ...).
 *   SID and bInterfaceNumber are optional. In this case, the first port with given vendor ID and device ID is open.
 *   In brief, following names are allowed:
 *   - VID=0x1234:PID=0x5678 (Open the first port with given Vendor ID and Product ID. bInterfaceNumber is 0)
 *   - VID=0x1234:PID=0x5678:SID=ABC1234 (Open the first port with given Vendor ID, Product ID and Serial ID. bInterfaceNumber is 0)
 *   - VID=0x1234:PID=0x5678:bInterfaceNumber=1 (Open the first port with given Vendor ID and Product ID. bInterfaceNumber is 1)
 *   - VID=0x1234:PID=0x5678:SID=ABC1234:bInterfaceNumber=1 (Open the first port with given Vendor ID, Product ID and Serial ID. bInterfaceNumber is 1)
 * The last example in the above list is the format returned by mdtUsbPortManager::scan().
 *
 * For read and write of data, the mdtAbstractPort's API is used.
 *
 * The control transfer is a little bit different:
 *  - If the main thread (f.ex. GUI thread) wants to send a request,
 *     it locks the mutex using lockMutex(), then it takes a frame
 *     in pool with controlFramesPool().
 *     Finaly, query is submitted using addControlRequest(), witch will
 *     wake the write wait condition, and main thread unlocks the mutex with unlockMutex().
 *  - The thread calls handleControlQueries() periodically, witch will init
 *     a new control transfer if a query is available in queries queue.
 *  - Each time waitEventWriteReady() and waitForReadyRead() are called,
 *     handleControlResponses() is called to check if a control response
 *     is available. If true, it will be enqueued in control responses queue.
 *  - The thread periodically checks if a response is available using controlResponseFrames()
 *     queue's size, and emit \todo signalName if a control response is available.
 *
 * If a additional (to read and write) interrupt IN endpoint exists, it is handled as message IN as follow:
 *  - Each time waitEventWriteReady() or waitForReadyRead() is called,
 *     handleMessageIn() is called, witch will initiate a new transfer if needed.
 *     If a transfer is complete, frame will be enqueued in message IN queue.
 *  - The thread periodically checks if a message IN is available using messageInFrames()
 *     queue's size, and emit \todo signalName if a message IN is available.
 */
class mdtUsbPort : public mdtAbstractPort
{
 public:

  mdtUsbPort(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtUsbPort();

  /*! \brief Reconnect device
   *
   * If one of the method returns a Disconnected error,
   *  the thread will call this method to try to reconnect.
   *
   * \param timeout Timeout [ms]
   * \return NoError if connection could be done.
   *          Disconnected if connection could not be done,
   *          in wich case the thread will retry (until max retry).
   *          A UnhandledError can be returned.
   */
  error_t reconnect(int timeout);

  /*! \brief Set the read data timeout
   *
   * The mutex is not handled by this method.
   */
  void setReadTimeout(int timeout);

  /*! \brief Set the write data timeout
   *
   * The mutex is not handled by this method.
   */
  void setWriteTimeout(int timeout);

  /*! \brief Get the control pool queue
   *
   * This queue contains the pool of control frames.
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrameUsbControl*> &controlFramesPool();

  /*! \brief Add a control request
   *
   * Once the frame is added to the control queries queue,
   *  waiting thread will be woken up (if waiting)
   *  and will send the frame.
   *
   * The mutex must be locked before calling this method,
   *  and still locked inside.
   *
   * \param frame Control frame
   * \param setwIndexAsbInterfaceNumber If true, wIndex will be set with current bInterfaceNumber
   * 
   * \pre frame must be a valid pointer.
   */
  void addControlRequest(mdtFrameUsbControl *frame, bool setwIndexAsbInterfaceNumber = false);

  /*! \brief Check about pending control queries and init a new transfer if needed
   *
   * If a frame is available in control query queue,
   *  a transfer will be initialized and submitted
   *  to libusb.
   *
   * The thread can call this method periodically.
   *  If nothing is to do, it will simply return NoError.
   *
   * The mutex must be locked before calling this method,
   *  and still locked inside.
   */
  error_t handleControlQueries();

  /*! \brief Cancel current control transfer
   *
   * If a control transfer is pending it will be cancelled.
   *
   * Mutex is not handled by this method.
   */
  error_t cancelControlTransfer();

  /*! \brief Check pending control transfer
   *
   * If a control transfer is pending,
   *  complete flag will be checked.
   *  If transfer is complete,
   *  some flags are checked, and
   *  frame is enqueued in response queue if Ok.
   *
   * This method is called by waitEventWriteReady()
   *  and waitForReadyRead(), witch are called from port
   *  thread.
   *
   * Mutex is not handled by this method.
   */
  error_t handleControlResponses();

  /*! \brief Get the control responses queue
   *
   * This queue contains the response frames returned from device.
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrameUsbControl*> &controlResponseFrames();

  /*! \brief Request thread to process one read transfer
   *
   * This is, for example, used by USBTMC bulk in abort process.
   *  If such request is pending, thread will do a single
   *  read transfer.
   *
   * Mutex is not handled by this method
   */
  void setSingleReadTransferRequest();

  /*! \brief Check if a single read request is pending
   *
   * This method is used by USB thread, and should
   *  not be used else.
   *
   * Mutex is not handled by this method.
   */
  bool singleReadTransferRequestPending() const;

  /*! \brief Reset single read transfer request flag
   */
  void resetSingleReadTransferRequest();

  /*! \brief Get read buffer size
   *
   * This is the same as wMaxPacketSize of read endpoint.
   *
   * This method is used by USB thread, and should
   *  not be used else.
   *
   * Mutex is not handled by this method.
   */
  int readBufferSize() const;

  /*! \brief Request a new bulk/interrupt input transfer
   *
   * Will fill and init a new bulk/interrupt (depending setup) transfer
   *  and submit it to libusb.
   */
  error_t initReadTransfer(qint64 maxSize);

  /*! \brief Cancel current bulk/interrupt input transfer
   *
   * If a input transfer is pending it will be cancelled.
   *
   * Mutex is not handled by this method.
   */
  error_t cancelReadTransfer();

  /*! \brief Wait until data is available on port.
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitForReadyRead();

  /*! \brief Read data from port
   *
   * This method is called from mdtUsbPortThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   *
   * \pre A transfert must be initialized with initReadTransfer() before using this method.
   */
  qint64 read(char *data, qint64 maxSize);

  /*! \brief Request a new interrupt input transfer for additional message input
   *
   * Will fill and init a new interrupt transfer
   *  and submit it to libusb.
   */
  error_t initMessageInTransfer(qint64 maxSize);

  /*! \brief Cancel current interrupt message input transfer
   *
   * If a input message transfer is pending it will be cancelled.
   *
   * Mutex is not handled by this method.
   */
  error_t cancelMessageInTransfer();

  /*! \brief Handle message IN
   *
   * Will init a new message IN transfer if needed.
   *  If one is pending, it will check about its completion,
   *  and enqueue the data to message IN frames queue if complete.
   */
  error_t handleMessageIn();

  /*! \brief Get the message IN frames pool
   *
   * This queue contains the pool of message IN frames.
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &messageInFramesPool();

  /*! \brief Get the message IN frames
   *
   * This queue contains the message IN frames.
   *
   * Mutex is not handled by this method.
   */
  QQueue<mdtFrame*> &messageInFrames();

  /*! \brief Request a new bulk/interrupt out transfer
   *
   * Will fill and init a new bulk/interrupt (depending setup) transfer
   *  and submit it to libusb.
   */
  error_t initWriteTransfer(const char *data, qint64 maxSize);

  /*! \brief Cancel current bulk/interrupt input transfer
   *
   * If a input transfer is pending it will be cancelled.
   *
   * Mutex is not handled by this method.
   */
  error_t cancelWriteTransfer();

  /*! \brief Wait until data can be written to port.
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitEventWriteReady();

  /*! \brief Write data to port
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   */
  qint64 write(const char *data, qint64 maxSize);

  /*! \brief Transfert callback for asynch I/O
   */
  static void transferCallback(struct libusb_transfer *transfer);

  /*! \brief Cancel pending transfers
   *
   * Will call cancelReadTransfer() , cancelReadTransfer().
   *
   * This method locks the mutex.
   */
  error_t cancelTransfers();

  /*! \brief Calle libusb handle_event*() function
   *
   * Notes: must be called only by I/O thread (typically mdtUsbPortThread).
   * This method unlocks the mutex during wait (must be locked by call, is locked again before return)
   *
   * \param timeout Wait timeout. If null, the port's internal defined timeout is used regarding endpoint
   * \param endpoint Concerned endpoint
   */
  error_t handleUsbEvents(struct timeval *timeout, quint8 endpoint);

  /*! \brief Get current read endpoint address
   *
   * Note: mutex is not handled by this method
   */
  quint8 currentReadEndpointAddress() const;

 private:

  /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must not be open whenn calling this method.
   */
  error_t pvOpen();

  /*! \brief Close port
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   */
  void pvClose();

  /*! \brief Setup port with given configurations.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   * \pre A valid configuration must be set before calling this method.
   */
  error_t pvSetup();

  /*! \brief Flush read port
   *
   * This method is called from flushIn(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   */
  void pvFlushIn();

  /*! \brief Flush write port
   *
   * This method is called from flushout(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   */
  void pvFlushOut();

  // Map a libusb error to mdtAbstractPort::error_t
  //  The endpoint helps to know if a error is associated
  //  to a read, write or control process.
  //  F.ex. timeout error will be mapped to ReadTimeout if read endpoint is concerned.
  error_t mapUsbError(int error, quint8 endpoint);

  // Get a text for given libusb error code
  QString errorText(int errorCode) const;

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbPort);

  unsigned int pvReadTimeout;
  struct timeval pvReadTimeoutTv;
  unsigned int pvWriteTimeout;
  struct timeval pvWriteTimeoutTv;
  libusb_context *pvLibusbContext;
  libusb_device_handle *pvHandle;
  int pvbInterfaceNumber;
  /*
   * Control endpoint members
   */
  char *pvControlBuffer;
  int pvControlBufferSize;
  libusb_transfer *pvControlTransfer;
  bool pvControlTransferPending;       // Flag to see if a transfer is pending
  int pvControlTransferComplete;       // Will be stored in transfer struct
  // Frames queues
  QQueue<mdtFrameUsbControl*> pvControlFramesPool;
  QQueue<mdtFrameUsbControl*> pvControlQueryFrames;
  QQueue<mdtFrameUsbControl*> pvControlResponseFrames;
  /*
   * Data bulk/interrupt IN endpoint members
   */
  char *pvReadBuffer;
  int pvReadBufferSize;
  quint8 pvReadEndpointAddress;
  libusb_transfer_type pvReadTransfertType;
  libusb_transfer *pvReadTransfer;
  bool pvReadTransferPending;       // Flag to see if a transfer is pending
  int pvReadTransferComplete;       // Will be stored in transfer struct
  bool pvSingleReadTransferRequestPending;  // See singleReadTransferRequestPending() and addSingleReadTransferRequest()
  /*
   * Data bulk/interrupt OUT endpoint members
   */
  char *pvWriteBuffer;
  int pvWriteBufferSize;
  quint8 pvWriteEndpointAddress;
  libusb_transfer_type pvWriteTransfertType;
  libusb_transfer *pvWriteTransfer;
  bool pvWriteTransferPending;      // Flag to see if a transfer is pending
  int pvWriteTransferComplete;      // Will be stored in transfer struct
  /*
   * Some devices have a additional interrupt IN endpoint
   *  for urgent messages. Other use interrupt IN endpoint
   *  for standard I/O. This is the reason to use another
   *  name than ...interruptIn... for these members
   */
  char *pvMessageInBuffer;
  int pvMessageInBufferSize;
  quint8 pvMessageInEndpointAddress;
  libusb_transfer *pvMessageInTransfer;
  bool pvMessageInTransferPending;  // Flag to see if a transfer is pending
  int pvMessageInTransferComplete;  // Will be stored in transfer struct
  // Frames queues
  QQueue<mdtFrame*> pvMessageInFramesPool;  // Frames pool
  QQueue<mdtFrame*> pvMessageInFrames;      // Incomming message frames
};

#endif // #ifndef MDT_USB_PORT_H
