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
#include "mdtDeviceU3606AWidget.h"

#include <QDebug>

/*! \todo A supprimer !(?)!
 */
mdtDeviceU3606AWidget::mdtDeviceU3606AWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  
  connect(pbEssais, SIGNAL(clicked()), this, SLOT(essais()));
  connect(pbScan, SIGNAL(clicked()), this, SLOT(scan()));
  connect(cbPort, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectPort(const QString&)));
  connect(&pvDevice, SIGNAL(idnChanged(const QStringList&)), this, SLOT(displayIdn(const QStringList&)));
  
  // Set widget's initial state
  pbEssais->setEnabled(false);
  setDefaulIdn();
}

void mdtDeviceU3606AWidget::essais()
{
  qDebug() << "Essais: ";
  pbEssais->setEnabled(false);
  pvDevice.getIdn();
  pbEssais->setEnabled(true);
}

void mdtDeviceU3606AWidget::scan()
{
  pbScan->setEnabled(false);
  cbPort->clear();
  cbPort->addItems(pvDevice.scan());
  pbScan->setEnabled(true);
}

void mdtDeviceU3606AWidget::selectPort(const QString &port)
{
  pbEssais->setEnabled(false);
  qDebug() << "Select port " << port;
  if(port.size() > 0){
    if(!pvDevice.selectPort(port)){
      qDebug() << "Error selecting port";
      return;
    }
  }
  // Set the device IDN data
  pvDevice.getIdn();
  pbEssais->setEnabled(true);
}

void mdtDeviceU3606AWidget::displayIdn(const QStringList &data)
{
  // We must have 6 items in data
  if(data.size() != 6){
    setDefaulIdn();
    return;
  }
  
  lbManufacturer->setText(data.at(0));
  lbModel->setText(data.at(1));
  lbSerial->setText(data.at(2));
  lbIfBoardFw->setText(data.at(3));
  lbMeasureBoardFw->setText(data.at(4));
  lbSrcBoardFw->setText(data.at(5));
}

void mdtDeviceU3606AWidget::setDefaulIdn()
{
  lbManufacturer->setText(tr("Unknow"));
  lbModel->setText(tr("Unknow"));
  lbSerial->setText(tr("Unknow"));
  lbIfBoardFw->setText(tr("Unknow"));
  lbMeasureBoardFw->setText(tr("Unknow"));
  lbSrcBoardFw->setText(tr("Unknow"));
}
