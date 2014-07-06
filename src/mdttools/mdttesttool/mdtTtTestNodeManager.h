/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_NODE_MANAGER_H
#define MDT_TT_TEST_NODE_MANAGER_H

#include "mdtDeviceContainer.h"
#include "mdtDevice.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QList>
#include <memory>

class mdtTtTestNodeManagerWidget;

/*! \brief Helper class to access test node data and devices
 *
 * 
 */
class mdtTtTestNodeManager : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeManager(QObject *parent, QSqlDatabase db);

  /*! \brief Add a test node
   */
  

  /*! \brief Create and add a device
   */
  template <typename T> std::shared_ptr<T> addDevice(const QVariant & identification, const QString & nodeIdentification, const QString & deviceName)
  {
    std::shared_ptr<T> device;

    device = pvDevices.addDevice<T>(identification);
    device->setName(deviceName);
    emit deviceAdded(device);

    return device;
  }

  /*! \brief Get device of type T and that has given identification
   */
  template <typename T> std::shared_ptr<T> device(const QVariant & identification)
  {
    return pvDevices.device<T>(identification);
  }

  /*! \brief Get a list of all devices
   *
   * Note: list is rebuilt at each call of this method.
   */
  QList<std::shared_ptr<mdtDevice> > allDevices();

  /*! \brief Set the test node manager widget
   */
  ///void setTestNodeManagerWidget();

  /*! \brief Remove all devices
   */
  void clear();

 signals:

  /*! \brief Emited when a new device was added
   */
  void deviceAdded(std::shared_ptr<mdtDevice> device);

  /*! \brief Emited when a new device was removed
   */
  ///void deviceRemoved(mdtDevice* device);

  /*! \brief Emited when container was cleared
   */
  void cleared();

 private:

  Q_DISABLE_COPY(mdtTtTestNodeManager);

  QSqlDatabase pvDatabase;
  mdtDeviceContainer pvDevices;
  ///std::shared_ptr<mdtTtTestNodeManagerWidget> pvManagerWidget;
};

#endif  // #ifndef MDT_TT_TEST_NODE_MANAGER_H
