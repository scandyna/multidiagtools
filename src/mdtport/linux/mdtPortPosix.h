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

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <QObject>
#include <QString>

class mdtPortPosix : public mdtAbstractPort
{
 public:

  mdtPortPosix(QObject *parent = 0);
  ~mdtPortPosix();

  // Implemtation of mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implemtation of mdtAbstractPort
  bool open(mdtPortConfig &cfg);

  // Implemtation of mdtAbstractPort
  void close();

  // Implemtation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  bool waitForReadyRead();

  // Implemtation of mdtAbstractPort method
  qint64 read(char *data, qint64 maxSize);

  // Implemtation of mdtAbstractPort method
  void flushIn();

  // Implemtation of mdtAbstractPort
  bool waitEventWriteReady();

  // Implemtation of mdtAbstractPort method
  qint64 write(const char *data, qint64 maxSize);

  // Implemtation of mdtAbstractPort method
  void flushOut();

 protected:

  int pvFd;

 private:

  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;
};

#endif  // #ifndef MDT_PORT_POSIX_H
