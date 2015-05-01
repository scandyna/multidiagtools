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
#include "mdtMultiIoDevice.h"
#include "mdtDeviceAddress.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeRouteData.h"
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
 * Internally, a global device container (instance of mdtGlobalDeviceContainer)
 *  holds instruments in a unique way.
 */
class mdtTtTestNodeManager : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeManager(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtTtTestNodeManager(QSqlDatabase db);

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

  /*! \brief Get device of type T and that has given alias
   */
  template <typename T>
  std::shared_ptr<T> device(const QString & alias)
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

  /*! \brief Check coherence of I/O map between device and database
   *
   * When we use a test node that was defined in database (TestNode_tbl and TestNodeUnit_tbl),
   *  we mabe want that physical device to witch we are connected has the same layout than
   *  the one that is defined in database.
   *
   * Check is done as follow:
   *  For each I/O of a type that has its IoPosition set in TestNodeUnit_tbl,
   *  the I/O of the same type must exists in mdtMultiIoDevice container.
   *  If a mismatch was found, it is described and can be retrived with lastError().
   *  Note that other error can happen, on witch case this function will also fail.
   *
   * \param alias Alias of device to check. Corresponding device must be a mdtMultiIoDevice or subclass.
   */
  bool checkDeviceIoMapCoherence(const QString & alias);

  /*! \brief Set short label to device I/Os
   *
   * Will get device from global device container that has given alias,
   *  and that is a subclass of mdtMultiIoDevice.
   *  Then, informations are get from database (TestNode_tbl and TestNodeUnit_view)
   *  for test node that has given alias.
   *  For all test node units that have a IoPosition, SchemaPosition is set as short label in device's I/O container
   *  (see mdtDeviceIos for details).
   */
  bool setDeviceIosLabelShort(const QString & alias);

  /*! \brief Set relays to enable to given device
   *
   * Similar to addRelaysToEnable() , but will fisrt clear digital outputs states (set them OFF).
   */
  bool setRelaysToEnable(const std::vector<mdtTtTestNodeRouteRelay> & relays , std::shared_ptr<mdtMultiIoDevice> dev);

  /*! \brief Add relays to enable to given device
   *
   * Will set digital outputs in device that correspond to relay for each relays in list.
   *  Note: will only cache output states. Sending I/O map to device must be done explicitly.
   */
  bool addRelaysToEnable(const std::vector<mdtTtTestNodeRouteRelay> & relays , std::shared_ptr<mdtMultiIoDevice> dev);

  /*! \brief Get last error
   */
  inline mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Check I/O map coherence of given device I/O container
   */
  bool checkDeviceIoMapCoherence(const QVariant & testNodeId, const std::shared_ptr<mdtMultiIoDevice> & dev);

  /*! \brief Check if given analog input position exists in device I/O container
   */
  bool analogInputIoPositionExistsInDevice(int ioPosition, const std::shared_ptr<mdtDeviceIos> & ios) const;

  /*! \brief Check if given analog output position exists in device I/O container
   */
  bool analogOutputIoPositionExistsInDevice(int ioPosition, const std::shared_ptr<mdtDeviceIos> & ios) const;

  /*! \brief Check if given digital input position exists in device I/O container
   */
  bool digitalInputIoPositionExistsInDevice(int ioPosition, const std::shared_ptr<mdtDeviceIos> & ios) const;

  /*! \brief Check if given digital output position exists in device I/O container
   */
  bool digitalOutputIoPositionExistsInDevice(int ioPosition, const std::shared_ptr<mdtDeviceIos> & ios) const;

  /*! \brief Set short labels to analog inputs
   */
  bool setAnalogInputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant & testNodeId, const QString & alias);

  /*! \brief Set short labels to analog outputs
   */
  bool setAnalogOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant & testNodeId, const QString & alias);

  /*! \brief Set short labels to digital inputs
   */
  bool setDigitalInputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant & testNodeId, const QString & alias);

  /*! \brief Set short labels to digital outputs
   */
  bool setDigitalOutputsLabelShort(std::shared_ptr<mdtDeviceIos> ios, const QVariant & testNodeId, const QString & alias);

  /*! \brief Get a list of analog inputs IoPosition in database
   */
  std::vector<int> getAnalogInputsIoPositionsList(const QVariant & testNodeId, bool & ok);

  /*! \brief Get a list of analog outputs IoPosition in database
   */
  std::vector<int> getAnalogOutputsIoPositionsList(const QVariant & testNodeId, bool & ok);

  /*! \brief Get a list of digital inputs IoPosition in database
   */
  std::vector<int> getDigitalInputsIoPositionsList(const QVariant & testNodeId, bool & ok);

  /*! \brief Get a list of digital outputs IoPosition in database
   */
  std::vector<int> getDigitalOutputsIoPositionsList(const QVariant & testNodeId, bool & ok);

  Q_DISABLE_COPY(mdtTtTestNodeManager);

  QSqlDatabase pvDatabase;
  mdtGlobalDeviceContainer pvDevices;
  mdtError pvLastError;
};

#endif  // #ifndef MDT_TT_TEST_NODE_MANAGER_H
