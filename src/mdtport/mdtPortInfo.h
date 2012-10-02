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
#ifndef MDT_PORT_INFO_H
#define MDT_PORT_INFO_H

#include <QList>
#include <QString>
#include <QMetaType>
#include "mdtDeviceInfo.h"

/*! \brief Contains informations about a port
 *
 * Used by mdtPortManager in scan() method.
 */
class mdtPortInfo
{
 public:

  mdtPortInfo();
  mdtPortInfo(const mdtPortInfo &other);
  ~mdtPortInfo();

  /*! \brief Set the port name
   */
  void setPortName(const QString &name);

  /*! \brief Get the port name
   */
  QString portName() const;

  /*! \brief Add a device
   */
  void addDevice(mdtDeviceInfo *device);

  /*! \brief Get the list of devices informations
   *
   * Note that content of returned list is not a copy,
   *  and that all items becomes ivalid after
   *  this (mdtPortInfo) object was deleted.
   */
  QList<mdtDeviceInfo*> &deviceInfoList();

  /*! \brief Copy operator
   */
  mdtPortInfo &operator=(const mdtPortInfo &other);

  /*! \brief Comparaison operator
   */
  bool operator==(const mdtPortInfo &other);

  /*! \brief Comparaison operator
   */
  bool operator!=(const mdtPortInfo &other);

 private:

  QList<mdtDeviceInfo*> pvDeviceInfoList;
  QString pvPortName;
};

Q_DECLARE_METATYPE(mdtPortInfo)

#endif  // #ifndef MDT_PORT_INFO_H
