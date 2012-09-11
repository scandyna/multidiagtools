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
#ifndef _MDTDEVICEU3606A_H
#define _MDTDEVICEU3606A_H


#include "mdtDevice.h"
//#include "linux/mdtUsbtmcPortManager.h"
#include "mdtUsbtmcPortManager.h"
#include "mdtFrameCodecScpiU3606A.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

class mdtDeviceU3606A : public mdtDevice
{
 Q_OBJECT

 public:

  mdtDeviceU3606A(QObject *parent = 0);
  ~mdtDeviceU3606A();
  
  /*! \brief Scan for attached devices
   * 
   * Scan for available ports, then it
   * get the IDN of found device.<br>
   * If IDN returns a U3606A, the port 
   * is added to ports list.
   * \returns String list of ports that have a U3606A attached
   */
  QStringList scan();

  /*! \brief Select a port
   *
   * Before using this function, the
   * specific device scan() method must be used
   * to find ports with attached device.
   * Then, pass the result at argument
   */
  bool selectPort(const QStringList &ports, int index);

  /*! \brief Select a port
   *
   * Before using this function, the
   * specific device scan() method must be used
   * to find ports with attached device.
   */
  bool selectPort(const QString &port);

 public slots:

  /*! \brief Get the IDN
   * 
   * Will get the IDN from device.
   * The signal idnChanged()
   * will be emited.
   */
  void getIdn();
  
  void essais();

 signals:

  /*! \brief Emitted when new IDN is available
   * 
   * The list contains following data:
   *  - Manufacturer
   *  - Model
   *  - Serial number
   *  - Interface revision number
   *  - Measurement board revision number
   *  - Source board revision number
   * \sa getIdn()
   */
  void idnChanged(const QStringList &data);
  
 private:

  mdtUsbtmcPortManager pvPortManager;
  mdtFrameCodecScpiU3606A pvCodec;

};
#endif
