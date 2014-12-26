/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
///#include "mdtUsbtmcPortManager.h"
#include <QWidget>
///#include <QStringList>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QVariant>
///#include <QGridLayout>
///#include <QFormLayout>

// mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget *parent)
//  : mdtAbstractPortSetupDialog(parent)
// {
//   // Setup the dialog's header part
//   pbOpen = new QPushButton(tr("&Open"));
//   pbClose = new QPushButton(tr("&Close"));
//   pbRescan = new QPushButton(tr("Rescan"));
//   cbPort = new QComboBox;
//   cbInterface = new QComboBox;
//   QWidget *wHeader = new QWidget;
//   QGridLayout *layout = new QGridLayout;
//   layout->addWidget(pbOpen, 0, 0);
//   layout->addWidget(pbClose, 0, 1);
//   layout->addWidget(pbRescan, 0, 2);
//   layout->addWidget(cbPort, 0, 3, 1, 8);
//   layout->addWidget(new QLabel(tr("Interface")), 1, 0);
//   layout->addWidget(cbInterface, 1, 1, 1, 5);
//   layout->setColumnStretch(5, 1);
//   layout->setColumnStretch(8, 2);
//   wHeader->setLayout(layout);
//   setHeaderWidget(wHeader);
//   connect(pbClose, SIGNAL(clicked()), this, SLOT(closePort()));
//   connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
//   connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
//   // Add bus/device list combobox to handler
//   pvPortInfoCbHandler.setPortsComboBox(cbPort);
//   pvPortInfoCbHandler.setDevicesComboBox(cbInterface);
//   // Setup device informations tab
//   pvDeviceInfoWidget = new QWidget;
//   QFormLayout *deviceInfoLayout = new QFormLayout;
//   lbManufacturer = new QLabel;
//   lbModel = new QLabel;
//   lbSerial = new QLabel;
//   lbFirmware = new QLabel;
//   deviceInfoLayout->addRow(tr("Manufacturer:"), lbManufacturer);
//   deviceInfoLayout->addRow(tr("Model:"), lbModel);
//   deviceInfoLayout->addRow(tr("Serial:"), lbSerial);
//   deviceInfoLayout->addRow(tr("Firmware:"), lbFirmware);
//   pvDeviceInfoWidget->setLayout(deviceInfoLayout);
//   addOptionsTabWidget(pvDeviceInfoWidget, tr("&Device informations"));
// }

mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  cbDevices->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  cbInterfaces->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(cbDevices, SIGNAL(currentIndexChanged(int)), this, SLOT(buildInterfacesList(int)));
}

mdtUsbtmcPortSetupDialog::~mdtUsbtmcPortSetupDialog()
{
}

void mdtUsbtmcPortSetupDialog::rescan()
{
  setStateScanning();
  // Clear previous scan result
  cbInterfaces->clear();
  cbDevices->clear();
  pvDeviceDescriptorsList.clear();
  // Enumerate USBTMC devices
  pvDeviceDescriptorsList = pvPort.scan();
  if(pvDeviceDescriptorsList.isEmpty()){
    pvLastError = pvPort.lastError();
    displayLastError();
    setStateNoDeviceFound();
    return;
  }
  // Build comboboxes - We store pvDeviceDescriptorsList index as user data
  int i = 0;
  for(auto & deviceDescriptor : pvDeviceDescriptorsList){
    cbDevices->addItem(deviceDisplayText(deviceDescriptor), i);
    ++i;
  }
  // Update device info
  if(pvSelectedDevice.isEmpty()){
    setStateNoDeviceFound();
  }
  // We have at least 1 device here
  /// \todo Display device infos
  
  setStateDeviceSelected();
}

