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
#ifndef MDT_PORT_WRITE_THREAD_H
#define MDT_PORT_WRITE_THREAD_H

#include "mdtPortThread.h"
#include <QObject>

class mdtPortWriteThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtPortWriteThread(QObject *parent = 0);

  /*! \brief Stop the thread
   */
  void stop();

 signals:

  /*! \brief Emited when a complete frame was written
   * \sa mdtPortManager
   * \sa mdtPort
   */
  ///void frameWritten();

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_PORT_WRITE_THREAD_H
