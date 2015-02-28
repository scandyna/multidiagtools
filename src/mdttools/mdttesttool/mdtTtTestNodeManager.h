/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtDeviceAddress.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtError.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QList>
#include <memory>

class mdtDeviceIos;
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
  template <typename T>
  std::shared_ptr<T> addDevice(const mdtDeviceAddress & address)
  {
    return pvDevices->addDevice<T>(address);
  }

  /*! \brief Create and add a device
   */
  template <typename T>
  std::shared_ptr<T> addDevice(const QString & alias)
  {
    return pvDevices->addDevice<T>(alias);
  }

//   template <typename T> std::shared_ptr<T> addDevice(const QVariant & identification, const QString & nodeIdentification, const QString & deviceName)
//   {
//     QString devName;
// 
//     devName = deviceName;
//     if(!nodeIdentification.isEmpty()){
//       devName += tr(" (Node ID: ") + nodeIdentification + tr(")");
//     }
//     ///return pvDevices->addDevice<T>(identification, devName);
//   }

  /*! \brief Get device of type T and that has given alias
   */
  template <typename T> std::shared_ptr<T> device(const QString & alias)
  {
    return pvDevices->device<T>(alias);
  }

  /*! \brief Get a list of all devices
   *
   * Note: list is rebuilt at each call of this method.
   */
  QList<std::shared_ptr<mdtDevice> > allDevices();

  /*! \brief Get device container object
   */
  std::shared_ptr<mdtDeviceContainer> container();

  /*! \brief Remove all devices
   */
  void clear();

  /*! \brief Set short label to device I/Os
   *
   * Will get I/Os contained in device (see mdtDeviceIos for details).
   *  and Test node units contained in given Test node.
   *  For each I/O, SchemaPosition of test node unit is used,
   *  and labelShort is set to I/O.
   */
  ///bool setDeviceIosLabelShort(const QVariant & testNodeId, const QVariant & deviceIdentification);
  bool setDeviceIosLabelShort(const QVariant & testNodeId);

  /*! \brief Get last error
   */
  inline mdtError lastError() const { return pvLastError; }

 private:

  /*! \brief Set short labels to digital outputs
   */
  bool setDigitalOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant & testNodeId, const QString & alias);

  Q_DISABLE_COPY(mdtTtTestNodeManager);

  QSqlDatabase pvDatabase;
  std::shared_ptr<mdtDeviceContainer> pvDevices;
  mdtError pvLastError;
};

#endif  // #ifndef MDT_TT_TEST_NODE_MANAGER_H
