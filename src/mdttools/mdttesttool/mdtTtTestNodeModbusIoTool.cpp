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
    pvLastError.setError(tr("Device with alias '") + deviceAlias + tr("' not found in global device container."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeModbusIoTool");
    pvLastError.commit();
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
    pvLastError = pvDeviceModbus->lastError();
    setStateConnectingToNodeFinished();
    return false;
  }
  showStatusMessage(tr("I/O detection ..."));
  if(!pvDeviceModbus->detectIos()){
    pvLastError = pvDeviceModbus->lastError();
    setStateConnectingToNodeFinished();
    return false;
  }
  // Set I/O short labels
  if(!pvTestNodeManager->setDeviceIosLabelShort(pvDeviceModbus->alias(), false)){
    pvLastError = pvTestNodeManager->lastError();
    pvDeviceModbus->disconnectFromDevice();
    setStateConnectingToNodeFinished();
    return false;
  }
  updateIosWidget();
  showStatusMessage(tr("I/O detection done"), 1000);
  setStateConnectingToNodeFinished();
  // Start periodic inputs querying
  pvDeviceModbus->start(100);

  return true;
}
