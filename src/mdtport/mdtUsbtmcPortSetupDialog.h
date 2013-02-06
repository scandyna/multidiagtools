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
#ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
#define MDT_USBTMC_PORT_SETUP_DIALOG_H

#include "ui_mdtUsbtmcPortSetupDialog.h"
#include "mdtPortConfigWidget.h"
#include "mdtPortInfoCbHandler.h"
#include "mdtPortInfo.h"
#include <QDialog>

class mdtUsbtmcPortManager;
class QWidget;
class QAbstractButton;

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

  // Rescan for available ports with attached USBTMC device
  void on_pbRescan_clicked();

  // Open selected port (if manager was set)
  void on_cbInterface_currentIndexChanged(int index);

  // Called when setup is accepted/canceled from user
  void on_buttonBox_clicked(QAbstractButton *button);

 private:

  // Set the running state
  void setStateRunning();

  // Set the stopped state
  void setStateStopped();

  // Set the Error state
  void setStateError(QString msg = tr("Error"));

  // Enable/diseable the apply buttons (Ok, Apply)
  void diseableApplyButtons();
  void enableApplyButtons();

  mdtUsbtmcPortManager * pvPortManager;
  mdtPortConfigWidget pvPortConfigWidget;
  mdtPortInfoCbHandler pvPortInfoCbHandler;
  mdtPortInfo pvCurrentPortInfo;
};

#endif  // #ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
