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

#include <QDebug>

/// \todo Handle Ok/Apply/Stop buttons: f.ex.: when something fails, Ok button is enabled, not coherent

mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget *parent)
/// : QDialog(parent)
 : mdtAbstractPortSetupDialog(parent)
{
  ///pvPortManager = 0;
  ///pvPortConfigWidget = new mdtPortConfigWidget;
  // Setup the dialog ui
  ///setupUi(this);

  // Setup the dialog's header part
  pbOpen = new QPushButton(tr("&Open"));
  pbClose = new QPushButton(tr("&Close"));
  pbRescan = new QPushButton(tr("Rescan"));
  cbPort = new QComboBox;
  cbInterface = new QComboBox;
  ///cbPort->setMinimumWidth(350);
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
  // Update GUI
  ///displayConfig();
}

mdtUsbtmcPortSetupDialog::~mdtUsbtmcPortSetupDialog()
{
}

/**
void mdtUsbtmcPortSetupDialog::setPortManager(mdtUsbtmcPortManager *manager)
{
  Q_ASSERT(manager != 0);

  pvPortManager = manager;
  pvCurrentPortInfo = pvPortManager->portInfo();
  // Set state regarding manager's state
  if(pvPortManager->isRunning()){
    setStateRunning();
  }else{
    setStateStopped();
  }
  // List available ports
  on_pbRescan_clicked();
  // Update GUI
  displayConfig();
}
*/

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

/**
void mdtUsbtmcPortSetupDialog::on_pbRescan_clicked()
{
  Q_ASSERT(pvPortManager != 0);

  // Stop manager and close port
  lbState->setText(tr("Stopping ..."));
  pvPortManager->closePort();
  setStateStopped();
  pbRescan->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(pvPortManager->scan());
  pbRescan->setEnabled(true);
}
*/

/**
void mdtUsbtmcPortSetupDialog::on_cbInterface_currentIndexChanged(int index)
{
  QString idn;
  QStringList idnItems;

  if(index < 0){
    return;
  }
  if(pvPortManager == 0){
    return;
  }
  cbInterface->setEnabled(false);
  diseableApplyButtons();
  // Close port
  pvPortManager->closePort();
  setStateStopped();
  // Open the port
  pvPortManager->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  if(!pvPortManager->openPort()){
    setStateError(tr("Cannot open port"));
    cbInterface->setEnabled(true);
    return;
  }
  if(!pvPortManager->start()){
    pvPortManager->closePort();
    setStateError(tr("Cannot start port"));
    cbInterface->setEnabled(true);
    return;
  }
  // Get identification
  idn = pvPortManager->sendQuery("*IDN?\n");
  if(idn.isEmpty()){
    pvPortManager->closePort();
    setStateError(tr("*IDN? query failed"));
    cbInterface->setEnabled(true);
    return;
  }
  */
  /**
  if(pvPortManager->writeData("*IDN?\n") < 0){
    pvPortManager->closePort();
    setStateError(tr("Cannot send the *IDN? query"));
    cbInterface->setEnabled(true);
    return;
  }
  if(pvPortManager->sendReadRequest(true) < 0){
    pvPortManager->closePort();
    setStateError(tr("Cannot send the *IDN? read request"));
    cbInterface->setEnabled(true);
    return;
  }
  if(!pvPortManager->waitReadenFrame()){
    pvPortManager->closePort();
    setStateError(tr("Cannot send the *IDN? query (received no data)"));
    cbInterface->setEnabled(true);
    return;
  }
  if(pvPortManager->readenFrames().size() != 1){
    pvPortManager->closePort();
    setStateError(tr("Cannot send the *IDN? query (received wrong amount of data)"));
    cbInterface->setEnabled(true);
    return;
  }
  */
  // Split IDN replay infos
  ///idn = pvPortManager->readenFrames().at(0);
  ///pvPortManager->clearReadenFrames();
  /**
  idn = idn.trimmed();
  idnItems = idn.split(',');
  if(idnItems.size() != 4){
    pvPortManager->closePort();
    setStateError(tr("Cannot send the *IDN? query (received wrong amount of data)"));
    cbInterface->setEnabled(true);
    return;
  }
  // Display infos
  lbManufacturer->setText(idnItems.at(0));
  lbModel->setText(idnItems.at(1));
  lbSerial->setText(idnItems.at(2));
  lbFirmware->setText(idnItems.at(3));
  // Display configuration
  displayConfig();
  setStateRunning();
  cbInterface->setEnabled(true);
  enableApplyButtons();
}
*/

  /**
void mdtUsbtmcPortSetupDialog::on_buttonBox_clicked(QAbstractButton *button)
{
  Q_ASSERT(pvPortManager != 0);
  Q_ASSERT(button != 0);

  QDialogButtonBox::StandardButton type;

  type = buttonBox->standardButton(button);
  if((type == QDialogButtonBox::Apply)||(type == QDialogButtonBox::Ok)){
    // Update current port info
    pvCurrentPortInfo = pvPortInfoCbHandler.currentPortInfo();
  }
  // Check if something is to do
  if(pvPortManager->portInfo() != pvCurrentPortInfo){
    pvPortManager->closePort();
    pvPortManager->setPortInfo(pvCurrentPortInfo);
    setStateStopped();
  }
  // Open and start port if needed
  if(!pvPortManager->isRunning()){
    if(!pvPortManager->openPort()){
      setStateError(tr("Cannot open port"));
      return;
    }
    if(!pvPortManager->start()){
      pvPortManager->closePort();
      setStateError(tr("Cannot start port"));
      return;
    }
  }
  setStateRunning();
}
*/

  /**
void mdtUsbtmcPortSetupDialog::setStateRunning()
{
  // Update state label
  lbState->setText(tr("Running"));
  lbState->setStyleSheet("QLabel { background-color : green; color : black; }");
}
*/

