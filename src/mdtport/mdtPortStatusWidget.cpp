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
#include "mdtPortStatusWidget.h"
#include "mdtPortManager.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>

mdtPortStatusWidget::mdtPortStatusWidget(QWidget *parent)
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
  pvLayout->setColumnStretch(3, 2);
  lbPermanentTextLabel = new QLabel;
  pvLayout->addWidget(lbPermanentTextLabel, 1, 0, 1, -1);
  setLayout(pvLayout);
  // Setup back to state text timer
  pvBackToStateTextTimer = new QTimer(this);
  pvBackToStateTextTimer->setSingleShot(true);
  connect(pvBackToStateTextTimer, SIGNAL(timeout()), this, SLOT(backToStateText()));
  connect(pbDetails, SIGNAL(clicked()), mbDetails, SLOT(exec()));
}

mdtPortStatusWidget::~mdtPortStatusWidget()
{
  disableTxRxLeds();
}

void mdtPortStatusWidget::enableTxRxLeds(mdtPortThread *txThread, mdtPortThread *rxThread)
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

void mdtPortStatusWidget::disableTxRxLeds()
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

void mdtPortStatusWidget::addCustomWidget(QWidget *widget)
{
  Q_ASSERT(pvLayout != 0);
  Q_ASSERT(widget != 0);

  removeCustomWidget();
  pvCustomWidget = widget;
  pvLayout->addWidget(pvCustomWidget, 0, pvLayout->columnCount());
}

void mdtPortStatusWidget::removeCustomWidget()
{
  Q_ASSERT(pvLayout != 0);

  if(pvCustomWidget != 0){
    pvLayout->removeWidget(pvCustomWidget);
    pvCustomWidget = 0;
  }
}

void mdtPortStatusWidget::setPermanentText(const QString &text)
{
  lbPermanentTextLabel->setText(text);
}

void mdtPortStatusWidget::clearMessage()
{
  pvBackToStateTextTimer->stop();
  pvShowingMessage = false;
  lbMessage->clear();
  mbDetails->setText(QString());
  mbDetails->setInformativeText("");
  pbDetails->setEnabled(false);
}

void mdtPortStatusWidget::setState(int id, const QString & text, int ledColorId, bool ledIsOn)
{
  // Set LED
  ldState->setColor((mdtLed::color_t)ledColorId);
  ldState->setOn(ledIsOn);
  // Set state text
  pvCurrentStateText = text;
  if(!pvShowingMessage){
    lbMessage->setText(pvCurrentStateText);
  }
}

void mdtPortStatusWidget::showMessage(const QString &message, const QString &details, int timeout)
{
  if(message.isEmpty()){
    clearMessage();
    return;
  }
  pvBackToStateTextTimer->stop();
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

void mdtPortStatusWidget::trigTxLed()
{
  Q_ASSERT(ldTx != 0);

  ldTx->setOn(100);
}

void mdtPortStatusWidget::trigRxLed()
{
  Q_ASSERT(ldRx != 0);

  ldRx->setOn(100);
}

void mdtPortStatusWidget::backToStateText()
{
  pvShowingMessage = false;
  lbMessage->setText(pvCurrentStateText);
  pbDetails->setEnabled(false);
}
