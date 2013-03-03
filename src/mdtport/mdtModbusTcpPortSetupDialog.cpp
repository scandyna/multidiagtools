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
#include "mdtPortInfo.h"
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
#include <QLineEdit>
#include <QSpinBox>

///#include <QVariant>
//#include <QDebug>

mdtModbusTcpPortSetupDialog::mdtModbusTcpPortSetupDialog(QWidget *parent)
 : mdtAbstractPortSetupDialog(parent)
{
  pvModbusTcpPortManager = 0;
  // Setup the dialog's header part
  pbConnect = new QPushButton(tr("&Connect"));
  pbDisconnect = new QPushButton(tr("&Disconnect"));
  pbRescan = new QPushButton(tr("Rescan"));
  pbAbort = new QPushButton(tr("&Abort"));
  cbDevices = new QComboBox;
  lbHost = new QLabel(tr("Host:"));
  leHost = new QLineEdit;
  lbPort = new QLabel(tr("Port:"));
  sbPort = new QSpinBox;
  sbPort->setMinimum(500);
  sbPort->setMaximum(50000);
  QWidget *wHeader = new QWidget;
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(pbRescan, 0, 0);
  layout->addWidget(pbAbort, 0, 1);
  layout->addWidget(cbDevices, 0, 2, 1, 8);
  layout->addWidget(pbConnect, 1, 0);
  layout->addWidget(pbDisconnect, 1, 1);
  layout->addWidget(lbHost, 1, 2);
  layout->addWidget(leHost, 1, 3, 1, 2);
  layout->addWidget(lbPort, 1, 5);
  layout->addWidget(sbPort, 1, 6);
  
  ///layout->addWidget(new QLabel(tr("Interface")), 1, 0);
  ///layout->addWidget(cbInterface, 1, 1, 1, 5);
  layout->setColumnStretch(3, 1);
  ///layout->setColumnStretch(8, 2);
  wHeader->setLayout(layout);
  setHeaderWidget(wHeader);
  connect(pbDisconnect, SIGNAL(clicked()), this, SLOT(closePort()));
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  ///connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
  connect(pbConnect, SIGNAL(clicked()), this, SLOT(openPort()));
  connect(cbDevices, SIGNAL(currentIndexChanged(int)), this, SLOT(displayHostPort(int)));
  pbAbort->setEnabled(false);
  // Add devices list combobox to handler
  pvPortInfoCbHandler.setPortsComboBox(cbDevices);
  // Setup device informations tab
  pvDeviceInfoWidget = new QWidget;
  QFormLayout *deviceInfoLayout = new QFormLayout;
  lbManufacturer = new QLabel;
  lbModel = new QLabel;
  lbHwRevision = new QLabel;
  lbFwRevision = new QLabel;
  lbIOs = new QLabel;
  lbHwNodeAddress = new QLabel;
  deviceInfoLayout->addRow(tr("Manufacturer:"), lbManufacturer);
  deviceInfoLayout->addRow(tr("Model:"), lbModel);
  deviceInfoLayout->addRow(tr("Hardware revision:"), lbHwRevision);
  deviceInfoLayout->addRow(tr("Firmware revision:"), lbFwRevision);
  deviceInfoLayout->addRow(tr("I/O count:"), lbIOs);
  deviceInfoLayout->addRow(tr("HW node address:"), lbHwNodeAddress);
  pvDeviceInfoWidget->setLayout(deviceInfoLayout);
  addOptionsTabWidget(pvDeviceInfoWidget, tr("&Device informations"));
  setWindowTitle(tr("MODBUS/TCP setup"));
  // Set a convenable size
  resize(700, 280);
}

mdtModbusTcpPortSetupDialog::~mdtModbusTcpPortSetupDialog()
{
}

void mdtModbusTcpPortSetupDialog::displayConfig()
{
  int index;

  // Set current port in devices list
  if(!portManager()->portName().isEmpty()){
    index = cbDevices->findData(portManager()->portName());
    if(index < 0){
      cbDevices->addItem(portManager()->portInfo().displayText(), portManager()->portName());
      index = cbDevices->findData(portManager()->portName());
    }
    cbDevices->setCurrentIndex(index);
  }
  // If port manager is running, we can try to get device informations
  if(portManager()->isRunning()){
    getDeviceInformations();
    getHardwareNodeAddress();
  }
}

