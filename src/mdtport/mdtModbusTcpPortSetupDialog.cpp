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
#include "mdtModbusTcpPortSetupDialog.h"
#include "mdtModbusTcpPortManager.h"
#include <QWidget>
#include <QStringList>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QVariant>
#include <QNetworkInterface>

//#include <QDebug>

mdtModbusTcpPortSetupDialog::mdtModbusTcpPortSetupDialog(QWidget *parent)
 : mdtAbstractPortSetupDialog(parent)
{
  // Setup the dialog's header part
  pbOpen = new QPushButton(tr("&Open"));
  pbClose = new QPushButton(tr("&Close"));
  pbRescan = new QPushButton(tr("Rescan"));
  ///cbPort = new QComboBox;
  cbDevices = new QComboBox;
  QWidget *wHeader = new QWidget;
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(pbOpen, 0, 0);
  layout->addWidget(pbClose, 0, 1);
  layout->addWidget(pbRescan, 0, 2);
  layout->addWidget(cbDevices, 0, 3, 1, 8);
  ///layout->addWidget(new QLabel(tr("Interface")), 1, 0);
  ///layout->addWidget(cbInterface, 1, 1, 1, 5);
  layout->setColumnStretch(5, 1);
  ///layout->setColumnStretch(8, 2);
  wHeader->setLayout(layout);
  setHeaderWidget(wHeader);
  connect(pbClose, SIGNAL(clicked()), this, SLOT(closePort()));
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
  // Add devices list combobox to handler
  pvPortInfoCbHandler.setPortsComboBox(cbDevices);
  ///pvPortInfoCbHandler.setDevicesComboBox(cbInterface);
  // Setup device informations tab
  pvDeviceInfoWidget = new QWidget;
  QFormLayout *deviceInfoLayout = new QFormLayout;
  ///lbManufacturer = new QLabel;
  ///lbModel = new QLabel;
  ///lbSerial = new QLabel;
  ///lbFirmware = new QLabel;
  ///deviceInfoLayout->addRow(tr("Manufacturer:"), lbManufacturer);
  ///deviceInfoLayout->addRow(tr("Model:"), lbModel);
  ///deviceInfoLayout->addRow(tr("Serial:"), lbSerial);
  ///deviceInfoLayout->addRow(tr("Firmware:"), lbFirmware);
  ///pvDeviceInfoWidget->setLayout(deviceInfoLayout);
  ///addOptionsTabWidget(pvDeviceInfoWidget, tr("&Device informations"));
}

mdtModbusTcpPortSetupDialog::~mdtModbusTcpPortSetupDialog()
{
}

void mdtModbusTcpPortSetupDialog::displayConfig()
{

}

void mdtModbusTcpPortSetupDialog::updateConfig()
{
}

void mdtModbusTcpPortSetupDialog::closePort()
{
  portManager()->closePort();
}

void mdtModbusTcpPortSetupDialog::rescan()
{
  mdtModbusTcpPortManager *modbusTcpPortManager;
  int index;
  QVariant var;

  modbusTcpPortManager = dynamic_cast<mdtModbusTcpPortManager*>(portManager());
  Q_ASSERT(modbusTcpPortManager != 0);

  if(portManager()->isRunning()){
    showStatusMessage(tr("Cannot rescan because port is open"), 3000);
    return;
  }
  pbRescan->setEnabled(false);
  pbOpen->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(modbusTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100));
  // Display current port
  ///var.setValue(portManager()->portInfo());
  ///index = cbPort->findData(var);
  ///if(index >= 0){
    ///cbPort->setCurrentIndex(index);
  ///}
  pbRescan->setEnabled(true);
  pbOpen->setEnabled(true);
}

void mdtModbusTcpPortSetupDialog::openPort()
{
  applySetup();
}

void mdtModbusTcpPortSetupDialog::portManagerSet()
{
  displayConfig();
  connect(portManager(), SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
}

void mdtModbusTcpPortSetupDialog::setStateDisconnected()
{
  pbOpen->setEnabled(true);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(true);
  ///cbPort->setEnabled(true);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateConnecting()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  ///cbPort->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateReady()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  ///cbPort->setEnabled(false);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateBusy()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  ///cbPort->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateWarning()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  ///cbPort->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateError()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  ///cbPort->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

bool mdtModbusTcpPortSetupDialog::applySetup()
{
  ///mdtUsbtmcPortManager *usbtmcPortManager;

  // We must close the port
  portManager()->closePort();
  // Apply current configuration
  updateConfig();
  portManager()->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  // Try to open port and start port manager
  if(!portManager()->openPort()){
    showStatusMessage(tr("Cannot open port"), 3000);
    return false;
  }
  if(!portManager()->start()){
    showStatusMessage(tr("Cannot start thread"), 3000);
    portManager()->closePort();
    return false;
  }

}