/**
void mdtUsbtmcPortSetupDialog::setStateStopped()
{
  // Update state label
  lbState->setText(tr("Stopped"));
  lbState->setStyleSheet("QLabel { background-color : orange; color : black; }");
}
*/

/**
void mdtUsbtmcPortSetupDialog::setStateError(QString msg)
{
  // Update state label
  lbState->setText(msg);
  lbState->setStyleSheet("QLabel { background-color : red; color : black; }");
}
*/

/**
void mdtUsbtmcPortSetupDialog::diseableApplyButtons()
{
  QPushButton *b;

  b = buttonBox->button(QDialogButtonBox::Ok);
  if(b != 0){
    b->setEnabled(false);
  }
  b = buttonBox->button(QDialogButtonBox::Apply);
  if(b != 0){
    b->setEnabled(false);
  }
}
*/

/**
void mdtUsbtmcPortSetupDialog::enableApplyButtons()
{
  QPushButton *b;

  b = buttonBox->button(QDialogButtonBox::Ok);
  if(b != 0){
    b->setEnabled(true);
  }
  b = buttonBox->button(QDialogButtonBox::Apply);
  if(b != 0){
    b->setEnabled(true);
  }
}
*/

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
  ///pvDeviceInfoWidget->setEnabled(true);
  ///pvPortConfigWidget->setEnabled(true);
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
  ///pvDeviceInfoWidget->setEnabled(false);
  ///pvPortConfigWidget->setEnabled(false);
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
  ///pvDeviceInfoWidget->setEnabled(false);
  ///pvPortConfigWidget->setEnabled(false);
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
  ///pvDeviceInfoWidget->setEnabled(false);
  ///pvPortConfigWidget->setEnabled(false);
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
  ///pvDeviceInfoWidget->setEnabled(false);
  ///pvPortConfigWidget->setEnabled(false);
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
  ///pvDeviceInfoWidget->setEnabled(false);
  ///pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

bool mdtUsbtmcPortSetupDialog::applySetup()
{
  mdtUsbtmcPortManager *usbtmcPortManager;
  QString idn;
  QStringList idnItems;

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
