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
#include "mdtDeviceU3606A.h"
#include "mdtError.h"
#include <QByteArray>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDevice(parent)
{
}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
}

QStringList mdtDeviceU3606A::scan()
{
  QStringList ports;
  QStringList portsWithDevice;
  QByteArray command = "*IDN?";
  int i;

  ports = pvPortManager.scan();
  // Travel found ports
  for(i=0; i<ports.size(); i++){
    // Try to open port
    pvPortManager.setPortName(ports.at(i));
    if(pvPortManager.openPort()){
      if(pvPortManager.start()){
        // Get the IDN
        if(pvPortManager.writeData(command, true)){
          if(pvPortManager.waitReadenFrame()){
            if(pvCodec.decodeIdn(pvPortManager.lastReadenFrame())){
              if(pvCodec.values().size() >= 2){
                if(pvCodec.values().at(1) == "U3606A"){
                  portsWithDevice << ports.at(i);
                }
              }
            }
          }
        }
      }
      pvPortManager.stop();
    }
    pvPortManager.closePort();
  }

  return portsWithDevice;
}

bool mdtDeviceU3606A::selectPort(const QStringList &ports, int index)
{
  if(ports.size() < 1){
    mdtError e(MDT_DEVICE_ERROR, "Passed ports is a empty lits" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
    e.commit();
    return false;
  }
  // Check index request
  if(index > ports.size()){
    mdtError e(MDT_DEVICE_ERROR, "Passed index is to big" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
    e.commit();
    return false;
  }

  return selectPort(ports.at(index));
}

bool mdtDeviceU3606A::selectPort(const QString &port)
{
  // Open the port manager for given port
  pvPortManager.setPortName(port);
  if(!pvPortManager.openPort()){
    return false;
  }
  // Start the port manager
  if(!pvPortManager.start()){
    pvPortManager.closePort();
    return false;
  }

  return true;
}

void mdtDeviceU3606A::getIdn()
{
  QByteArray command = "*IDN?";
  QStringList data;
  QStringList boardsRevisions;
  int i;
  
  // Default data
  for(i=0; i<6; i++){
    data << "Unknow";
  }
  
  if(!pvPortManager.writeData(command, true)){
    emit idnChanged(data);
    return;
  }
  if(!pvPortManager.waitReadenFrame()){
    emit idnChanged(data);
    return;
  }
  if(!pvCodec.decodeIdn(pvPortManager.lastReadenFrame())){
    emit idnChanged(data);
    return;
  }
  if(pvCodec.values().size() != 4){
    emit idnChanged(data);
    return;
  }
  // We have data, construct the list
  data.clear();
  data << pvCodec.values().at(0).toString();
  data << pvCodec.values().at(1).toString();
  data << pvCodec.values().at(2).toString();
  boardsRevisions = pvCodec.values().at(3).toString().split("-");
  for(i=0; i<boardsRevisions.size(); i++){
    data << boardsRevisions.at(i);
  }
  emit idnChanged(data);
}

void mdtDeviceU3606A::essais()
{
  getIdn();
}
