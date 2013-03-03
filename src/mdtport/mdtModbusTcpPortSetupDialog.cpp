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
#include "mdtError.h"
#include <QWidget>
#include <QStringList>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QNetworkInterface>
#include <QCloseEvent>

#include <QVariant>
//#include <QDebug>

mdtModbusTcpPortSetupDialog::mdtModbusTcpPortSetupDialog(QWidget *parent)
 : mdtAbstractPortSetupDialog(parent)
{
  pvModbusTcpPortManager = 0;
  // Setup the dialog's header part
  pbOpen = new QPushButton(tr("&Open"));
  pbClose = new QPushButton(tr("&Close"));
  pbRescan = new QPushButton(tr("Rescan"));
  pbAbort = new QPushButton(tr("&Abort"));
  ///cbPort = new QComboBox;
  cbDevices = new QComboBox;
  QWidget *wHeader = new QWidget;
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(pbOpen, 0, 0);
  layout->addWidget(pbClose, 0, 1);
  layout->addWidget(pbRescan, 0, 2);
  layout->addWidget(pbAbort, 0, 3);
  layout->addWidget(cbDevices, 0, 4, 1, 8);
  ///layout->addWidget(new QLabel(tr("Interface")), 1, 0);
  ///layout->addWidget(cbInterface, 1, 1, 1, 5);
  layout->setColumnStretch(5, 1);
  ///layout->setColumnStretch(8, 2);
  wHeader->setLayout(layout);
  setHeaderWidget(wHeader);
  connect(pbClose, SIGNAL(clicked()), this, SLOT(closePort()));
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
  pbAbort->setEnabled(false);
  // Add devices list combobox to handler
  pvPortInfoCbHandler.setPortsComboBox(cbDevices);
  ///pvPortInfoCbHandler.setDevicesComboBox(cbInterface);
  // Setup device informations tab
  pvDeviceInfoWidget = new QWidget;
  QFormLayout *deviceInfoLayout = new QFormLayout;
  lbManufacturer = new QLabel;
  lbModel = new QLabel;
  lbHwRevision = new QLabel;
  lbFwRevision = new QLabel;
  ///lbSerial = new QLabel;
  ///lbFirmware = new QLabel;
  deviceInfoLayout->addRow(tr("Manufacturer:"), lbManufacturer);
  deviceInfoLayout->addRow(tr("Model:"), lbModel);
  deviceInfoLayout->addRow(tr("Hardware revision:"), lbHwRevision);
  deviceInfoLayout->addRow(tr("Firmware revision:"), lbFwRevision);
  ///deviceInfoLayout->addRow(tr("Serial:"), lbSerial);
  ///deviceInfoLayout->addRow(tr("Firmware:"), lbFirmware);
  pvDeviceInfoWidget->setLayout(deviceInfoLayout);
  addOptionsTabWidget(pvDeviceInfoWidget, tr("&Device informations"));
}

mdtModbusTcpPortSetupDialog::~mdtModbusTcpPortSetupDialog()
{
}

void mdtModbusTcpPortSetupDialog::displayConfig()
{
  int index;

  qDebug() << "mdtModbusTcpPortSetupDialog::displayConfig(): portName: " << portManager()->portName();
  // Set current port in devices list
  if(!portManager()->portName().isEmpty()){
    index = cbDevices->findData(portManager()->portName());
    if(index < 0){
      qDebug() << "mdtModbusTcpPortSetupDialog::displayConfig(): add portName: " << portManager()->portName();
      cbDevices->addItem(portManager()->portInfo().displayText(), portManager()->portName());
      index = cbDevices->findData(portManager()->portName());
    }
    qDebug() << "mdtModbusTcpPortSetupDialog::displayConfig(): index: " << index;
    cbDevices->setCurrentIndex(index);
  }
  // If port manager is running, we can try to get device informations
  if(portManager()->isRunning()){
    getDeviceInformations();
  }
}

void mdtModbusTcpPortSetupDialog::updateConfig()
{
}

