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
#ifndef MDT_DEVICE_CONTAINER_H
#define MDT_DEVICE_CONTAINER_H

#include "mdtDevice.h"
#include "mdtDeviceAddress.h"
#include <vector>
#include <memory>
#include <typeinfo>
#include <QVariant>
#include <QList>

class QObject;

/*! \brief Contains different type of devices
 *
 * For some applications, many devices of different type must be handled.
 *
 * This class helps for this.
 *
 * \sa mdtGlobalDeviceContainer
 */
class mdtDeviceContainer : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Construct a empty container
   */
  mdtDeviceContainer(QObject *parent = 0);

  /*! \brief Create and add a new device to container
   *
   * Use this version if device address is allready known.
   */
  template <typename T>
  std::shared_ptr<T> addDevice(const mdtDeviceAddress & address)
  {
    std::shared_ptr<T> dev(new T);
    dev->setDeviceAddress(address);
    pvDevices.push_back(dev);
    emit deviceAdded(dev);

    return dev;
  }

  /*! \brief Create and add a new device to container
   *
   * Use this version if device address currently not known.
   */
  template <typename T>
  std::shared_ptr<T> addDevice(const QString & alias)
  {
    mdtDeviceAddress deviceAddress;

    deviceAddress.setAlias(alias);

    return addDevice<T>(deviceAddress);
  }

  /*! \brief Get first device of type T and that has given alias
   */
  template <typename T>
  std::shared_ptr<T> device(const QString & alias)
  {
    for(auto & dev : pvDevices){
      Q_ASSERT(dev);
      if(dev->alias() == alias){
        return std::dynamic_pointer_cast<T>(dev);
      }
    }
    return std::shared_ptr<T>();
  }

  /*! \brief Get number of contained devices
   */
  inline int deviceCount() const
  {
    return pvDevices.size(); 
  }

  /*! \brief Remove all devices
   */
  void clear();

  /*! \brief Get a list of all devices
   *
   * Note: list is rebuilt at each call of this method.
   */
  QList<std::shared_ptr<mdtDevice>> allDevices();

  /*! \brief Disconnect frome devices
   */
  void disconnectFromDevices();

 signals:

  /*! \brief Emited when a new device was added
   */
  void deviceAdded(std::shared_ptr<mdtDevice> device);

  /*! \brief Emited when container was cleared
   */
  void cleared();

 private:

  Q_DISABLE_COPY(mdtDeviceContainer);

  std::vector<std::shared_ptr<mdtDevice> > pvDevices;
};


/*! \brief Global device container
 *
 * Hold a single instance of a mdtDeviceContainer.
 *
 * \sa mdtDeviceContainer
 */
class mdtGlobalDeviceContainer
{
 public:

  /*! \brief Access global container
   */
  std::shared_ptr<mdtDeviceContainer> container();

  /*! \brief Use global container
   *
   * Permit to use the global container the same way
   *  as mdtDeviceContainer
   */
  mdtDeviceContainer* operator->() const;

  /*! \brief 
   *
   * Release all devices shared pointers and the global container itself.
   *
   * It is recommanded to call this function
   *  (from one global container object) before the end of the programm.
   *  This is because internal container is a static member,
   *  and it can happen that it is destroyed to late,
   *  causing troubles when device destructors are called.
   */
  void clear();

 private:

  static std::shared_ptr<mdtDeviceContainer> pvContainer;
};


#endif // #ifndef MDT_DEVICE_CONTAINER_H
