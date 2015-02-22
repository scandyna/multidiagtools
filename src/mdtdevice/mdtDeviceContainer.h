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
 */
class mdtDeviceContainer : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Construct a empty container
   */
  mdtDeviceContainer(QObject *parent = 0);

  /*! \brief Create and add a new device
   */
  template <typename T> std::shared_ptr<T> addDevice(const QVariant & identification, const QString & name)
  {
    std::shared_ptr<T> dev(new T);
    ///dev->setIdentification(identification);
    ///dev->setName(name);
    pvDevices.push_back(dev);
    emit deviceAdded(dev);
    return dev;
  }

  /*! \brief Get device of type T and that has given identification
   */
  template <typename T> std::shared_ptr<T> device(const QVariant & identification)
  {
    std::vector<std::shared_ptr<mdtDevice>>::const_iterator it;
    std::shared_ptr<T> dev;

    /**
    for(it = pvDevices.begin(); it != pvDevices.end(); ++it){
      Q_ASSERT(*it);
      if((*it)->identification() == identification){
        dev = std::dynamic_pointer_cast<T>(*it);
        return dev;
      }
    }
    */

    return std::shared_ptr<T>();
  }

  /*! \brief Get number of contained devices
   */
  inline int deviceCount() const { return pvDevices.size(); }

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

  std::vector<std::shared_ptr<mdtDevice>> pvDevices;
};

#endif // #ifndef MDT_DEVICE_CONTAINER_H
