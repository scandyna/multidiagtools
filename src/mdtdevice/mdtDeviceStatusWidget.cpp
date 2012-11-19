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
#include "mdtDeviceStatusWidget.h"
#include "mdtBlinkLed.h"
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

#include <QDebug>

mdtDeviceStatusWidget::mdtDeviceStatusWidget(QWidget *parent)
 : QWidget(parent)
{
  QGridLayout *layout = new QGridLayout;

  // Add GUI elements
  pbDetails = new QPushButton("...");
  pbDetails->setEnabled(false);
  layout->addWidget(pbDetails, 0, 0);
  ldState = new mdtBlinkLed;
  ldState->setFixedSize(15, 15);
  layout->addWidget(ldState, 0, 1);
  lbMessage = new QLabel;
  lbMessage->setMinimumWidth(150);
  layout->addWidget(lbMessage, 0, 2);
  lbTx = new QLabel("Tx");
  layout->addWidget(lbTx, 0, 3);
  ldTx = new mdtBlinkLed;
  ldTx->setFixedSize(12, 12);
  ldTx->setGreen();
  layout->addWidget(ldTx, 0, 4);
  lbRx = new QLabel("Rx");
  layout->addWidget(lbRx, 0, 5);
  ldRx = new mdtBlinkLed;
  ldRx->setFixedSize(12, 12);
  ldRx->setGreen();
  layout->addWidget(ldRx, 0, 6);
  setLayout(layout);

}

mdtDeviceStatusWidget::~mdtDeviceStatusWidget()
{
}

void mdtDeviceStatusWidget::setDevice(mdtDevice *device)
{
  Q_ASSERT(device != 0);

  connect(device, SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
  setState(device->state());
}

void mdtDeviceStatusWidget::setState(int state)
{
  if(state == mdtDevice::Ready){
    ldState->setGreen();
    ldState->setOn();
    lbMessage->setText(tr("Ready"));
  }else if(state == mdtDevice::Disconnected){
    ldState->setGreen();
    ldState->setOff();
    lbMessage->setText(tr("Disconnected"));
  }else if(state == mdtDevice::Busy){
    ldState->setOrange();
    ldState->setOn();
    lbMessage->setText(tr("Busy"));
  }else{
    ldState->setRed();
    ldState->setOn();
    lbMessage->setText(tr("Unknown state"));
  }
}

void mdtDeviceStatusWidget::setState(int state, const QString &message, const QString &details)
{
}

