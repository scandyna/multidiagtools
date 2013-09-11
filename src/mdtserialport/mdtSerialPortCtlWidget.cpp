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
  // Set alignement center
  lbCar->setAlignment(Qt::AlignCenter);
  lbRng->setAlignment(Qt::AlignCenter);
  lbDsr->setAlignment(Qt::AlignCenter);
  lbCts->setAlignment(Qt::AlignCenter);
  lbDtr->setAlignment(Qt::AlignCenter);
  lbRts->setAlignment(Qt::AlignCenter);
  // Set text
  lbCar->setText(" CAR");
  lbRng->setText(" RNG");
  lbDtr->setText("  DTR");
  lbDsr->setText(" DSR");
  lbRts->setText("  RTS");
  lbCts->setText(" CTS");
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
  // Fix LEDs size
  ldCar->setFixedSize(12, 12);
  ldRng->setFixedSize(12, 12);
  ldDsr->setFixedSize(12, 12);
  ldCts->setFixedSize(12, 12);
  // Layout
  l = new QGridLayout;
  // Add labels
  l->addWidget(lbCar, 0, 0);
  l->addWidget(lbRng, 0, 2);
  l->addWidget(lbDtr, 0, 4);
  l->addWidget(lbDsr, 0, 6);
  l->addWidget(lbRts, 0, 8);
  l->addWidget(lbCts, 0, 10);
  // Add LEDs
  l->addWidget(ldCar, 0, 1);
  l->addWidget(ldRng, 0, 3);
  l->addWidget(ldDsr, 0, 7);
  l->addWidget(ldCts, 0, 11);
  // Add buttons
  l->addWidget(pbDtr, 0, 5);
  l->addWidget(pbRts, 0, 9);

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
