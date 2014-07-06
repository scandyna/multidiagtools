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
#ifndef MDT_TT_TEST_NODE_MANAGER_WIDGET_H
#define MDT_TT_TEST_NODE_MANAGER_WIDGET_H

#include <QWidget>
#include <QObject>
#include <QList>
#include <memory>

class QGridLayout;
class mdtDevice;
class QLabel;
class mdtLed;
class mdtTtTestNodeManager;

namespace mdtTtTestNodeManagerWidgetPrivate
{
  /*! \internal Container for mdtTtTestNodeManagerWidget
   */
  class mdtTtTestNodeManagerWidgetItem : public QObject
  {
    Q_OBJECT

  public:

    /*! \internal Constructor
     *
     * Will create widgets (without parent)
     */
    mdtTtTestNodeManagerWidgetItem(QObject* parent, std::shared_ptr<mdtDevice> _device);

    std::shared_ptr<mdtDevice> device;
    std::shared_ptr<QLabel> lbDeviceName;
    std::shared_ptr<mdtLed> ldState;
    std::shared_ptr<QLabel> lbDeviceStateText;

  public slots:

    void setState(int stateId, const QString & stateText, int ledColorId, bool ledIsOn);

  private:

    Q_DISABLE_COPY(mdtTtTestNodeManagerWidgetItem);
  };

}

/*! \brief Manage device status widget items
 */
class mdtTtTestNodeManagerWidget : public QWidget
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeManagerWidget(QWidget* parent = 0);

  /*! \brief Set test node manager
   *
   * Will clear widget and add all devices allready set in m.
   *  Then, signals from m are connected,
   *  so this widget will be updated each time a device
   *  is added/removed from manager m.
   */
  void setTestNodeManager(std::shared_ptr<mdtTtTestNodeManager> m);

 public slots:

  /*! \brief Add a device
   */
  void addDevice(std::shared_ptr<mdtDevice> device);

  /*! \brief Clear
   */
  void clear();

  /*! \brief Remove a device
   */
  ///void removeDevice(mdtDevice* device);

 private:

  Q_DISABLE_COPY(mdtTtTestNodeManagerWidget);

  /*! \brief Remove a widget item
   *
   * Will anly remove widgets from layout, pvItems is not changed.
   */
  void removeItemWidgets(std::shared_ptr<mdtTtTestNodeManagerWidgetPrivate::mdtTtTestNodeManagerWidgetItem> item);

  QGridLayout *pvLayout;
  int pvCurrentLayoutRow;
  QList<std::shared_ptr<mdtTtTestNodeManagerWidgetPrivate::mdtTtTestNodeManagerWidgetItem> > pvItems;
};

#endif // #ifndef MDT_TT_TEST_NODE_MANAGER_WIDGET_H
