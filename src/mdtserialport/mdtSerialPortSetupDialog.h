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
#ifndef MDT_SERIAL_PORT_SETUP_DIALOG_H
#define MDT_SERIAL_PORT_SETUP_DIALOG_H

///#include "ui_mdtSerialPortSetupDialog.h"
#include "mdtSerialPortManager.h"
#include "mdtPortConfigWidget.h"
#include "mdtSerialPortConfigWidget.h"
#include "mdtPortInfoCbHandler.h"
#include <QDialog>
///#include <QAbstractButton>
#include <QString>
///#include <QCloseEvent>

#include "mdtAbstractPortSetupDialog.h"

class QPushButton;

/*! \brief Setup dialog for serial port
 * 
 * The first step before display this dialog is to set the port manager
 * with setPortManager()
 * 
 * \image html mdtSerialPortSetupDialog.png
 */
///class mdtSerialPortSetupDialog : public QDialog, public Ui::mdtSerialPortSetupDialog
class mdtSerialPortSetupDialog : public mdtAbstractPortSetupDialog
{
 Q_OBJECT

 public:

  mdtSerialPortSetupDialog(QWidget *parent = 0);

  /*! \brief Set the serial port manager instance
   * \pre manager must be a valid pointer
   */
  void setPortManager(mdtSerialPortManager *manager);

 private slots:

  // Read the configuration and update GUI
  void displayConfig();

  // Read options from GUI and update configuration
  void updateConfig();

  // Tell the manager to re-open with new configuration
  ///void applySetup();

  // Called when setup is accepted from user
  ///void on_buttonBox_clicked(QAbstractButton *button);

  // Rescan for available ports
  ///void on_pbRescan_clicked();

  // Open selected port (if manager was set)
  ///void on_cbPort_currentIndexChanged(int index);

  // Close port
  void closePort();

  // Rescan
  void rescan();

  // Open port
  void openPort();

 private:

  /*! \brief Set the Disconnected state
   */
  void setStateDisconnected();

  /*! \brief Set the Connecting state
   */
  void setStateConnecting();

  /*! \brief Set the Ready state
   */
  void setStateReady();

  /*! \brief Set the Busy state
   */
  void setStateBusy();

  /*! \brief Set the Warning state
   */
  void setStateWarning();

  /*! \brief Set the Error state
   */
  void setStateError();

  /*! \brief Apply setup
   */
  bool applySetup();

  // Set the running state
  ///void setStateRunning();

  // Set the stopped state
  ///void setStateStopped();

  // Set the Error state
  ///void setStateError(QString msg = tr("Error"));

  // Enable/diseable the apply buttons (Ok, Apply)
  ///void diseableApplyButtons();
  ///void enableApplyButtons();

  mdtSerialPortManager *pvSerialPortManager;
  mdtPortConfigWidget *pvPortConfigWidget;
  mdtSerialPortConfigWidget *pvSerialPortConfigWidget;
  mdtPortInfoCbHandler pvPortInfoCbHandler;
  QPushButton *pbOpen;
  QPushButton *pbClose;
  QPushButton *pbRescan;
  QComboBox *cbPort;
};

#endif  // #ifndef MDT_SERIAL_PORT_SETUP_DIALOG_H