void mdtModbusTcpPortSetupDialog::displayHostPort(int cbDevicesIndex)
{
  QString portName;
  QStringList portNameItems;
  int port;
  bool ok;

  if(cbDevicesIndex < 0){
    leHost->clear();
    sbPort->setValue(502);
    return;
  }
  portName = cbDevices->itemData(cbDevicesIndex).toString();
  portNameItems = portName.split(":");
  if(portNameItems.size() == 2){
    leHost->setText(portNameItems.at(0));
    port = portNameItems.at(1).toInt(&ok);
    if(ok){
      sbPort->setValue(port);
    }else{
      showStatusMessage(tr("Cannot extract port from port name"), 2000);
    }
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

  Q_ASSERT(pvModbusTcpPortManager != 0);

  if(pvModbusTcpPortManager->isRunning()){
    showStatusMessage(tr("Cannot rescan because port is open"), 3000);
    return;
  }
  pbRescan->setEnabled(false);
  pbAbort->setEnabled(true);
  pbConnect->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  cbDevices->setEnabled(false);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(pvModbusTcpPortManager->scan(QNetworkInterface::allInterfaces(), 502, 100));
  // Display current port
  index = cbDevices->findData(portManager()->portInfo().portName());
  if(index >= 0){
    cbDevices->setCurrentIndex(index);
  }
  pbAbort->setEnabled(false);
  pbRescan->setEnabled(true);
  pbConnect->setEnabled(true);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  cbDevices->setEnabled(true);
  leHost->setEnabled(true);
  sbPort->setEnabled(true);
}

void mdtModbusTcpPortSetupDialog::openPort()
{
  if(!applySetup()){
    return;
  }
  getDeviceInformations();
  getHardwareNodeAddress();
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
  pbConnect->setEnabled(true);
  pbDisconnect->setEnabled(false);
  pbRescan->setEnabled(true);
  cbDevices->setEnabled(true);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
  leHost->setEnabled(true);
  sbPort->setEnabled(true);
}

void mdtModbusTcpPortSetupDialog::setStateConnecting()
{
  pbConnect->setEnabled(false);
  pbDisconnect->setEnabled(false);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateReady()
{
  pbConnect->setEnabled(false);
  pbDisconnect->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateBusy()
{
  pbConnect->setEnabled(false);
  pbDisconnect->setEnabled(false);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateWarning()
{
  pbConnect->setEnabled(false);
  pbDisconnect->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
}

void mdtModbusTcpPortSetupDialog::setStateError()
{
  pbConnect->setEnabled(false);
  pbDisconnect->setEnabled(true);
  pbRescan->setEnabled(false);
  cbDevices->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
  leHost->setEnabled(false);
  sbPort->setEnabled(false);
}

bool mdtModbusTcpPortSetupDialog::applySetup()
{
  mdtPortInfo portInfo;

  // Check that host is set
  if(leHost->text().trimmed().isEmpty()){
    showStatusMessage(tr("Please enter a host name or IP address"), 3000);
    return false;
  }
  setStateConnecting();
  // Set port name
  portInfo.setPortName(leHost->text().trimmed() + ":" + QString::number(sbPort->value()));
  portInfo.setDisplayText(tr("Host: ") + leHost->text().trimmed() + tr("   ,   Port: ") + QString::number(sbPort->value()));
  // We must close the port
  portManager()->closePort();
  // Apply current configuration
  updateConfig();
  ///portManager()->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  portManager()->setPortInfo(portInfo);
  // Try to open port and start port manager
  showStatusMessage(tr("Trying to connect ..."));
  if(!portManager()->openPort()){
    setStateDisconnected();
    showStatusMessage(tr("Cannot connect to ") + portManager()->portName());
    return false;
  }
  if(!portManager()->start()){
    showStatusMessage(tr("Cannot start thread"));
    portManager()->closePort();
    setStateDisconnected();
    return false;
  }
  showStatusMessage(tr("Connected"), 3000);

  return true;
}

void mdtModbusTcpPortSetupDialog::closeEvent(QCloseEvent *event)
{
  Q_ASSERT(pvModbusTcpPortManager != 0);

  pvModbusTcpPortManager->abortScan();
  // If portManager is connecting or somthing similar, we stop it
  if((portManager()->currentState() != mdtPortManager::Ready)&&(portManager()->currentState() != mdtPortManager::Disconnected)){
    portManager()->stop();
  }
  event->accept();
}

void mdtModbusTcpPortSetupDialog::clearDeviceInformations()
{
  lbManufacturer->clear();
  lbModel->clear();
  lbFwRevision->clear();
  lbHwRevision->clear();
  lbIOs->clear();
  lbHwNodeAddress->clear();
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
  QString IOs;

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
  // Firmware revision minor is available at register address 0x2014
  if(!pvModbusTcpPortManager->getRegisterValues(0x2014, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  fwRevision += ".";
  fwRevision += QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Firmware revision index is available at register address 0x2010
  if(!pvModbusTcpPortManager->getRegisterValues(0x2010, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  fwRevision += ".";
  fwRevision += QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Get amount of analog inputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1023, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs = "Analog inputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0) / 16);
  // Get amount of analog outputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1022, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , analog outputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0) / 16);
  // Get amount of digital inputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1025, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , digital inputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Get amount of digital outputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1024, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , digital outputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0));

  // We should be connected to a Wago fieldbus
  lbManufacturer->setText("Wago");
  lbModel->setText(model);
  lbHwRevision->setText("N/A");
  lbFwRevision->setText(fwRevision);
  lbIOs->setText(IOs);

  return true;
}

bool mdtModbusTcpPortSetupDialog::getBeckhoffBcInformations()
{
  Q_ASSERT(pvModbusTcpPortManager != 0);
  Q_ASSERT(pvModbusTcpPortManager->isRunning());

  QString model;
  QString fwRevision;
  QString hwRevision;
  QString IOs;

  if(!pvModbusTcpPortManager->getRegisterValues(0x1000, 6)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 6){
    return false;
  }
  // In the 3 first registers, we find the series name (f.ex. BK9000)
  model.append((char)(pvModbusTcpPortManager->registerValues().at(0) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(0) >> 8));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(1) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(1) >> 8));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(2) & 0xFF));
  model.append((char)(pvModbusTcpPortManager->registerValues().at(2) >> 8));
  if((model != "BK9000")&&(model != "BC9000")&&(model != "BK9100")){
    return false;
  }
  // In 3 next registers, we find the firmware and hardware revision
  fwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(3) >> 8));
  fwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(4) & 0xFF));
  hwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(5) & 0xFF));
  hwRevision.append((char)(pvModbusTcpPortManager->registerValues().at(5) >> 8));
  // Get amount of analog inputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1011, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  /// \todo Check if Ok (need more Analog I/Os to check this part..)
  IOs = "Analog inputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0) / 64);
  // Get amount of analog outputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1010, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , analog outputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0) / 64);
  // Get amount of digital inputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1013, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , digital inputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // Get amount of digital outputs
  if(!pvModbusTcpPortManager->getRegisterValues(0x1012, 1)){
    return false;
  }
  if(pvModbusTcpPortManager->registerValues().size() != 1){
    return false;
  }
  IOs += " , digital outputs: " + QString::number(pvModbusTcpPortManager->registerValues().at(0));
  // We should be connected to BC9000 or BK9000 from Beckhoff
  lbManufacturer->setText("Beckhoff");
  lbModel->setText(model);
  lbHwRevision->setText(hwRevision);
  lbFwRevision->setText(fwRevision);
  lbIOs->setText(IOs);

  return true;
}

void mdtModbusTcpPortSetupDialog::getHardwareNodeAddress()
{
  Q_ASSERT(pvModbusTcpPortManager != 0);
  Q_ASSERT(pvModbusTcpPortManager->isRunning());

  int nodeId;

  nodeId = pvModbusTcpPortManager->getHardwareNodeAddress(8);
  if(nodeId < 1){
    lbHwNodeAddress->setText("N/A");
    return;
  }
  lbHwNodeAddress->setText(QString::number(nodeId));
}
