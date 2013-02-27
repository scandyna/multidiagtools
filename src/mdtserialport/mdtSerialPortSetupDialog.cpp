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
///#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVariant>

#include <QDebug>

mdtSerialPortSetupDialog::mdtSerialPortSetupDialog(QWidget *parent)
/// : QDialog(parent)
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
  
  ///setupUi(this);
  // Add serial port options widget
  addOptionsTabWidget(pvSerialPortConfigWidget, tr("Serial port options"));
  /**
  if(twOptions->count() >= 1){
    twOptions->removeTab(0);
  }
  twOptions->insertTab(1, pvSerialPortConfigWidget, tr("Serial port options"));
  */
  // Add port options widget
  addOptionsTabWidget(pvPortConfigWidget, tr("Advanced port options"));
  /**
  if(twOptions->count() >= 2){
    twOptions->removeTab(1);
  }
  twOptions->insertTab(1, pvPortConfigWidget, tr("Advanced port options"));
  */
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

  /**
  // Stop manager and close port
  lbState->setText(tr("Stopping ..."));
  pvSerialPortManager->closePort();
  setStateStopped();
  // List available ports
  on_pbRescan_clicked();
  // Update GUI
  displayConfig();
  */
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

/// \todo Is this method used ??
/**
void mdtSerialPortSetupDialog::applySetup()
{
  Q_ASSERT(pvSerialPortManager != 0);
}
*/

/**
void mdtSerialPortSetupDialog::on_buttonBox_clicked(QAbstractButton *button)
{
  Q_ASSERT(pvSerialPortManager != 0);
  Q_ASSERT(button != 0);

  QDialogButtonBox::StandardButton type;

  type = buttonBox->standardButton(button);
  if((type == QDialogButtonBox::Apply)||(type == QDialogButtonBox::Ok)){
    diseableApplyButtons();
    // Close port
    lbState->setText(tr("Stopping ..."));
    pvSerialPortManager->closePort();
    // Get current config
    updateConfig();
    // Open the port
    /// \bug Should not use display text as port name !
    pvSerialPortManager->setPortName(cbPort->currentText());
    if(!pvSerialPortManager->openPort()){
      setStateError(tr("Cannot open port"));
      enableApplyButtons();
      return;
    }
    // Start R/W
    lbState->setText(tr("Starting ..."));
    if(!pvSerialPortManager->start()){
      setStateError(tr("Cannot start threads"));
      enableApplyButtons();
      return;
    }
    setStateRunning();
    enableApplyButtons();
  }
}
*/

/**
void mdtSerialPortSetupDialog::on_pbRescan_clicked()
{
  Q_ASSERT(pvSerialPortManager != 0);

  ///pbRescan->setEnabled(false);
  //cbPort->clear();
  //cbPort->addItems(pvSerialPortManager->scan());
  pvPortInfoCbHandler.fillComboBoxes(pvSerialPortManager->scan());
  ///pbRescan->setEnabled(true);
}
*/

/**
void mdtSerialPortSetupDialog::on_cbPort_currentIndexChanged(int index)
{
  if(index < 0){
    return;
  }
  if(pvSerialPortManager == 0){
    return;
  }
  ///cbPort->setEnabled(false);
  // Close port
  pvSerialPortManager->closePort();
  ///setStateStopped();
  // Open the port
  ///pvSerialPortManager->setPortName(pvPortInfoCbHandler.currentPortInfo().portName());
  pvSerialPortManager->setPortInfo(pvPortInfoCbHandler.currentPortInfo());
  if(pvSerialPortManager->port().open() != mdtAbstractPort::NoError){
    ///setStateError(tr("Cannot fetch port attributes"));
    ///cbPort->setEnabled(true);
    return;
  }
  // List available baud rates
  pvSerialPortConfigWidget->setAvailableBaudRates(pvSerialPortManager->port().availableBaudRates());
  // Close port
  pvSerialPortManager->closePort();
  // Display configuration
  displayConfig();
  ///cbPort->setEnabled(true);
}
*/

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
  if(!applySetup()){
    
  }
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

/**
void mdtSerialPortSetupDialog::setStateRunning()
{
  // Update state label
  lbState->setText(tr("Running"));
  lbState->setStyleSheet("QLabel { background-color : green; color : black; }");
}
*/

/**
void mdtSerialPortSetupDialog::setStateStopped()
{
  // Update state label
  lbState->setText(tr("Stopped"));
  lbState->setStyleSheet("QLabel { background-color : orange; color : black; }");
}
*/

/**
void mdtSerialPortSetupDialog::setStateError(QString msg)
{
  // Update state label
  lbState->setText(msg);
  lbState->setStyleSheet("QLabel { background-color : red; color : black; }");
}
*/

/**
void mdtSerialPortSetupDialog::diseableApplyButtons()
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
void mdtSerialPortSetupDialog::enableApplyButtons()
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
