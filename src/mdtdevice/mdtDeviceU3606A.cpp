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
#include "mdtAnalogIo.h"
#include <QByteArray>
#include <QList>

#include <QDebug>

mdtDeviceU3606A::mdtDeviceU3606A(QObject *parent)
 : mdtDeviceScpi(parent)
{
  pvCodec = new mdtFrameCodecScpiU3606A;
  // Setup port manager
  portManager()->config().setReadFrameSize(512);
  portManager()->config().setReadQueueSize(10);
  portManager()->config().setReadTimeout(30000);
  portManager()->config().setWriteFrameSize(512);
  portManager()->config().setWriteQueueSize(1);
  connect(pvUsbtmcPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  connect(pvUsbtmcPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
}

mdtDeviceU3606A::~mdtDeviceU3606A()
{
  delete pvCodec;
}

mdtAbstractPort::error_t mdtDeviceU3606A::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtDeviceInfo device;

  // Setup device info
  device = devInfo;
  device.setVendorId(0x0957);
  device.setProductId(0x4d18);

  return mdtDeviceScpi::connectToDevice(device);
}

void mdtDeviceU3606A::onStateChanged(int state)
{
  qDebug() << "mdtDeviceU3606A::onStateChanged() ...";
}

void mdtDeviceU3606A::decodeReadenFrame(mdtPortTransaction transaction)
{
  bool ok;

  qDebug() << "mdtDeviceU3606A::decodeReadenFrame() , ID: " << transaction.id();
  
  setStateReady();
  switch(transaction.type()){
    case MDT_FC_SCPI_VALUE:
      if(transaction.analogIo() == 0){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received value response from device but no I/O was affected to query", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
        e.commit();
      }else{
        ok = pvCodec->decodeValues(transaction.data());
        if(ok && (pvCodec->values().size() == 1)){
          transaction.analogIo()->setValue(pvCodec->values().at(0), false);
        }else{
          transaction.analogIo()->setValue(QVariant(), false);
        }
      }
      break;
    case MDT_FC_SCPI_ERR:
      ok = pvCodec->decodeError(transaction.data());
      if(!ok){
        mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": SYST:ERR? reply could not be decoded", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
        e.commit();
        break;
      }
      handleDeviceError(pvCodec->values());
      break;
    default:
      mdtError e(MDT_DEVICE_ERROR, "Device " + name() + ": received unsupported response from device (type: " + QString::number(transaction.type()) + ")", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtDeviceU3606A");
      e.commit();
  }
}

int mdtDeviceU3606A::readAnalogInput(mdtPortTransaction *transaction)
{
  Q_ASSERT(pvIos != 0);
  Q_ASSERT(transaction != 0);

  int bTag;

  /// \todo Add resolution and limits (min/max)
  /// \todo Handle type (Ampere, Voltage, Resistance, AC/DC, ...)
  setStateBusy();
  // Wait until data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady()){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Send query
  bTag = pvUsbtmcPortManager->writeData("MEAS:VOLT:DC?\n");
  if(bTag < 0){
    return bTag;
  }
  // Wait until more data can be sent
  if(!pvUsbtmcPortManager->waitOnWriteReady()){
    return mdtAbstractPort::WritePoolEmpty;
  }
  // Remember query type.
  transaction->setType(MDT_FC_SCPI_VALUE);
  // Send read request
  bTag = pvUsbtmcPortManager->sendReadRequest(transaction);

  return bTag;
}

bool mdtDeviceU3606A::queriesSequence()
{
  getAnalogInputValue(0, true, true, true);

  return true;
}

/**
void mdtDeviceU3606A::handleDeviceSpecificError()
{
}
*/