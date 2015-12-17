/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtCodecScpi.h"
#include "mdtErrorDialog.h"
#include <QWidget>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QByteArray>

//#include <QDebug>

mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  cbDevices->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  cbInterfaces->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(cbDevices, SIGNAL(currentIndexChanged(int)), this, SLOT(buildInterfacesList(int)));
  connect(cbInterfaces, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSelectedDevice(int)));
  setStateNoDeviceFound();
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
    return;
  }
  // We have at least 1 device here
  displayDeviceInformations();
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
  // Display device informations
  if(!pvScanning){
    displayDeviceInformations();
  }
}

void mdtUsbtmcPortSetupDialog::displayDeviceInformations()
{
  mdtCodecScpi codec;
  mdtScpiIdnResponse idnResponse;
  QByteArray data;
  // Clear previous resulsts
  lbManufacturer->clear();
  lbModel->clear();
  lbSerial->clear();
  lbFirmware->clear();
  setStateScanning();
  // Open device
  if(pvSelectedDevice.isEmpty()){
    setStateNoDeviceFound();
    return;
  }
  if(!pvPort.openDevice(pvSelectedDevice, pvSelectedbInterfaceNumber, false)){
    pvLastError = pvPort.lastError();
    displayLastError();
    setStateNoDeviceFound();
    return;
  }
  // Get device iformation and display them
  data = pvPort.sendQuery("*IDN?\n");
  if(data.isEmpty()){
    pvLastError = pvPort.lastError();
    pvPort.close();
    displayLastError();
    setStateNoDeviceFound();
    return;
  }
  idnResponse = codec.decodeIdn(data);
  pvPort.close();
  lbManufacturer->setText(idnResponse.manufacturer);
  lbModel->setText(idnResponse.model);
  lbSerial->setText(idnResponse.serial);
  lbFirmware->setText(idnResponse.firmwareLevel);
  setStateDeviceSelected();
}

void mdtUsbtmcPortSetupDialog::setStateScanning()
{
  QPushButton *pbOk = buttonBox->button(QDialogButtonBox::Ok);
  QPushButton *pbCancel = buttonBox->button(QDialogButtonBox::Cancel);
  Q_ASSERT(pbOk != 0);
  Q_ASSERT(pbCancel != 0);

  pbRescan->setEnabled(false);
  pvScanning = true;
  pbOk->setEnabled(false);
  pbCancel->setEnabled(false);
}

void mdtUsbtmcPortSetupDialog::setStateNoDeviceFound()
{
  QPushButton *pbOk = buttonBox->button(QDialogButtonBox::Ok);
  QPushButton *pbCancel = buttonBox->button(QDialogButtonBox::Cancel);
  Q_ASSERT(pbOk != 0);
  Q_ASSERT(pbCancel != 0);

  pbRescan->setEnabled(true);
  pvScanning = false;
  pbOk->setEnabled(false);
  pbCancel->setEnabled(true);
}

void mdtUsbtmcPortSetupDialog::setStateDeviceSelected()
{
  QPushButton *pbOk = buttonBox->button(QDialogButtonBox::Ok);
  QPushButton *pbCancel = buttonBox->button(QDialogButtonBox::Cancel);
  Q_ASSERT(pbOk != 0);
  Q_ASSERT(pbCancel != 0);

  pbRescan->setEnabled(true);
  pvScanning = false;
  pbOk->setEnabled(true);
  pbCancel->setEnabled(true);
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
  mdtErrorDialog dialog(this);
  dialog.exec();
}
