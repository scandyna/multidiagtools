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
#ifndef MDT_DEVICE_CONTAINER_WIDGET_H
#define MDT_DEVICE_CONTAINER_WIDGET_H

#include <QWidget>
#include <QObject>
#include <QList>
#include <memory>

class QGridLayout;
class mdtDevice;
class QLabel;
class mdtLed;
class mdtDeviceContainer;

namespace mdtDeviceContainerWidgetPrivate
{
  /*! \internal Container for mdtTtTestNodeManagerWidget
   */
  class mdtDeviceContainerWidgetItem : public QObject
  {
    Q_OBJECT

  public:

    /*! \internal Constructor
     *
     * Will create widgets (without parent)
     */
    mdtDeviceContainerWidgetItem(QObject* parent, std::shared_ptr<mdtDevice> _device);

    std::shared_ptr<mdtDevice> device;
    std::shared_ptr<QLabel> lbDeviceName;
    std::shared_ptr<mdtLed> ldState;
    std::shared_ptr<QLabel> lbDeviceStateText;
    std::shared_ptr<QLabel> lbMessage;

  public slots:

    void setState(int stateId, const QString & stateText, int ledColorId, bool ledIsOn);
    void setMessage(const QString & message, const QString & details);

  private:

    Q_DISABLE_COPY(mdtDeviceContainerWidgetItem);
  };

}

/*! \brief Manage device status widget items
 */
class mdtDeviceContainerWidget : public QWidget
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtDeviceContainerWidget(QWidget* parent = 0);

  /*! \brief Set device container
   *
   * Will clear widgets, then add all devices allready set in c.
   *  Then, signals from c are connected,
   *  so this widget will be updated each time a device
   *  is added/removed from container c.
   */
  void setContainer(std::shared_ptr<mdtDeviceContainer> c);

 public slots:

  /*! \brief Add a device
   */
  void addDevice(std::shared_ptr<mdtDevice> device);

  /*! \brief Clear
   */
  void clear();

 private:

  Q_DISABLE_COPY(mdtDeviceContainerWidget);

  /*! \brief Remove a widget item
   *
   * Will anly remove widgets from layout, pvItems is not changed.
   */
  void removeItemWidgets(std::shared_ptr<mdtDeviceContainerWidgetPrivate::mdtDeviceContainerWidgetItem> item);

  QGridLayout *pvLayout;
  int pvCurrentLayoutRow;
  QList<std::shared_ptr<mdtDeviceContainerWidgetPrivate::mdtDeviceContainerWidgetItem> > pvItems;
};

#endif // #ifndef MDT_DEVICE_CONTAINER_WIDGET_H
