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
#include "mdtSerialPortCtlWidget.h"
#include "mdtAbstractSerialPort.h"
#include <QGridLayout>

#include <QDebug>

mdtSerialPortCtlWidget::mdtSerialPortCtlWidget(QWidget *parent)
 : QWidget(parent)
{
  QGridLayout *l;

  // Create labels
  lbCar = new QLabel;
  lbRng = new QLabel;
  lbDsr = new QLabel;
  lbCts = new QLabel;
  lbDtr = new QLabel;
  lbRts = new QLabel;
  lbTx = new QLabel;
  lbRx = new QLabel;
  // Set alignement center
  lbCar->setAlignment(Qt::AlignHCenter);
  lbRng->setAlignment(Qt::AlignHCenter);
  lbDsr->setAlignment(Qt::AlignHCenter);
  lbCts->setAlignment(Qt::AlignHCenter);
  lbDtr->setAlignment(Qt::AlignHCenter);
  lbRts->setAlignment(Qt::AlignHCenter);
  lbTx->setAlignment(Qt::AlignHCenter);
  lbRx->setAlignment(Qt::AlignHCenter);
  // Set text
  lbCar->setText("CAR");
  lbRng->setText("RNG");
  lbDtr->setText("DTR");
  lbDsr->setText("DSR");
  lbRts->setText("RTS");
  lbCts->setText("CTS");
  lbTx->setText("TX");
  lbRx->setText("RX");
  // Create buttons
  pbDtr = new QPushButton;
  pbRts = new QPushButton;
  pbDtr->setCheckable(true);
  pbRts->setCheckable(true);
  // Create LEDs
  ldCar = new mdtLed;
  ldRng = new mdtLed;
  ldDsr = new mdtLed;
  ldCts = new mdtLed;
  ldTx = new mdtBlinkLed;
  ldRx = new mdtBlinkLed;
  // Fix LEDs size
  ldCar->setFixedSize(15, 15);
  ldRng->setFixedSize(15, 15);
  ldDsr->setFixedSize(15, 15);
  ldCts->setFixedSize(15, 15);
  ldTx->setFixedSize(15, 15);
  ldRx->setFixedSize(15, 15);
  // Layout
  l = new QGridLayout;
  // Add labels
  l->addWidget(lbCar, 0, 0);
  l->addWidget(lbRng, 0, 1);
  l->addWidget(lbDtr, 0, 2);
  l->addWidget(lbDsr, 0, 3);
  l->addWidget(lbRts, 0, 4);
  l->addWidget(lbCts, 0, 5);
  l->addWidget(lbTx, 0, 6);
  l->addWidget(lbRx, 0, 7);
  // Add LEDs
  l->addWidget(ldCar, 1, 0);
  l->addWidget(ldRng, 1, 1);
  l->addWidget(ldDsr, 1, 3);
  l->addWidget(ldCts, 1, 5);
  l->addWidget(ldTx, 1, 6);
  l->addWidget(ldRx, 1, 7);
  // Add buttons
  l->addWidget(pbDtr, 1, 2);
  l->addWidget(pbRts, 1, 4);

  setLayout(l);
  
  // Connect buttons to text update slots
  connect(pbDtr, SIGNAL(toggled(bool)), this, SLOT(setDtrText(bool)));
  connect(pbRts, SIGNAL(toggled(bool)), this, SLOT(setRtsText(bool)));
  // Set initial texts
  pbDtr->setText(" -- ");
  pbRts->setText(" -- ");
}

void mdtSerialPortCtlWidget::makeConnections(mdtSerialPortManager *manager)
{
  Q_ASSERT(manager != 0);

  mdtAbstractSerialPort *port = &manager->port();
  Q_ASSERT(port != 0);

  // Modem lines
  connect(port, SIGNAL(carChanged(bool)), ldCar, SLOT(setOn(bool)));
  connect(port, SIGNAL(rngChanged(bool)), ldRng, SLOT(setOn(bool)));
  connect(port, SIGNAL(dsrChanged(bool)), ldDsr, SLOT(setOn(bool)));
  connect(port, SIGNAL(ctsChanged(bool)), ldCts, SLOT(setOn(bool)));
  connect(pbDtr, SIGNAL(toggled(bool)), port, SLOT(setDtr(bool)));
  connect(pbRts, SIGNAL(toggled(bool)), port, SLOT(setRts(bool)));
  // TX/RX
  /// \todo Create threads somwere here, and connect signals/slots
  connect(manager->writeThread(), SIGNAL(ioProcessBegin()), this, SLOT(trigTxState()));
  ///connect(port, SIGNAL(writeTimeoutStateChanged(bool)), this, SLOT(updateTxTimeoutState(bool)));
  connect(manager->readThread(), SIGNAL(ioProcessBegin()), this, SLOT(trigRxState()));
  ///connect(port, SIGNAL(readTimeoutStateChanged(bool)), this, SLOT(updateRxTimeoutState(bool)));
  // IO error
  ///connect(manager->writeThread(), SIGNAL(errorOccured(int)), this, SLOT(trigTxErrorState()));
  ///connect(manager->readThread(), SIGNAL(errorOccured(int)), this, SLOT(trigRxErrorState()));
}

void mdtSerialPortCtlWidget::trigTxState()
{
  // Good TX state is not priority
  if(ldTx->isOn()){
    return;
  }
  ldTx->setGreen();
  ldTx->setOn(100);
}

void mdtSerialPortCtlWidget::trigTxErrorState()
{
  ldTx->setRed();
  ldTx->setOn(1000, true);
}

void mdtSerialPortCtlWidget::updateTxTimeoutState(bool state)
{
  if(state){
    ldTx->setOrange();
    ldTx->setOn();
  }else{
    ldTx->setGreen();
    ldTx->setOff();
  }
}

void mdtSerialPortCtlWidget::trigRxState()
{
  // Good RX state is not priority
  if(ldRx->isOn()){
    return;
  }
  ldRx->setGreen();
  ldRx->setOn(100);
}

void mdtSerialPortCtlWidget::trigRxErrorState()
{
  ldRx->setRed();
  ldRx->setOn(1000, true);
}

void mdtSerialPortCtlWidget::updateRxTimeoutState(bool state)
{
  if(state){
    ldRx->setOrange();
    ldRx->setOn();
  }else{
    ldRx->setGreen();
    ldRx->setOff();
  }
}

void mdtSerialPortCtlWidget::setDtrText(bool on)
{
  if(on){
    pbDtr->setText("ON ");
  }else{
    pbDtr->setText("OFF");
  }
}

void mdtSerialPortCtlWidget::setRtsText(bool on)
{
  if(on){
    pbRts->setText("ON ");
  }else{
    pbRts->setText("OFF");
  }
}
