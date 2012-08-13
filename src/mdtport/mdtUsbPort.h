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
  static void transfertCallback(struct libusb_transfer *transfer);

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

  // Get a text for given libusb error code
  QString errorText(int errorCode) const;

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbPort);

  unsigned int pvReadTimeout;
  unsigned int pvWriteTimeout;
  libusb_context *pvLibusbContext;
  libusb_device_handle *pvHandle;
};

#endif // #ifndef MDT_USB_PORT_H
