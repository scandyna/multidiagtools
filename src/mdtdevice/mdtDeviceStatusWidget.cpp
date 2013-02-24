/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include <QTimer>
#include <QMessageBox>

#include <QDebug>

mdtDeviceStatusWidget::mdtDeviceStatusWidget(QWidget *parent)
 : QWidget(parent)
{
  pvLayout = new QGridLayout;

  pvCustomWidget = 0;
  pvTxThread = 0;
  pvRxThread = 0;
  lbTx = 0;
  lbRx = 0;
  ldTx = 0;
  ldRx = 0;
  pvShowingMessage = false;
  // Add GUI elements
  pbDetails = new QPushButton("...");
  pbDetails->setEnabled(false);
  mbDetails = new QMessageBox(this);
  mbDetails->setStandardButtons(QMessageBox::Ok);
  mbDetails->setIcon(QMessageBox::Warning);
  pvLayout->addWidget(pbDetails, 0, 0);
  ldState = new mdtBlinkLed;
  ldState->setFixedSize(15, 15);
  pvLayout->addWidget(ldState, 0, 1);
  lbMessage = new QLabel;
  lbMessage->setMinimumWidth(150);
  pvLayout->addWidget(lbMessage, 0, 2);
  setLayout(pvLayout);
  // Set some default attributes
  setDefaultStateTexts();
  setDefaultStateColors();
  // Setup back to state text timer
  pvBackToStateTextTimer = new QTimer(this);
  pvBackToStateTextTimer->setSingleShot(true);
  connect(pvBackToStateTextTimer, SIGNAL(timeout()), this, SLOT(backToStateText()));
  connect(pbDetails, SIGNAL(clicked()), mbDetails, SLOT(exec()));
}

mdtDeviceStatusWidget::~mdtDeviceStatusWidget()
{
  disableTxRxLeds();
}

void mdtDeviceStatusWidget::enableTxRxLeds(mdtPortThread *txThread, mdtPortThread *rxThread)
{
  Q_ASSERT(pvLayout != 0);
  Q_ASSERT(txThread != 0);
  Q_ASSERT(rxThread != 0);

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
  pvLayout->addWidget(lbTx, 0, 3);
  pvLayout->addWidget(ldTx, 0, 4);
  pvLayout->addWidget(lbRx, 0, 5);
  pvLayout->addWidget(ldRx, 0, 6);
  // Make connections
  pvTxThread = txThread;
  pvRxThread = rxThread;
  connect(pvTxThread, SIGNAL(writeProcessBegin()), this, SLOT(trigTxLed()));
  connect(pvRxThread, SIGNAL(readProcessBegin()), this, SLOT(trigRxLed()));
}

void mdtDeviceStatusWidget::disableTxRxLeds()
{
  Q_ASSERT(layout() != 0);

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

void mdtDeviceStatusWidget::addCustomWidget(QWidget *widget)
{
  Q_ASSERT(pvLayout != 0);
  Q_ASSERT(widget != 0);

  removeCustomWidget();
  pvCustomWidget = widget;
  pvLayout->addWidget(pvCustomWidget, 0, pvLayout->columnCount());
}

void mdtDeviceStatusWidget::removeCustomWidget()
{
  Q_ASSERT(pvLayout != 0);

  if(pvCustomWidget != 0){
    pvLayout->removeWidget(pvCustomWidget);
    pvCustomWidget = 0;
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
    pvCurrentStateText = pvReadyText;
  }else if(state == mdtDevice::Disconnected){
    ldState->setGreen();
    ldState->setOff();
    pvCurrentStateText = pvDisconnectedText;
  }else if(state == mdtDevice::Connecting){
    ldState->setColor(pvConnectingColor);
    ldState->setOn();
    pvCurrentStateText = pvConnectingText;
  }else if(state == mdtDevice::Busy){
    ldState->setColor(pvBusyColor);
    ldState->setOn();
    pvCurrentStateText = pvBusyText;
  }else{
    ldState->setRed();
    ldState->setOn();
    pvCurrentStateText = tr("Unknown state");
  }
  // Set state text
  if(!pvShowingMessage){
    lbMessage->setText(pvCurrentStateText);
  }
}

void mdtDeviceStatusWidget::showMessage(const QString &message, const QString &details, int timeout)
{
  pvShowingMessage = true;
  lbMessage->setText(message);
  if(!details.isEmpty()){
    mbDetails->setText(message);
    mbDetails->setInformativeText(details);
    pbDetails->setEnabled(true);
    /// \todo Implement details !
  }else{
    mbDetails->setText("");
    mbDetails->setInformativeText("");
    pbDetails->setEnabled(false);
  }
  if(timeout > 0){
    pvBackToStateTextTimer->start(timeout);
  }
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

void mdtDeviceStatusWidget::backToStateText()
{
  pvShowingMessage = false;
  lbMessage->setText(pvCurrentStateText);
  pbDetails->setEnabled(false);
}
