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

  // Implemtation of mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implemtation of mdtAbstractPort
  bool open(mdtPortConfig &cfg);

  // Overload of QIODevice method
  void close();

  // Implemtation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  // This method is called the reader thread
  // Implemtation of mdtAbstractPort
  bool waitForReadyRead();

  // Implemtation of mdtAbstractPort
  qint64 read(char *data, qint64 maxSize);

  // This method is called the writer thread
  // Implemtation of mdtAbstractPort
  bool waitEventWriteReady();

  // Implemtation of mdtAbstractPort
  qint64 write(const char *data, qint64 maxSize);

  // Tell the thread that a frame can be read
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

  QWaitCondition pvReadCondition;
  QWaitCondition pvWriteCondition;
  int pvReadTimeout;
  int pvWriteTimeout;
  int pvFd;
};

#endif  // #ifndef MDT_USBTMC_PORT_H