void mdtUsbtmcPortSetupDialog::buildInterfacesList(int deviceCbIndex)
{
  cbInterfaces->clear();
  if(deviceCbIndex < 0){
    return;
  }
  int devDescIdx = cbDevices->itemData(deviceCbIndex).toInt();

  Q_ASSERT(devDescIdx >= 0);
  Q_ASSERT(devDescIdx < pvDeviceDescriptorsList.size());
  QList<mdtUsbInterfaceDescriptor> ifaceDescriptors = pvDeviceDescriptorsList.at(devDescIdx).allInterfaces();
  // Build interface list - We store bInterfaceNumber in user data
  for(auto & iface : ifaceDescriptors){
    cbInterfaces->addItem(interfaceDisplayText(iface), iface.bInterfaceNumber());
  }
}

void mdtUsbtmcPortSetupDialog::updateSelectedDevice(int ifaceCbIndex)
{
  // Clear previous result
  pvSelectedDevice.clear();
  pvSelectedbInterfaceNumber = 0;
  if(ifaceCbIndex < 0){
    return;
  }
  // Get selected device descriptor
  if(cbDevices->currentIndex() < 0){
    return;
  }
  int devDescIdx = cbDevices->itemData(cbDevices->currentIndex()).toInt();
  Q_ASSERT(devDescIdx >= 0);
  Q_ASSERT(devDescIdx < pvDeviceDescriptorsList.size());
  pvSelectedDevice = pvDeviceDescriptorsList.at(devDescIdx);
  // Get bInterfaceNumber
  pvSelectedbInterfaceNumber = cbInterfaces->itemData(ifaceCbIndex).toInt();
}

void mdtUsbtmcPortSetupDialog::setStateScanning()
{
  pbRescan->setEnabled(false);
}

void mdtUsbtmcPortSetupDialog::setStateNoDeviceFound()
{
  pbRescan->setEnabled(true);
}

void mdtUsbtmcPortSetupDialog::setStateDeviceSelected()
{
  pbRescan->setEnabled(true);
}

QString mdtUsbtmcPortSetupDialog::deviceDisplayText(const mdtUsbDeviceDescriptor & descriptor) const
{
  return descriptor.idString();
}

QString mdtUsbtmcPortSetupDialog::interfaceDisplayText(const mdtUsbInterfaceDescriptor & descriptor) const
{
  QString str;

  str = QString::number(descriptor.bInterfaceNumber());
  str += " ";
  if(descriptor.bInterfaceProtocol() == 0x00){
    str += "(USBTMC)";
  }else if(descriptor.bInterfaceProtocol() == 0x01){
    str += "(USBTMC USB488)";
  }else{
    str += "(Unknown protocol)";
  }

  return str;
}

void mdtUsbtmcPortSetupDialog::displayLastError()
{
  QMessageBox msgBox(this);

  msgBox.setText(pvLastError.text());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}

// void mdtUsbtmcPortSetupDialog::displayConfig()
// {
//   // Rescan
//   if(portManager()->isClosed()){
//     rescan();
//   }
// }
// 
// void mdtUsbtmcPortSetupDialog::updateConfig()
// {
// }
// 
// void mdtUsbtmcPortSetupDialog::closePort()
// {
//   portManager()->stop();
// }
// 
// void mdtUsbtmcPortSetupDialog::rescan()
// {
//   int index;
//   QVariant var;
// 
//   if(!portManager()->isClosed()){
//     showStatusMessage(tr("Cannot rescan because port is open"), 3000);
//     return;
//   }
//   pbRescan->setEnabled(false);
//   pvPortInfoCbHandler.fillComboBoxes(portManager()->scan());
//   // Display current port
//   var.setValue(portManager()->portInfo());
//   index = cbPort->findData(var);
//   if(index >= 0){
//     cbPort->setCurrentIndex(index);
//   }
//   pbRescan->setEnabled(true);
// }
// 
// void mdtUsbtmcPortSetupDialog::openPort()
// {
//   applySetup();
// }

/**
void mdtUsbtmcPortSetupDialog::portManagerSet()
{
  displayConfig();
}
*/

