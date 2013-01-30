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

  pvTxThread = 0;
  pvRxThread = 0;
  lbTx = 0;
  lbRx = 0;
  ldTx = 0;
  ldRx = 0;
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
  setLayout(layout);
  // Set some default attributes
  setDefaultStateTexts();
  setDefaultStateColors();
}

mdtDeviceStatusWidget::~mdtDeviceStatusWidget()
{
  disableTxRxLeds();
}

void mdtDeviceStatusWidget::setDevice(mdtDevice *device)
{
  Q_ASSERT(device != 0);

  connect(device, SIGNAL(stateChanged(int)), this, SLOT(setState(int)));
  setState(device->state());
}

void mdtDeviceStatusWidget::enableTxRxLeds(mdtPortThread *txThread, mdtPortThread *rxThread)
{
  Q_ASSERT(layout() != 0);
  Q_ASSERT(txThread != 0);
  Q_ASSERT(rxThread != 0);

  QGridLayout *gridLayout;

  gridLayout = dynamic_cast<QGridLayout*>(layout());
  Q_ASSERT(gridLayout != 0);

  // Remove LEDs (if exists)
  disableTxRxLeds();
  // Create LEDs
  ldTx = new mdtBlinkLed;
  ldTx->setFixedSize(12, 12);
  ldTx->setGreen();
  ldRx = new mdtBlinkLed;
  ldRx->setFixedSize(12, 12);
  ldRx->setGreen();
  // Create labels
  lbTx = new QLabel("Tx");
  lbRx = new QLabel("Rx");
  // Layout
  gridLayout->addWidget(lbTx, 0, 3);
  gridLayout->addWidget(ldTx, 0, 4);
  gridLayout->addWidget(lbRx, 0, 5);
  gridLayout->addWidget(ldRx, 0, 6);
  // Make connections
  pvTxThread = txThread;
  pvRxThread = rxThread;
  connect(pvTxThread, SIGNAL(writeProcessBegin()), this, SLOT(trigTxLed()));
  connect(pvRxThread, SIGNAL(readProcessBegin()), this, SLOT(trigRxLed()));
}

void mdtDeviceStatusWidget::disableTxRxLeds()
{
  if(ldTx != 0){
    Q_ASSERT(pvTxThread != 0);
    disconnect(pvTxThread, SIGNAL(writeProcessBegin()), this, SLOT(trigTxLed()));
    layout()->removeWidget(ldTx);
    delete ldTx;
    ldTx = 0;
  }
  if(lbTx != 0){
    layout()->removeWidget(lbTx);
    delete lbTx;
    lbTx = 0;
  }
  if(ldRx != 0){
    Q_ASSERT(pvRxThread != 0);
    disconnect(pvRxThread, SIGNAL(readProcessBegin()), this, SLOT(trigRxLed()));
    layout()->removeWidget(ldRx);
    delete ldRx;
    ldRx = 0;
  }
  if(lbRx != 0){
    layout()->removeWidget(lbRx);
    delete lbRx;
    lbRx = 0;
  }
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

void mdtDeviceStatusWidget::trigTxLed()
{
  Q_ASSERT(ldTx != 0);

  ldTx->setOn(100);
}

void mdtDeviceStatusWidget::trigRxLed()
{
  Q_ASSERT(ldRx != 0);

  ldRx->setOn(100);
}
