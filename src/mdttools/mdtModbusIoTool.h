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
#ifndef MDT_MODBUS_IO_TOOL_H
#define MDT_MODBUS_IO_TOOL_H

#include "ui_mdtModbusIoTool.h"
#include "mdtDevice.h"
#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QActionGroup>
#include <memory>

class mdtDeviceModbus;
class mdtDeviceIosWidget;
class mdtPortStatusWidget;

/*! \brief Minimal tool for MODBUS I/O
 */
class mdtModbusIoTool : public QMainWindow, public Ui::mdtModbusIoTool
{
 Q_OBJECT

 public:

  /*! \brief Construct a mdtModbusIoTool object
   */
  mdtModbusIoTool(QWidget *parent = 0, Qt::WindowFlags flags = 0);

  /*! \brief destruct a mdtModbusIoTool object
   */
  virtual ~mdtModbusIoTool();

  /*! \brief Build the translations menu
   */
  void setAvailableTranslations(const QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey);

  /*! \brief Set MODBUS device
   *
   * \pre device must be a valid pointer
   */
  void setModbusDevice(const std::shared_ptr<mdtDeviceModbus> & device);

  /*! \brief Connect to MODBUS device
   */
  virtual bool connectToDevice();

  /*! \brief Disconnect from MODBUS device
   */
  virtual void disconnectFromDevice();

  /// \todo Essai...
  void updateIosWidget();

signals:
  
  void errorEvent();
  
 public slots:

  /*! \brief Retranslate
   *
   * This slot is called by mdtApplication
   *  when language has changed
   */
  void retranslate();

 private slots:

  /*! \brief Update current state
   */
  void setState(mdtDevice::State_t state);

  /*! \brief Call setup dialog
   *
   * \pre MODBUS device must be set with setModbusDevice() before calling this function
   */
  void setup();

  /*! \brief Connect to a device with given hardware node ID
   *
   * Node ID will be readen from sbHwNodeId spin box.
   *
   * \pre MODBUS device must be set with setModbusDevice() before calling this function
   */
  void connectToDeviceSlot();

  /*! \brief Disconnect from device
   */
  void disconnectFromDeviceSlot();

 protected:

  /*! \brief Used to show a message in status bar
   *
   * \param message Message to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showStatusMessage(const QString &message, int timeout = 0);

  /*! \brief Used to show a message in status bar
   *
   * \param message Message to show
   * \param details Details to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showStatusMessage(const QString &message, const QString &details, int timeout = 0);

 protected:

  std::shared_ptr<mdtDeviceModbus> pvDeviceModbus;

/// private:

  /*! \brief Set connecting to node state
   *
   * Will set the pvConnectingToNode flag, so
   *  other states will not update all widgets
   */
  void setStateConnectingToNode();

  /*! \brief Set connecting to node finished state
   *
   * Will unset pvConnectingToNode flag
   *  and tell port manager to notify current state
   */
  void setStateConnectingToNodeFinished();

  /*! \brief Set the disconnected state
   */
//  void setStatePortClosed();

  /*! \brief Set the disconnected state
   */
  void setStateDisconnected();

  /*! \brief Set the connecting state
   */
  void setStateConnecting();

  /*! \brief Set the ready state
   */
  void setStateReady();

  /*! \brief Set the busy state
   */
  void setStateBusy();

  /*! \brief Set the error state
   */
  void setStateError();

 private:

  bool pvReady; /// \todo Ok ???
  bool pvConnectingToNode;
  ///mdtDeviceModbusWago *pvDeviceModbusWago;
  ///mdtDeviceModbus *pvDeviceModbusWago;
  
  mdtDeviceIosWidget *pvDeviceIosWidget;
  mdtPortStatusWidget *pvStatusWidget;
  QString pvStatusDeviceInformations;   // Used to display informations about device in permanent area of status widget
  // Translations menu
  QActionGroup *pvLanguageActionGroup;
  // Disable copy
  Q_DISABLE_COPY(mdtModbusIoTool);
};

#endif  // #ifndef MDT_MODBUS_IO_TOOL_H
