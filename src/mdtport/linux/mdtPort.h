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
#ifndef MDT_PORT_POSIX_H
#define MDT_PORT_POSIX_H

#include "mdtAbstractPort.h"
#include "mdtPortLock.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <QObject>
#include <QString>

class mdtPort : public mdtAbstractPort
{
 public:

  mdtPort(QObject *parent = 0);
  ~mdtPort();

  /*! \brief Set the read data timeout
   */
  void setReadTimeout(int timeout);

  /*! \brief Set the write data timeout
   */
  void setWriteTimeout(int timeout);

  /*! \brief Wait until data is available on port
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

  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;
  mdtPortLock *pvPortLock;
  int pvFd;
};

#endif  // #ifndef MDT_PORT_POSIX_H
