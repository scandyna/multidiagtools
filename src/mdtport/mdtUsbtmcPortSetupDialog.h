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
#ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
#define MDT_USBTMC_PORT_SETUP_DIALOG_H

#include "ui_mdtUsbtmcPortSetupDialog.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbtmcPort.h"
#include "mdtError.h"
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QList>

class QWidget;
class QAbstractButton;

/*! \brief Dialog for USBTMC devices enumeration and selection
 */
class mdtUsbtmcPortSetupDialog : public QDialog, Ui::mdtUsbtmcPortSetupDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtUsbtmcPortSetupDialog(QWidget *parent = 0);

  /*! \brief Desctructor
   */
  ~mdtUsbtmcPortSetupDialog();

  /*! \brief Enumerate attached USBTMC devices
   */
  void scan()
  {
    rescan();
  }

  /*! \brief Get selected device
   *
   * Returns a empty device descriptor if no device was selected or found.
   */
  mdtUsbDeviceDescriptor selectedDevice() const
  {
    return pvSelectedDevice;
  }

  /*! \brief Get selected bInterfaceNumber
   */
  uint8_t selectedbInterfaceNumber() const
  {
    return pvSelectedbInterfaceNumber;
  }

 private slots:

  /*! \brief Enumerate attached USBTMC devices
   */
  void rescan();

  /*! \brief Build interfaces list
   */
  void buildInterfacesList(int deviceCbIndex);

  /*! \brief Update selected device and interface
   */
  void updateSelectedDevice(int ifaceCbIndex);

 private:

  /*! \brief Display device informations
   */
  void displayDeviceInformations();

  /*! \brief Set scanning state
   */
  void setStateScanning();

  /*! \brief Set no device found state
   */
  void setStateNoDeviceFound();

  /*! \brief Set device selected state
   */
  void setStateDeviceSelected();

  /*! \brief Get text to display in list devices from descriptor
   */
  QString deviceDisplayText(const mdtUsbDeviceDescriptor & descriptor) const;

  /*! \brief Get text to display in interfaces list from descriptor
   */
  QString interfaceDisplayText(const mdtUsbInterfaceDescriptor & descriptor) const;

  /*! \brief Display last error to the user
   */
  void displayLastError();

  Q_DISABLE_COPY(mdtUsbtmcPortSetupDialog);

  mdtUsbtmcPort pvPort;
  QList<mdtUsbDeviceDescriptor> pvDeviceDescriptorsList;
  mdtUsbDeviceDescriptor pvSelectedDevice;
  uint8_t pvSelectedbInterfaceNumber;
  mdtError pvLastError;
  bool pvScanning;
};

#endif  // #ifndef MDT_USBTMC_PORT_SETUP_DIALOG_H
