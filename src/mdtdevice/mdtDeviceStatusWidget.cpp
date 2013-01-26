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
  // Set some default attributes
  setDefaultStateTexts();
  setDefaultStateColors();
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

void mdtDeviceStatusWidget::setDefaultStateTexts()
{
  pvReadyText = tr("Ready");
  pvDisconnectedText = tr("Disconnected");
  pvConnectingText = tr("Connecting ...");
  pvBusyText = tr("Busy");
}

void mdtDeviceStatusWidget::setStateReadyText(const QString &text)
{
  pvReadyText = text;
}

void mdtDeviceStatusWidget::setStateDisconnectedText(const QString &text)
{
  pvDisconnectedText = text;
}

void mdtDeviceStatusWidget::setStateConnectingText(const QString &text)
{
  pvConnectingText = text;
}

void mdtDeviceStatusWidget::setStateBusyText(const QString &text)
{
  pvBusyText = text;
}

void mdtDeviceStatusWidget::setDefaultStateColors()
{
  pvReadyColor = mdtLed::LED_COLOR_GREEN;
  pvConnectingColor = mdtLed::LED_COLOR_ORANGE;
  pvBusyColor = mdtLed::LED_COLOR_ORANGE;
}

void mdtDeviceStatusWidget::setStateReadyColor(mdtLed::color_t color)
{
  pvReadyColor = color;
}

void mdtDeviceStatusWidget::setStateConnectingColor(mdtLed::color_t color)
{
  pvConnectingColor = color;
}

void mdtDeviceStatusWidget::setStateBusyColor(mdtLed::color_t color)
{
  pvBusyColor = color;
}

void mdtDeviceStatusWidget::setState(int state)
{
  if(state == mdtDevice::Ready){
    ldState->setColor(pvReadyColor);
    ldState->setOn();
    lbMessage->setText(pvReadyText);
  }else if(state == mdtDevice::Disconnected){
    ldState->setGreen();
    ldState->setOff();
    lbMessage->setText(pvDisconnectedText);
  }else if(state == mdtDevice::Connecting){
    ldState->setColor(pvConnectingColor);
    ldState->setOn();
    lbMessage->setText(pvConnectingText);
  }else if(state == mdtDevice::Busy){
    ldState->setColor(pvBusyColor);
    ldState->setOn();
    lbMessage->setText(pvBusyText);
  }else{
    ldState->setRed();
    ldState->setOn();
    lbMessage->setText(tr("Unknown state"));
  }
}

void mdtDeviceStatusWidget::setState(int state, const QString &message, const QString &details)
{
}

