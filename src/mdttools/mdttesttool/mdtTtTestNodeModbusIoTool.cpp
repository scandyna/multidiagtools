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
#include "mdtTtTestNodeModbusIoTool.h"
#include "mdtTtTestNodeManager.h"
#include "mdtDeviceModbus.h"
#include "mdtModbusHwNodeId.h"

#include <QDebug>

mdtTtTestNodeModbusIoTool::mdtTtTestNodeModbusIoTool(QWidget* parent, QSqlDatabase db)
 : mdtModbusIoTool(parent),
   pvTestNodeManager(new mdtTtTestNodeManager(this, db))
{
}

bool mdtTtTestNodeModbusIoTool::setModbusDevice(const QString & deviceAlias)
{
  // Get modbus device from global container
  auto device = pvTestNodeManager->device<mdtDeviceModbus>(deviceAlias);
  if(!device){
    ///showStatusMessage(tr("No device with alias '") + deviceAlias + tr("' is available in global device container."));
    qDebug() << "mdtTtTestNodeModbusIoTool::setModbusDevice(): device with alias " << deviceAlias << " not found in global device container";
    return false;
  }
  mdtModbusIoTool::setModbusDevice(device);

  return true;
}

bool mdtTtTestNodeModbusIoTool::connectToDevice()
{
  Q_ASSERT(pvDeviceModbus);

  /// \todo Check that not connected..
  
  // Update GUI state
  setStateConnectingToNode();
  // Try to find and connect to device
  if(!pvDeviceModbus->connectToDevice()){
    ///pvStatusWidget->setPermanentText(tr("Device with HW node ID ") + QString::number(sbHwNodeId->value()) + tr(" not found"));
    setStateConnectingToNodeFinished();
    return false;
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbus->detectIos()){
    QString details = tr("This probably caused by presence of a unsupported module.");
    showStatusMessage(tr("I/O detection failed"), details);
    pvDeviceModbus->disconnectFromDevice();
    setStateConnectingToNodeFinished();
    emit errorEvent();
    return false;
  }
  // Set I/O short labels
  if(!pvTestNodeManager->setDeviceIosLabelShort(pvDeviceModbus->alias(), false)){
    /// \todo display error
    qDebug() << "mdtTtTestNodeModbusIoTool::connectToDevice(): " << pvTestNodeManager->lastError().text();
    pvDeviceModbus->disconnectFromDevice();
    return false;
  }
  ///pvDeviceModbus->ios()->setIosDefaultLabelShort();
  ///pvDeviceIosWidget->setDeviceIos(pvDeviceModbus->ios());
  updateIosWidget();
  pvDeviceModbus->getDigitalOutputs(0);
  showStatusMessage(tr("I/O detection done"), 1000);
  ///pbAbortScan->setEnabled(false);
  ///pvStatusDeviceInformations = tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value());
  setStateConnectingToNodeFinished();
  ///pvStatusWidget->setPermanentText(tr("Connected to device with HW node ID ") + QString::number(sbHwNodeId->value()));

  return true;
}