void mdtModbusTcpPortSetupDialog::closePort()
{
  clearDeviceInformations();
  portManager()->closePort();
}

void mdtModbusTcpPortSetupDialog::rescan()
{
  int index;
  ///QVariant var;

  Q_ASSERT(pvModbusTcpPortManager != 0);

  if(pvModbusTcpPortManager->isRunning()){
    showStatusMessage(tr("Cannot rescan because port is open"), 3000);
    return;
  }
  pbRescan->setEnabled(false);
  pbAbort->setEnabled(true);
  pbOpen->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  cbDevices->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(pvModbusTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100));
  // Display current port
  ///var.setValue(portManager()->portInfo().portName());
  index = cbDevices->findData(portManager()->portInfo().portName());
  if(index >= 0){
    cbDevices->setCurrentIndex(index);
  }
  pbAbort->setEnabled(false);
  pbRescan->setEnabled(true);
  pbOpen->setEnabled(true);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  cbDevices->setEnabled(true);
}

void mdtModbusTcpPortSetupDialog::openPort()
{
  if(!applySetup()){
    return;
  }
  getDeviceInformations();
}

void mdtModbusTcpPortSetupDialog::portManagerSet()
{
  pvModbusTcpPortManager = dynamic_cast<mdtModbusTcpPortManager*>(portManager());
  Q_ASSERT(pvModbusTcpPortManager != 0);

  displayConfig();
  connect(pvModbusTcpPortManager, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), this, SLOT(showStatusMessage(const QString&, const QString&, int)));
  connect(pbAbort, SIGNAL(clicked()), pvModbusTcpPortManager, SLOT(abortScan()));
}

