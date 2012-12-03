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
#ifndef MDT_USBTMC_PORT_MANAGER_H
#define MDT_USBTMC_PORT_MANAGER_H

#include "mdtUsbPort.h"
#include "mdtUsbPortThread.h"
#include "mdtPortManager.h"
#include "mdtPortInfo.h"
#include <QObject>
#include <QStringList>
#include <QList>

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
class mdtUsbtmcPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct a USBTMC port manager
   *
   * Creates a mdtPortConfig, a mdtUsbPort,
   *  and thread object.
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
   * \pre Manager must no running
   */
  QList<mdtPortInfo*> scan();

  /*! \brief Write data by copy
   *
   * Data will be encoded regarding USBTMC standard and passed to the mdtUsbPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param data Data to write
   * \return bTag ID on success or mdtAbstractPort::WriteQueueEmpty (< 0).
   * \pre Port must be set with setPort() before use of this method.
   */
  int writeData(QByteArray data);

  /*! \brief Send a read request to device
   *
   * USBTMC standard need that a read request is sent to device
   *  before we can read any data.
   *
   * \return bTag ID on success or value < 0 if write queue is full.
   */
  int sendReadRequest();

 public slots:

  /// \todo Essais ...
  ///void abort();

  /*! \brief Called by the thread whenn a complete frame was readen
   *
   */
  void fromThreadNewFrameReaden();

 private:

  quint8 pvCurrentWritebTag;  // USBTMC's frame bTag

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbtmcPortManager);
};

#endif  // #ifndef MDT_USBTMC_PORT_MANAGER_H
