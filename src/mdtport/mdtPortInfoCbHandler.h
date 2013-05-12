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
#ifndef MDT_PORT_INFO_CB_HANDLER_H
#define MDT_PORT_INFO_CB_HANDLER_H

#include <QObject>
#include <QList>

class QComboBox;
class mdtPortInfo;
class mdtDeviceInfo;

/*! \brief Handle the port info comboBox and the device info comboBox
 *
 * After a port scan was done (f.ex. with mdtPortManager::scan() ),
 *  it is useful to display the result into one or two comboboxes.
 *
 * The first combobox will then display a list of available
 *  port (for.ex. /dev/ttyS0, /dev/ttyS1, ... for serial port,
 *  or attached devices list on USB port, etc...).
 *
 * The second comboBox is not allways useful.
 *  It can be used, f.ex. with USB interfaces.
 * If a specific port manager is implemented, it gives
 *  a possibility to list devices attached on a specific
 *  RS485 port.
 */
class mdtPortInfoCbHandler : public QObject
{
 Q_OBJECT

 public:

  mdtPortInfoCbHandler(QObject *parent = 0);
  ~mdtPortInfoCbHandler();

  /*! \brief Set the comboBox that must display ports
   *
   * Note that comboBox object will not be deleted by this clas.
   *
   * \pre comboBox must be a valid object.
   */
  void setPortsComboBox(QComboBox *comboBox);

  /*! \brief Set the comboBox that must display devices
   *
   * Note that comboBox object will not be deleted by this clas.
   *
   * \pre comboBox must be a valid object.
   * \pre Ports combobox must be set before with setPortsComboBox()
   */
  void setDevicesComboBox(QComboBox *comboBox);

  /*! \brief Fill the comboboxes with a scan result.
   *
   * All ports in list ares added to the ports combobox.
   *  If the devices combobox was set, it will be updated
   *  at each ports combobox index change.
   */
  void fillComboBoxes(const QList<mdtPortInfo*> &portInfoList);

  /*! \brief Add a single port info.
   *
   * Will act similary to fillComboBoxes().
   */
  void addPortInfo(const mdtPortInfo &portInfo);

  /*! \brief Get the port info object at given index.
   *
   * Can return a empty object if nothing was set,
   *  or index is out of range.
   */
  mdtPortInfo portInfoAt(int index);

  /*! \brief Get the selected port info object.
   *
   * Can return a empty object if nothing was set,
   *  or nothing valid is selected.
   */
  mdtPortInfo currentPortInfo();

  /*! \brief Get index of a given port info.
   *
   * Will return the index of first port info that match.
   * If nothing was found, -1 is returned.
   */
  int indexOfPortInfo(const mdtPortInfo &portInfo) const;

  /*! \brief Get the selected device info object.
   *
   * Can return a empty object if nothing was set,
   *  or nothing valid is selected
   *  or device combobox was not set.
   */
  mdtDeviceInfo currentDeviceInfo();

 private slots:

  /*! \brief Update the devices combobox with current port's devices
   */
  void updateDevicesCb(int portsCbIndex);

 private:

  QComboBox *cbPorts;
  QComboBox *cbDevices;
};

#endif  // #ifndef MDT_PORT_INFO_CB_HANDLER_H
