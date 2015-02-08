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
#include "mdtDeviceContainerWidget.h"
#include "mdtDeviceContainer.h"
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
 * mdtDeviceContainerWidgetItem implementation
 */
namespace mdtDeviceContainerWidgetPrivate
{
  mdtDeviceContainerWidgetItem::mdtDeviceContainerWidgetItem(QObject* parent, shared_ptr<mdtDevice> _device)
  : QObject(parent), lbDeviceName(new QLabel), ldState(new mdtLed), lbDeviceStateText(new QLabel), lbMessage(new QLabel)
  {
    Q_ASSERT(_device);

    device = _device;
    lbDeviceName->setText(device->name());
    ldState->setFixedSize(15, 15);
    qDebug() << "mdtDeviceContainerWidgetItem::mdtDeviceContainerWidgetItem() - Functionnality not implemented yet ...";
    /**
     * \todo Adapt !
    Q_ASSERT(device->portManager() != 0);
    connect(device->portManager(), SIGNAL(stateChangedForUi(int,const QString&,int,bool)), this, SLOT(setState(int,const QString&,int,bool)));
    connect(device->portManager(), SIGNAL(statusMessageChanged(const QString&, const QString&,int)), this, SLOT(setMessage(const QString&, const QString&)));
    device->portManager()->notifyCurrentState();
    */
  }

  mdtDeviceContainerWidgetItem::~mdtDeviceContainerWidgetItem()
  {
    /**
     * \todo Adapt!
    if((device)&&(device->portManager() != 0)){
      disconnect(device->portManager(), SIGNAL(stateChangedForUi(int,const QString&,int,bool)), this, SLOT(setState(int,const QString&,int,bool)));
      disconnect(device->portManager(), SIGNAL(statusMessageChanged(const QString&, const QString&,int)), this, SLOT(setMessage(const QString&, const QString&)));
    }
    */
  }

  void mdtDeviceContainerWidgetItem::setState(int stateId, const QString & stateText, int ledColorId, bool ledIsOn)
  {
    ldState->setColor((mdtLed::color_t)ledColorId);
    ldState->setOn(ledIsOn);
    lbDeviceStateText->setText(stateText);
  }

  void mdtDeviceContainerWidgetItem::setMessage(const QString& message, const QString& details)
  {
    lbMessage->setText(message);
  }

}

/*
 * mdtDeviceContainerWidget implementation
 */

using namespace mdtDeviceContainerWidgetPrivate;

mdtDeviceContainerWidget::mdtDeviceContainerWidget(QWidget* parent)
 : QWidget(parent)
{
  QLabel *label;

  pvLayout = new QGridLayout;
  // Add header
  label = new QLabel(tr("Device name"));
  pvLayout->addWidget(label, 0, 0);
  label = new QLabel(tr("State"));
  pvLayout->addWidget(label, 0, 1, Qt::AlignHCenter);
  ///label = new QLabel(tr("State text"));
  ///pvLayout->addWidget(label, 0, 2);
  label = new QLabel(tr("Message"));
  pvLayout->addWidget(label, 0, 3);
  pvLayout->setColumnStretch(3, 1);
  pvCurrentLayoutRow = 1;
  setLayout(pvLayout);
}

void mdtDeviceContainerWidget::setContainer(shared_ptr< mdtDeviceContainer > c)
{
  Q_ASSERT(c);

  QList<shared_ptr<mdtDevice> > devices;
  int i;

  // Add devices currently existing in container
  clear();
  devices = c->allDevices();
  for(i = 0; i < devices.size(); ++i){
    Q_ASSERT(devices.at(i));
    addDevice(devices.at(i));
  }
  connect(c.get(), SIGNAL(deviceAdded(std::shared_ptr<mdtDevice>)), this, SLOT(addDevice(std::shared_ptr<mdtDevice>)));
  connect(c.get(), SIGNAL(cleared()), this, SLOT(clear()));
}

void mdtDeviceContainerWidget::clear()
{
  QMutableListIterator<shared_ptr<mdtDeviceContainerWidgetItem> > it(pvItems);

  while(it.hasNext()){
    removeItemWidgets(it.next());
    it.remove();
  }
}

void mdtDeviceContainerWidget::addDevice(shared_ptr<mdtDevice> device)
{
  Q_ASSERT(device);
  ///Q_ASSERT(device->portManager() != 0);

  shared_ptr<mdtDeviceContainerWidgetItem> item(new mdtDeviceContainerWidgetItem(this, device));

  Q_ASSERT(item->lbDeviceName);
  Q_ASSERT(item->ldState);
  pvLayout->addWidget(item->lbDeviceName.get(), pvCurrentLayoutRow, 0);
  pvLayout->addWidget(item->ldState.get(), pvCurrentLayoutRow, 1, Qt::AlignHCenter);
  pvLayout->addWidget(item->lbDeviceStateText.get(), pvCurrentLayoutRow, 2);
  pvLayout->addWidget(item->lbMessage.get(), pvCurrentLayoutRow, 3);
  pvItems.append(item);
  ++pvCurrentLayoutRow;
}

void mdtDeviceContainerWidget::removeItemWidgets(shared_ptr<mdtDeviceContainerWidgetItem> item)
{
  Q_ASSERT(item);

  pvLayout->removeWidget(item->lbDeviceName.get());
  pvLayout->removeWidget(item->ldState.get());
  pvLayout->removeWidget(item->lbDeviceStateText.get());
  pvLayout->removeWidget(item->lbMessage.get());
  --pvCurrentLayoutRow;
  Q_ASSERT(pvCurrentLayoutRow > 0); // Row 0 is the header
}
