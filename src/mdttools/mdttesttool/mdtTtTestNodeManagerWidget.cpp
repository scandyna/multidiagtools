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
#include "mdtTtTestNodeManagerWidget.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDevice.h"
#include "mdtPortManager.h"
#include "mdtLed.h"
#include <QLabel>
#include <QGridLayout>
#include <QMutableListIterator>

#include <QDebug>

class mdtPortManager;

using namespace std;

/*
 * mdtTtTestNodeManagerWidgetItem implementation
 */
namespace mdtTtTestNodeManagerWidgetPrivate
{
  mdtTtTestNodeManagerWidgetItem::mdtTtTestNodeManagerWidgetItem(QObject* parent, shared_ptr<mdtDevice> _device)
  : QObject(parent), lbDeviceName(new QLabel), ldState(new mdtLed), lbDeviceStateText(new QLabel)
  {
    Q_ASSERT(_device);

    device = _device;
    lbDeviceName->setText(device->name());
    ldState->setFixedSize(15, 15);
  }

  void mdtTtTestNodeManagerWidgetItem::setState(int stateId, const QString & stateText, int ledColorId, bool ledIsOn)
  {
    ldState->setColor((mdtLed::color_t)ledColorId);
    ldState->setOn(ledIsOn);
    lbDeviceStateText->setText(stateText);
  }
}

/*
 * mdtTtTestNodeManagerWidget implementation
 */

using namespace mdtTtTestNodeManagerWidgetPrivate;

mdtTtTestNodeManagerWidget::mdtTtTestNodeManagerWidget(QWidget* parent)
 : QWidget(parent)
{
  QLabel *label;

  pvLayout = new QGridLayout;
  // Add header
  label = new QLabel(tr("Device name"));
  pvLayout->addWidget(label, 0, 0);
  label = new QLabel(tr("State"));
  pvLayout->addWidget(label, 0, 1, Qt::AlignHCenter);
  
  pvCurrentLayoutRow = 1;
  setLayout(pvLayout);
}

void mdtTtTestNodeManagerWidget::setTestNodeManager(shared_ptr< mdtTtTestNodeManager > m)
{
  Q_ASSERT(m);

  QList<shared_ptr<mdtDevice> > devices;
  int i;

  // Add devices currently existing in manager
  clear();
  devices = m->allDevices();
  for(i = 0; i < devices.size(); ++i){
    Q_ASSERT(devices.at(i));
    addDevice(devices.at(i));
  }
  connect(m.get(), SIGNAL(deviceAdded(std::shared_ptr<mdtDevice>)), this, SLOT(addDevice(std::shared_ptr<mdtDevice>)));
  connect(m.get(), SIGNAL(cleared()), this, SLOT(clear()));
  ///connect(m.get(), SIGNAL(deviceAdded(mdtDevice*)), this, SLOT(addDevice(mdtDevice*)));
  ///connect(m.get(), SIGNAL(deviceRemoved(mdtDevice*)), this, SLOT(removeDevice(mdtDevice*)));
}

void mdtTtTestNodeManagerWidget::clear()
{
  QMutableListIterator<shared_ptr<mdtTtTestNodeManagerWidgetItem> > it(pvItems);

  while(it.hasNext()){
    removeItemWidgets(it.next());
    it.remove();
  }
}

void mdtTtTestNodeManagerWidget::addDevice(shared_ptr<mdtDevice> device)
{
  ///Q_ASSERT(device != 0);
  Q_ASSERT(device);
  Q_ASSERT(device->portManager() != 0);

  qDebug() << "Add device: " << device->name();
  shared_ptr<mdtTtTestNodeManagerWidgetItem> item(new mdtTtTestNodeManagerWidgetItem(this, device));
  ///shared_ptr<mdtTtTestNodeManagerWidgetItem> item(new mdtTtTestNodeManagerWidgetItem(this, make_shared<mdtDevice>(device)));
  ///shared_ptr<mdtTtTestNodeManagerWidgetItem> item(new mdtTtTestNodeManagerWidgetItem(this, shared_ptr<mdtDevice>(device)));

  Q_ASSERT(item->lbDeviceName);
  Q_ASSERT(item->ldState);
  pvLayout->addWidget(item->lbDeviceName.get(), pvCurrentLayoutRow, 0);
  pvLayout->addWidget(item->ldState.get(), pvCurrentLayoutRow, 1, Qt::AlignHCenter);
  pvLayout->addWidget(item->lbDeviceStateText.get(), pvCurrentLayoutRow, 2);
  connect(device->portManager(), SIGNAL(stateChangedForUi(int,const QString&,int,bool)), item.get(), SLOT(setState(int,const QString&,int,bool)));
  device->portManager()->notifyCurrentState();
  pvItems.append(item);
  ++pvCurrentLayoutRow;
}

/**
void mdtTtTestNodeManagerWidget::removeDevice(mdtDevice* device)
{
  Q_ASSERT(device != 0);

  int i;
  shared_ptr<mdtTtTestNodeManagerWidgetItem> item;

  if(!device){
    return;
  }
  // Search item related to given device
  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item);
    Q_ASSERT(item->device);
    if(item->device.get() == device){
      removeItemWidgets(item);
      pvItems.removeAt(i);
      return;
    }
  }
}
*/

void mdtTtTestNodeManagerWidget::removeItemWidgets(shared_ptr<mdtTtTestNodeManagerWidgetItem> item)
{
  Q_ASSERT(item);

  pvLayout->removeWidget(item->lbDeviceName.get());
  pvLayout->removeWidget(item->ldState.get());
  --pvCurrentLayoutRow;
  Q_ASSERT(pvCurrentLayoutRow > 0); // Row 0 is the header
}
