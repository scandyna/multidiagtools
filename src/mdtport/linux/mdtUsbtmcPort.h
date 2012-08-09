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
#ifndef MDT_USBTMC_PORT_H
#define MDT_USBTMC_PORT_H

#include "mdtAbstractPort.h"
#include "mdtPortLock.h"
#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <unistd.h>
#include <fcntl.h>

/*! \brief USBTMC device file calss
 * 
 * With USBTMC, when we try to read, and no data are available,
 * both read and write threads are blocked.<br>
 * To start the read, call the readOneFrame() method.<br>
 * To start the write, call the writeOneFrame() method.
 */
class mdtUsbtmcPort : public mdtAbstractPort
{
 Q_OBJECT

 public:

  mdtUsbtmcPort(QObject *parent = 0);
  ~mdtUsbtmcPort();

  /*! \brief Set the read data timeout
   */
  void setReadTimeout(int timeout);

  /*! \brief Set the write data timeout
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
   */
  qint64 read(char *data, qint64 maxSize);

  /*! \brief Wait until data can be written to port.
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitEventWriteReady();

  /*! \brief Write data to port
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.
   * Mutex is not handled by this method.
   */
  qint64 write(const char *data, qint64 maxSize);

  // Tell the thread that a frame can be read
  /// Write a frame... \todo fix this class..
  void writeOneFrame();

 public slots:

  /*! \brief Tell the thread that a frame can be read
   * 
   * In mdtUsbtmcPortManager, this slot is connected to
   * the write thread, and called each time a frame was written.<br>
   * If the waitingFrame flag is true, the read thread will be
   * notified that data must be read.
   * \sa setWaitingFrame()
   * \sa mdtPortWriteThread
   */
  void readOneFrame();

 private:

   /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must not be open whenn calling this method.
   *
   * \sa close()
   * \sa error_t
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
   *
   * \sa error_t
   */
  error_t pvSetup();

  QWaitCondition pvReadCondition;
  QWaitCondition pvWriteCondition;
  int pvReadTimeout;
  int pvWriteTimeout;
  int pvFd;
  mdtPortLock *pvPortLock;
};

#endif  // #ifndef MDT_USBTMC_PORT_H
