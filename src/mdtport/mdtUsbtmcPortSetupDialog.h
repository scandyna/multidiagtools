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
#ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
#define MDT_USBTMC_PORT_SETUP_DIALOG_H

#include "ui_mdtUsbtmcPortSetupDialog.h"
#include "mdtPortConfigWidget.h"
#include "mdtPortInfoCbHandler.h"
#include <QDialog>

class mdtUsbtmcPortManager;
class QWidget;

class mdtUsbtmcPortSetupDialog : public QDialog, Ui_mdtUsbtmcPortSetupDialog
{
 Q_OBJECT

 public:

  mdtUsbtmcPortSetupDialog(QWidget *parent = 0);
  ~mdtUsbtmcPortSetupDialog();

  void setPortManager(mdtUsbtmcPortManager *manager);

 private slots:

  // Read the configuration and update GUI
  void displayConfig();

  // Read options from GUI and update configuration
  void updateConfig();

  // Tell the manager to re-open with new configuration
  void applySetup();

  // Enable/diseable the apply buttons (Ok, Apply)
  void diseableApplyButtons();
  void enableApplyButtons();

  // Rescan for available ports with attached USBTMC device
  void on_pbRescan_clicked();

  // Open selected port (if manager was set)
  void on_cbPort_currentIndexChanged(int index);

 private:

  void setStateRunning();

  void setStateError();

  mdtUsbtmcPortManager * pvPortManager;
  mdtPortConfigWidget pvPortConfigWidget;
  mdtPortInfoCbHandler pvPortInfoCbHandler;
};

#endif  // #ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
