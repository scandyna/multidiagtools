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
#include "mdtSerialPortSetupDialog.h"
#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVariant>

//#include <QDebug>

mdtSerialPortSetupDialog::mdtSerialPortSetupDialog(QWidget *parent)
 : mdtAbstractPortSetupDialog(parent)
{
  pvSerialPortManager = 0;
  pvPortConfigWidget = new mdtPortConfigWidget;
  pvSerialPortConfigWidget = new mdtSerialPortConfigWidget;
  // Setup the dialog's header part
  pbOpen = new QPushButton(tr("&Open"));
  pbClose = new QPushButton(tr("&Close"));
  pbRescan = new QPushButton(tr("Rescan"));
  cbPort = new QComboBox;
  cbPort->setMinimumWidth(150);
  QWidget *wHeader = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(pbOpen);
  layout->addWidget(pbClose);
  layout->addWidget(pbRescan);
  layout->addWidget(cbPort);
  layout->addStretch();
  wHeader->setLayout(layout);
  setHeaderWidget(wHeader);
  connect(pbClose, SIGNAL(clicked()), this, SLOT(closePort()));
  connect(pbRescan, SIGNAL(clicked()), this, SLOT(rescan()));
  connect(pbOpen, SIGNAL(clicked()), this, SLOT(openPort()));
  // Add serial port options widget
  addOptionsTabWidget(pvSerialPortConfigWidget, tr("Serial port options"));
  // Add port options widget
  addOptionsTabWidget(pvPortConfigWidget, tr("Advanced port options"));
  // Have a problem with widgets in TabWidget, so fix a minimum size
  setMinimumHeight(400);
  // Set a correct default size
  resize(550, 400);
  // Add ports list combobox to handler
  pvPortInfoCbHandler.setPortsComboBox(cbPort);
}

void mdtSerialPortSetupDialog::setPortManager(mdtSerialPortManager *manager)
{
  Q_ASSERT(manager != 0);

  pvSerialPortManager = manager;
  mdtAbstractPortSetupDialog::setPortManager(manager);
  // Update GUI
  displayConfig();
}

void mdtSerialPortSetupDialog::displayConfig()
{
  Q_ASSERT(pvSerialPortManager != 0);

  int index;
  QVariant var;

  // Display current port
  var.setValue(portManager()->portInfo());
  index = cbPort->findData(var);
  if(index >= 0){
    cbPort->setCurrentIndex(index);
  }else{
    cbPort->addItem(portManager()->portInfo().displayText(), var);
    index = cbPort->findData(var);
    cbPort->setCurrentIndex(index);
  }
  // Get available baud rates. Check if port manager is running first
  if(pvSerialPortManager->isRunning()){
    pvSerialPortManager->port().lockMutex();
    pvSerialPortConfigWidget->setAvailableBaudRates(pvSerialPortManager->port().availableBaudRates());
    pvSerialPortManager->port().unlockMutex();
  }else{
    if(pvSerialPortManager->port().open() != mdtAbstractPort::NoError){
      showStatusMessage(tr("Cannot get baud rates"), 3000);
      return;
    }
    pvSerialPortConfigWidget->setAvailableBaudRates(pvSerialPortManager->port().availableBaudRates());
    pvSerialPortManager->port().close();
  }
  // Display current configuration
  pvPortConfigWidget->displayConfig(pvSerialPortManager->config());
  pvSerialPortConfigWidget->displayConfig(pvSerialPortManager->config());
}

void mdtSerialPortSetupDialog::updateConfig()
{
  Q_ASSERT(pvSerialPortManager != 0);

  // Port config section
  pvPortConfigWidget->updateConfig(pvSerialPortManager->config());
  // Serial port section
  pvSerialPortConfigWidget->updateConfig(pvSerialPortManager->config());
}

void mdtSerialPortSetupDialog::closePort()
{
  Q_ASSERT(pvSerialPortManager != 0);

  pvSerialPortManager->closePort();
}

void mdtSerialPortSetupDialog::rescan()
{
  if(portManager()->isRunning()){
    showStatusMessage(tr("Cannot rescan because port is open"), 3000);
    return;
  }
  pbRescan->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(pvSerialPortManager->scan());
  pbRescan->setEnabled(true);
}

void mdtSerialPortSetupDialog::openPort()
{
  applySetup();
}

void mdtSerialPortSetupDialog::setStateDisconnected()
{
  pbOpen->setEnabled(true);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(true);
  cbPort->setEnabled(true);
  pvSerialPortConfigWidget->setEnabled(true);
  pvPortConfigWidget->setEnabled(true);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtSerialPortSetupDialog::setStateConnecting()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  pvSerialPortConfigWidget->setEnabled(false);
  pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtSerialPortSetupDialog::setStateReady()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  pvSerialPortConfigWidget->setEnabled(false);
  pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(true);
  setApplyButtonEnabled(true);
  setCancelButtonEnabled(true);
}

void mdtSerialPortSetupDialog::setStateBusy()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(false);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  pvSerialPortConfigWidget->setEnabled(false);
  pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(false);
}

void mdtSerialPortSetupDialog::setStateWarning()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  pvSerialPortConfigWidget->setEnabled(false);
  pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

void mdtSerialPortSetupDialog::setStateError()
{
  pbOpen->setEnabled(false);
  pbClose->setEnabled(true);
  pbRescan->setEnabled(false);
  cbPort->setEnabled(false);
  pvSerialPortConfigWidget->setEnabled(false);
  pvPortConfigWidget->setEnabled(false);
  setOkButtonEnabled(false);
  setApplyButtonEnabled(false);
  setCancelButtonEnabled(true);
}

bool mdtSerialPortSetupDialog::applySetup()
{
  Q_ASSERT(pvSerialPortManager != 0);

  // We must close the port
  pvSerialPortManager->closePort();
  // Apply current configuration
  updateConfig();
  pvSerialPortManager->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  // Try to open port and start port manager
  if(!pvSerialPortManager->openPort()){
    showStatusMessage(tr("Cannot open port"), 3000);
    return false;
  }
  if(!pvSerialPortManager->start()){
    showStatusMessage(tr("Cannot start threads"), 3000);
    return false;
  }

  return true;
}
