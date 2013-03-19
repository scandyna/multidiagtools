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
#ifndef MDT_MODBUS_TCP_PORT_SETUP_DIALOG_H
#define MDT_MODBUS_TCP_PORT_SETUP_DIALOG_H

#include "mdtPortInfoCbHandler.h"
#include "mdtPortInfo.h"
#include "mdtAbstractPortSetupDialog.h"
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class mdtModbusTcpPortManager;
class QWidget;
class QAbstractButton;
class QLineEdit;
class QSpinBox;

class mdtModbusTcpPortSetupDialog : public mdtAbstractPortSetupDialog
{
 Q_OBJECT

 public:

  mdtModbusTcpPortSetupDialog(QWidget *parent = 0);
  ~mdtModbusTcpPortSetupDialog();

 private slots:

  // Read the configuration and update GUI
  void displayConfig();

  // Display host and port in lineEdit and spinBox
  void displayHostPort(int cbDevicesIndex);

  // Read options from GUI and update configuration
  void updateConfig();

  // Close port
  void closePort();

  // Rescan
  void rescan();

  // Open port
  void openPort();

 private:

  /*! \brief Called by setPortManager()
   */
  void portManagerSet();

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

  /*! \brief Abort scan on close event
   */
  void closeEvent(QCloseEvent *event);

  /*! \brief Clear device informations
   */
  void clearDeviceInformations();

  /*! \brief Get device informations
   */
  void getDeviceInformations();

  /*! \brief Try to get Wago 750 informations
   */
  bool getWago750Informations();

  /*! \brief Try to get Beckhoff BC/NK informations
   */
  bool getBeckhoffBcInformations();

  /*! \brief Get HW node address
   */
  void getHardwareNodeAddress();

  ///mdtPortConfigWidget *pvPortConfigWidget;
  mdtModbusTcpPortManager *pvModbusTcpPortManager;
  mdtPortInfoCbHandler pvPortInfoCbHandler;
  mdtPortInfo pvCurrentPortInfo;
  QPushButton *pbConnect;
  QPushButton *pbDisconnect;
  QPushButton *pbRescan;
  QPushButton *pbAbort;
  QComboBox *cbDevices;
  QLabel *lbHost;
  QLineEdit *leHost;
  QLabel *lbPort;
  QSpinBox *sbPort;
  QWidget *pvDeviceInfoWidget;
  QLabel *lbManufacturer;
  QLabel *lbModel;
  QLabel *lbHwRevision;
  QLabel *lbFwRevision;
  QLabel *lbIOs;
  QLabel *lbHwNodeAddress;
};

#endif  // #ifndef MDT_MODBUS_TCP_PORT_SETUP_DIALOG_H