// void mdtUsbtmcPortSetupDialog::setStatePortClosed()
// {
//   pbOpen->setEnabled(true);
//   pbClose->setEnabled(false);
//   pbRescan->setEnabled(true);
//   cbPort->setEnabled(true);
//   cbInterface->setEnabled(true);
//   setOkButtonEnabled(true);
//   setApplyButtonEnabled(true);
//   setCancelButtonEnabled(true);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStateDisconnected()
// {
//   pbOpen->setEnabled(true);
//   pbClose->setEnabled(false);
//   pbRescan->setEnabled(true);
//   cbPort->setEnabled(true);
//   cbInterface->setEnabled(true);
//   setOkButtonEnabled(true);
//   setApplyButtonEnabled(true);
//   setCancelButtonEnabled(true);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStateConnecting()
// {
//   pbOpen->setEnabled(false);
//   pbClose->setEnabled(false);
//   pbRescan->setEnabled(false);
//   cbPort->setEnabled(false);
//   cbInterface->setEnabled(false);
//   setOkButtonEnabled(false);
//   setApplyButtonEnabled(false);
//   setCancelButtonEnabled(false);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStatePortReady()
// {
//   pbOpen->setEnabled(false);
//   pbClose->setEnabled(true);
//   pbRescan->setEnabled(false);
//   cbPort->setEnabled(false);
//   cbInterface->setEnabled(false);
//   setOkButtonEnabled(true);
//   setApplyButtonEnabled(true);
//   setCancelButtonEnabled(true);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStateReady()
// {
//   pbOpen->setEnabled(false);
//   pbClose->setEnabled(true);
//   pbRescan->setEnabled(false);
//   cbPort->setEnabled(false);
//   cbInterface->setEnabled(false);
//   setOkButtonEnabled(true);
//   setApplyButtonEnabled(true);
//   setCancelButtonEnabled(true);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStateBusy()
// {
//   pbOpen->setEnabled(false);
//   pbClose->setEnabled(false);
//   pbRescan->setEnabled(false);
//   cbPort->setEnabled(false);
//   cbInterface->setEnabled(false);
//   setOkButtonEnabled(false);
//   setApplyButtonEnabled(false);
//   setCancelButtonEnabled(false);
// }
// 
// void mdtUsbtmcPortSetupDialog::setStateError()
// {
//   pbOpen->setEnabled(false);
//   pbClose->setEnabled(true);
//   pbRescan->setEnabled(false);
//   cbPort->setEnabled(false);
//   cbInterface->setEnabled(false);
//   setOkButtonEnabled(false);
//   setApplyButtonEnabled(false);
//   setCancelButtonEnabled(true);
// }

// bool mdtUsbtmcPortSetupDialog::applySetup()
// {
//   mdtUsbtmcPortManager *usbtmcPortManager;
//   QString idn;
//   QStringList idnItems;
// 
//   if(pvPortInfoCbHandler.currentPortInfo().portName().isEmpty()){
//     showStatusMessage(tr("Please choose a device in list"), 3000);
//     return false;
//   }
//   // We must close the port
//   portManager()->stop();
//   // Apply current configuration
//   updateConfig();
//   portManager()->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
//   // Try to open port and start port manager
//   if(!portManager()->start()){
//     showStatusMessage(tr("Cannot open port"), 3000);
//     return false;
//   }
//   // Get identification
//   usbtmcPortManager = dynamic_cast<mdtUsbtmcPortManager*>(portManager());
//   Q_ASSERT(usbtmcPortManager != 0);
//   idn = usbtmcPortManager->sendQuery("*IDN?\n");
//   if(idn.isEmpty()){
//     usbtmcPortManager->stop();
//     showStatusMessage(tr("Cannot get device infos (*IDN? query failed)"), 3000);
//     return false;
//   }
//   idn = idn.trimmed();
//   idnItems = idn.split(',');
//   if(idnItems.size() != 4){
//     usbtmcPortManager->stop();
//     showStatusMessage(tr("Cannot get device infos (*IDN? query returned unexpected data)"), 3000);
//     return false;
//   }
//   // Display infos
//   lbManufacturer->setText(idnItems.at(0));
//   lbModel->setText(idnItems.at(1));
//   lbSerial->setText(idnItems.at(2));
//   lbFirmware->setText(idnItems.at(3));
// 
//   return true;
// }
