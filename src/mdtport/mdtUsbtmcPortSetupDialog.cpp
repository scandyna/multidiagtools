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

mdtUsbtmcPortSetupDialog::mdtUsbtmcPortSetupDialog(QWidget *parent)
 : QDialog(parent)
{
  pvPortManager = 0;
  ///pvPortConfigWidget = new mdtPortConfigWidget;
  // Setup the dialog ui
  setupUi(this);

  // Add bus/device list combobox to handler handler
  pvPortInfoCbHandler.setPortsComboBox(cbBusDevice);
  pvPortInfoCbHandler.setDevicesComboBox(cbInterface);
}

mdtUsbtmcPortSetupDialog::~mdtUsbtmcPortSetupDialog()
{
}

void mdtUsbtmcPortSetupDialog::setPortManager(mdtUsbtmcPortManager *manager)
{
  Q_ASSERT(manager != 0);

  pvPortManager = manager;
}

void mdtUsbtmcPortSetupDialog::displayConfig()
{
}

void mdtUsbtmcPortSetupDialog::updateConfig()
{
}

void mdtUsbtmcPortSetupDialog::applySetup()
{
}

void mdtUsbtmcPortSetupDialog::diseableApplyButtons()
{
}

void mdtUsbtmcPortSetupDialog::enableApplyButtons()
{
}

void mdtUsbtmcPortSetupDialog::on_pbRescan_clicked()
{
  Q_ASSERT(pvPortManager != 0);

  // Stop manager and close port
  lbState->setText(tr("Stopping ..."));
  pvPortManager->closePort();
  ///setStateStopped();
  pbRescan->setEnabled(false);
  pvPortInfoCbHandler.fillComboBoxes(pvPortManager->scan());
  pbRescan->setEnabled(true);
}

void mdtUsbtmcPortSetupDialog::on_cbPort_currentIndexChanged(int index)
{
  
}

void mdtUsbtmcPortSetupDialog::setStateRunning()
{
}

void mdtUsbtmcPortSetupDialog::setStateError()
{
}