void mdtModbusTcpPortSetupDialog::setStateDisconnected()
{
  pbOpen->setEnabled(true);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(true);
  cbDevices->setEnabled(true);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateConnecting()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateReady()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateBusy()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateWarning()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateError()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

bool mdtModbusTcpPortSetupDialog::applySetup()
{
  ///mdtUsbtmcPortManager *usbtmcPortManager;

  // Check that a port was selected
  if(cbDevices->currentText().isEmpty()){
    showStatusMessage(tr("Please select a device in list"), 3000);
    return false;
  }
  // We must close the port
  portManager()->closePort();
  // Apply current configuration
  updateConfig();
  portManager()->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  // Try to open port and start port manager
  if(!portManager()->openPort()){
    showStatusMessage(tr("Cannot open port ") + portManager()->portName(), 3000);
    return false;
  }
  if(!portManager()->start()){
    showStatusMessage(tr("Cannot start thread"), 3000);
    portManager()->closePort();
    return false;
  }

  return true;
}

void mdtModbusTcpPortSetupDialog::closeEvent(QCloseEvent *event)
{
  Q_ASSERT(pvModbusTcpPortManager != 0);

  pvModbusTcpPortManager->abortScan();
  event->accept();
}

void mdtModbusTcpPortSetupDialog::clearDeviceInformations()
{
  lbManufacturer->clear();
  lbModel->clear();
  lbFwRevision->clear();
  lbHwRevision->clear();
}

void mdtModbusTcpPortSetupDialog::getDeviceInformations()
{
  Q_ASSERT(pvModbusTcpPortManager != 0);

  clearDeviceInformations();
  if(!pvModbusTcpPortManager->isRunning()){
    showStatusMessage(tr("Cannot get device informations, please connect first"), 3000);
    return;
  }
  /*
   * MODBUS specifications provide a way to get device informations:
   *  FC 43 / 14 (0x2B / 0x0E) : Read Device Identification
   * But, it seems that Wago (750-xxx) and Beckhoff (BK90xx) don't implement it :-(
   * So, try some calls of register service to find them ...
   */
  // We will try to get values from some registers. This will add errors to log (Something like: invalid address range)
  // We put a information into the log, so that we know that we are fetching device informations.
  mdtError e1(MDT_NO_ERROR, "Checking about device manufacturer, can produce some errors ...", mdtError::Info);
  MDT_ERROR_SET_SRC(e1, "mdtDeviceModbusWago");
  e1.commit();
  // Try Wago informations
  if(getWago750Informations()){
    return;
  }
  // Try Beckhoff informations
  if(getBeckhoffBcInformations()){
    return;
  }

}

bool mdtModbusTcpPortSetupDialog::getWago750Informations()
{
  Q_ASSERT(pvModbusTcpPortManager != 0);
  Q_ASSERT(pvModbusTcpPortManager->isRunning());

  QString model;
  QString fwRevision;

  // Wago give the series number (f.ex. 750) at register address 0x2011
  if(!pvModbusTcpPortManager->getRegisterValues(0x2011, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  model = QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Item number is available at register address 0x2012
  if(!pvModbusTcpPortManager->getRegisterValues(0x2012, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  model += "-";
  model += QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Firmware revision major is available at register address 0x2013
  if(!pvModbusTcpPortManager->getRegisterValues(0x2013, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  fwRevision = QString::number(pvModbusTcpPortManager->registerValues().at(0));
  ///qDebug() << "getWago750Informations() , FW major: " << pvModbusTcpPortManager->registerValues();
  // Firmware revision minor is available at register address 0x2014
  if(!pvModbusTcpPortManager->getRegisterValues(0x2014, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  fwRevision += ".";
  fwRevision += QString::number(pvModbusTcpPortManager->registerValues().at(0));
  ///qDebug() << "getWago750Informations() , FW minor: " << pvModbusTcpPortManager->registerValues();
  // Firmware revision index is available at register address 0x2010
  if(!pvModbusTcpPortManager->getRegisterValues(0x2010, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  fwRevision += ".";
  fwRevision += QString::number(pvModbusTcpPortManager->registerValues().at(0));
  ///qDebug() << "getWago750Informations() , FW index: " << pvModbusTcpPortManager->registerValues();
  ///qDebug() << "getWago750Informations() , FW: " << fwRevision;
  // We should be connected to a Wago fieldbus
  lbManufacturer->setText("Wago");
  lbModel->setText(model);
  lbHwRevision->setText("N/A");
  lbFwRevision->setText(fwRevision);

  return true;
}

bool mdtModbusTcpPortSetupDialog::getBeckhoffBcInformations()
{
  Q_ASSERT(pvModbusTcpPortManager != 0);
  Q_ASSERT(pvModbusTcpPortManager->isRunning());

  QString model;
  QString fwRevision;
  QString hwRevision;

  if(!pvModbusTcpPortManager->getRegisterValues(0x1000, 7)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 7){
    return false;
  }
  qDebug() << "getBeckhoffBcInformations(): registers: " << pvModbusTcpPortManager->registerValues();
  // In the 3 first registers, we find the series name (f.ex. BK9000)
  model.append((char)(pvModbusTcpPortManager->registerValues().at(0) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(0) >> 8));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(1) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(1) >> 8));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(2) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(2) >> 8));
  if((model != "BK9000")&&(model != "BC9000")){
    return false;
  }
  // In 3 next registers, we find the firmware and hardware revision
  ///fwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(3) & 0xFF));
  fwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(3) >> 8));
  fwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(4) & 0xFF));
  hwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(5) & 0xFF));
  hwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(5) >> 8));
  
  qDebug() << "--> FW: " << fwRevision;
  qDebug() << "--> HW: " << hwRevision;

  for(int i=0; i<pvModbusTcpPortManager->registerValues().size(); i++){
    qDebug() << " -> REG[" << i << "]: MSB: " << (char)(pvModbusTcpPortManager->registerValues().at(i) >> 8);
    qDebug() << " -> REG[" << i << "]: LSB: " << (char)(pvModbusTcpPortManager->registerValues().at(i) & 0xFF);
  }

  // We should be connected to BC9000 or BK9000 from Beckhoff
  lbManufacturer->setText("Beckhoff");
  lbModel->setText(model);
  lbHwRevision->setText(hwRevision);
  lbFwRevision->setText(fwRevision);


  return true;
}
