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
#include "mdtSerialPortSetupDialog.h"
#include <QWidget>
#include <QDialogButtonBox>
#include <QPushButton>

#include <QDebug>

mdtSerialPortSetupDialog::mdtSerialPortSetupDialog(QWidget *parent)
 : QDialog(parent)
{
  pvPortManager = 0;
  pvPortConfigWidget = new mdtPortConfigWidget;
  pvSerialPortConfigWidget = new mdtSerialPortConfigWidget;
  // Setup the dialog ui
  setupUi(this);
  // Add serial port options widget
  if(twOptions->count() >= 1){
    twOptions->removeTab(0);
  }
  twOptions->insertTab(1, pvSerialPortConfigWidget, tr("Serial port options"));
  // Add port options widget
  if(twOptions->count() >= 2){
    twOptions->removeTab(1);
  }
  twOptions->insertTab(1, pvPortConfigWidget, tr("Advanced port options"));
  // Add ports list combobox to handler handler
  pvPortInfoCbHandler.setPortsComboBox(cbPort);
}

void mdtSerialPortSetupDialog::setPortManager(mdtSerialPortManager *manager)
{
  Q_ASSERT(manager != 0);

  pvPortManager = manager;

  // Stop manager and close port
  lbState->setText(tr("Stopping ..."));
  pvPortManager->closePort();
  setStateStopped();
  // List available ports
  on_pbRescan_clicked();
  // Update GUI
  displayConfig();
}

void mdtSerialPortSetupDialog::displayConfig()
{
  Q_ASSERT(pvPortManager != 0);

  // Port config section
  pvPortConfigWidget->displayConfig(pvPortManager->config());
  pvSerialPortConfigWidget->displayConfig(pvPortManager->config());
}

void mdtSerialPortSetupDialog::updateConfig()
{
  Q_ASSERT(pvPortManager != 0);

  // Port config section
  pvPortConfigWidget->updateConfig(pvPortManager->config());
  // Serial port section
  pvSerialPortConfigWidget->updateConfig(pvPortManager->config());
}

/// \todo Is this method used ??
void mdtSerialPortSetupDialog::applySetup()
{
  Q_ASSERT(pvPortManager != 0);
}

void mdtSerialPortSetupDialog::on_buttonBox_clicked(QAbstractButton *button)
{
  Q_ASSERT(pvPortManager != 0);
  Q_ASSERT(button != 0);

  QDialogButtonBox::StandardButton type;

  type = buttonBox->standardButton(button);
  if((type == QDialogButtonBox::Apply)||(type == QDialogButtonBox::Ok)){
    diseableApplyButtons();
    // Close port
    lbState->setText(tr("Stopping ..."));
    pvPortManager->closePort();
    // Get current config
    updateConfig();
    // Open the port
    pvPortManager->setPortName(cbPort->currentText());
    if(!pvPortManager->openPort()){
      setStateError(tr("Cannot open port"));
      enableApplyButtons();
      return;
    }
    // Start R/W
    lbState->setText(tr("Starting ..."));
    if(!pvPortManager->start()){
      setStateError(tr("Cannot start threads"));
      enableApplyButtons();
      return;
    }
    setStateRunning();
    enableApplyButtons();
  }
}

void mdtSerialPortSetupDialog::on_pbRescan_clicked()
{
  Q_ASSERT(pvPortManager != 0);

  pbRescan->setEnabled(false);
  //cbPort->clear();
  //cbPort->addItems(pvPortManager->scan());
  pvPortInfoCbHandler.fillComboBoxes(pvPortManager->scan());
  pbRescan->setEnabled(true);
}

void mdtSerialPortSetupDialog::on_cbPort_currentIndexChanged(int index)
{
  if(index < 0){
    return;
  }
  if(pvPortManager == 0){
    return;
  }
  cbPort->setEnabled(false);
  // Close port
  pvPortManager->closePort();
  setStateStopped();
  // Open the port
  //pvPortManager->setPortName(cbPort->currentText());
  pvPortManager->setPortName(pvPortInfoCbHandler.currentPortInfo().portName());
  if(!pvPortManager->openPort()){
    setStateError(tr("Cannot fetch port attributes"));
    cbPort->setEnabled(true);
    return;
  }
  // List available baud rates
  pvSerialPortConfigWidget->setAvailableBaudRates(pvPortManager->port().availableBaudRates());
  // Close port
  pvPortManager->closePort();
  // Display configuration
  displayConfig();
  cbPort->setEnabled(true);
}

void mdtSerialPortSetupDialog::setStateRunning()
{
  // Update state label
  lbState->setText(tr("Running"));
  lbState->setStyleSheet("QLabel { background-color : green; color : black; }");
}

void mdtSerialPortSetupDialog::setStateStopped()
{
  // Update state label
  lbState->setText(tr("Stopped"));
  lbState->setStyleSheet("QLabel { background-color : orange; color : black; }");
}

void mdtSerialPortSetupDialog::setStateError(QString msg)
{
  // Update state label
  lbState->setText(msg);
  lbState->setStyleSheet("QLabel { background-color : red; color : black; }");
}

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
