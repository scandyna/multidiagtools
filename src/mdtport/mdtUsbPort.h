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

struct mdt_usb_port_transfer_data
{
  int flag1;
};

/*! \brief USB port I/O port class
 *
 * Built on top of libusb-1.x API
 *
 * \todo Note about setPortName() with vid:pid syntax
 */
class mdtUsbPort : public mdtAbstractPort
{
 public:

  mdtUsbPort(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtUsbPort();

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

  /*! \brief Wait until data is available on port.
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitForReadyRead();

  /*! \brief Read data from port
   *
   * This method is called from mdtPortReadThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   */
  qint64 read(char *data, qint64 maxSize);

  /*! \brief Flush read buffers
   */
  void flushIn();

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

  /*! \brief Flush write buffers
   */
  void flushOut();

  /*! \brief Transfert callback for asynch I/O
   */
  static void transferCallback(struct libusb_transfer *transfer);

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
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushIn() and should not
   *  be handled here.
   */
  void pvFlushIn();

  /*! \brief Flush write port
   *
   * This method is called from flushout(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushOut() and should not
   *  be handled here.
   */
  void pvFlushOut();

  // Get a text for given libusb error code
  QString errorText(int errorCode) const;

  // Get poll() file descriptors from libusb
  struct pollfd *getLibusbPollFds(int *numFds);

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbPort);

  unsigned int pvReadTimeout;
  struct timeval pvReadTimeoutTv;
  unsigned int pvWriteTimeout;
  struct timeval pvWriteTimeoutTv;
  libusb_context *pvLibusbContext;
  libusb_device_handle *pvHandle;
  // Data buffers
  char *pvReadBuffer;
  int pvReadBufferSize;
  int pvReadenLength;
  quint8 pvReadEndpointAddress;
  libusb_transfer_type pvReadTransfertType;
  char *pvWriteBuffer;
  int pvWriteBufferSize;
  int pvWrittenLength;
  quint8 pvWriteEndpointAddress;
  libusb_transfer_type pvWriteTransfertType;
  // Interrupt in (available on some devices)
  char *pvInterruptInBuffer;
  int pvInterruptInBufferSize;
  // Transfers
  libusb_transfer *pvReadTransfer;
  bool pvReadTransferPending;       // Flag te see if a transfer is pending
  bool pvReadTransferComplete;      // Will be stored in transfer struct
  libusb_transfer *pvWriteTransfer;
  bool pvWriteTransferPending;      // Flag te see if a transfer is pending
  bool pvWriteTransferComplete;      // Will be stored in transfer struct
};

#endif // #ifndef MDT_USB_PORT_H
