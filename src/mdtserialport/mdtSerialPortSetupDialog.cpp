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
}

void mdtSerialPortSetupDialog::setPortManager(mdtSerialPortManager *manager)
{
  Q_ASSERT(manager != 0);

  pvPortManager = manager;
  // Close port
  pvPortManager->closePort();
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
    updateConfig();
    // Open the port
    if(!pvPortManager->setPortName(cbPort->currentText())){
      qDebug() << "ERR set port name !";
      return;
    }
    if(!pvPortManager->openPort()){
      qDebug() << "ERR open port !";
      return;
    }
    // Start R/W
    if(!pvPortManager->start()){
      qDebug() << "ERR start !";
      return;
    }
    /// NOTE: retsart etc...
  }
}

void mdtSerialPortSetupDialog::on_pbRescan_clicked()
{
  Q_ASSERT(pvPortManager != 0);

  cbPort->clear();
  pbRescan->setEnabled(false);
  cbPort->addItems(pvPortManager->scan());
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
  // Open the port
  if(!pvPortManager->setPortName(cbPort->currentText())){
    qDebug() << "ERRRRRR Open !";
    return;
  }
  // List available baud rates
  pvSerialPortConfigWidget->setAvailableBaudRates(pvPortManager->port().availableBaudRates());
  // Display configuration
  displayConfig();
}
