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
#include "mdtUsbtmcPortSetupDialog.h"
#include "mdtUsbtmcPortManager.h"
#include <QWidget>
#include <QStringList>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QVariant>

//#include <QDebug>

mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget *parent)
 : mdtAbstractPortSetupDialog(parent)
{
  // Setup the dialog's header part
  pbOpen = new QPushButton(tr("&Open"));
  pbClose = new QPushButton(tr("&Close"));
  pbRescan = new QPushButton(tr("Rescan"));
  cbPort = new QComboBox;
  cbInterface = new QComboBox;
  QWidget *wHeader = new QWidget;
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(pbOpen, 0, 0);
  layout->addWidget(pbClose, 0, 1);
  layout->addWidget(pbRescan, 0, 2);
  layout->addWidget(cbPort, 0, 3, 1, 8);
  layout->addWidget(new QLabel(tr("Interface")), 1, 0);
  layout->addWidget(cbInterface, 1, 1, 1, 5);
  layout->setColumnStretch(5, 1);
  layout->setColumnStretch(8, 2);
  wHeader->setLayout(layout);
  setHeaderWidget(wHeader);
  connect(pbClose, SIGNAL(clicked()), this, SLOT(closePort()));
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
  // Add bus/device list combobox to handler
  pvPortInfoCbHandler.setPortsComboBox(cbPort);
  pvPortInfoCbHandler.setDevicesComboBox(cbInterface);
  // Setup device informations tab
  pvDeviceInfoWidget = new QWidget;
  QFormLayout *deviceInfoLayout = new QFormLayout;
  lbManufacturer = new QLabel;
  lbModel = new QLabel;
  lbSerial = new QLabel;
  lbFirmware = new QLabel;
  deviceInfoLayout->addRow(tr("Manufacturer:"), lbManufacturer);
  deviceInfoLayout->addRow(tr("Model:"), lbModel);
  deviceInfoLayout->addRow(tr("Serial:"), lbSerial);
  deviceInfoLayout->addRow(tr("Firmware:"), lbFirmware);
  pvDeviceInfoWidget->setLayout(deviceInfoLayout);
  addOptionsTabWidget(pvDeviceInfoWidget, tr("&Device informations"));
}

mdtUsbtmcPortSetupDialog::~mdtUsbtmcPortSetupDialog()
{
}

void mdtUsbtmcPortSetupDialog::displayConfig()
{
  // Rescan
  if(!portManager()->isRunning()){
    rescan();
  }
}

void mdtUsbtmcPortSetupDialog::updateConfig()
{
}

void mdtUsbtmcPortSetupDialog::closePort()
{
  portManager()->closePort();
}

void mdtUsbtmcPortSetupDialog::rescan()
{
  int index;
  QVariant var;

  if(portManager()->isRunning()){
    showStatusMessage(tr("Cannot rescan because port is open"), 3000);
    return;
  }
  pbRescan->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(portManager()->scan());
  // Display current port
  var.setValue(portManager()->portInfo());
  index = cbPort->findData(var);
  if(index >= 0){
    cbPort->setCurrentIndex(index);
  }
  pbRescan->setEnabled(true);
}

void mdtUsbtmcPortSetupDialog::openPort()
{
  applySetup();
}

void mdtUsbtmcPortSetupDialog::portManagerSet()
{
  displayConfig();
}

void mdtUsbtmcPortSetupDialog::setStateDisconnected()
{
  pbOpen->setEnabled(true);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(true);
  cbPort->setEnabled(true);
  cbInterface->setEnabled(true);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtUsbtmcPortSetupDialog::setStateConnecting()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  cbInterface->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtUsbtmcPortSetupDialog::setStateReady()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  cbInterface->setEnabled(false);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtUsbtmcPortSetupDialog::setStateBusy()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  cbInterface->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtUsbtmcPortSetupDialog::setStateWarning()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  cbInterface->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

void mdtUsbtmcPortSetupDialog::setStateError()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  cbInterface->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

bool mdtUsbtmcPortSetupDialog::applySetup()
{
  mdtUsbtmcPortManager *usbtmcPortManager;
  QString idn;
  QStringList idnItems;

  /// \todo Check if current port info is empty before
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
    return false;
  }
  // Get identification
  usbtmcPortManager = dynamic_cast<mdtUsbtmcPortManager*>(portManager());
  Q_ASSERT(usbtmcPortManager != 0);
  idn = usbtmcPortManager->sendQuery("*IDN?\n");
  if(idn.isEmpty()){
    usbtmcPortManager->closePort();
    showStatusMessage(tr("Cannot get device infos (*IDN? query failed)"), 3000);
    return false;
  }
  idn = idn.trimmed();
  idnItems = idn.split(',');
  if(idnItems.size() != 4){
    usbtmcPortManager->closePort();
    showStatusMessage(tr("Cannot get device infos (*IDN? query returned unexpected data)"), 3000);
    return false;
  }
  // Display infos
  lbManufacturer->setText(idnItems.at(0));
  lbModel->setText(idnItems.at(1));
  lbSerial->setText(idnItems.at(2));
  lbFirmware->setText(idnItems.at(3));

  return true;
}
