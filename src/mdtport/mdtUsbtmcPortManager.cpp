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
#include "mdtUsbtmcPortManager.h"
#include "mdtAlgorithms.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include "mdtFrameUsbTmc.h"
#include <QDir>
#include <QFileInfoList>
#include <QString>
#include <QApplication>

#include <QDebug>

mdtUsbtmcPortManager::mdtUsbtmcPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Port setup
  mdtPortConfig *config = new mdtPortConfig;
  config->setFrameType(mdtFrame::FT_USBTMC);
  config->setReadFrameSize(512);
  config->setWriteFrameSize(512);
  mdtUsbPort *port = new mdtUsbPort;
  port->setConfig(config);
  setPort(port);

  // Threads setup
  addThread(new mdtUsbPortThread);

  // USBTMC specific
  pvCurrentWritebTag = 0;
}

mdtUsbtmcPortManager::~mdtUsbtmcPortManager()
{
  Q_ASSERT(pvPort != 0);

  delete &pvPort->config();
  detachPort(true, true);
}

/**
QStringList mdtUsbtmcPortManager::scan()
{
  QStringList portNames;
  QStringList availablePorts;
  mdtUsbtmcPort *port;
  QFileInfoList filesInfo;
  QDir dir;
  QStringList extList;

  // We only need system files
  dir.setFilter(QDir::Files | QDir::System);
  // USBTMC interfaces
  extList << "usbtmc*";
  // Common device directory
  dir.setPath("/dev");
  if(!dir.exists()){
    mdtError e(MDT_UNDEFINED_ERROR, "directory '" + dir.dirName() + "' not exists", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortManagerPosix");
    e.commit();
    return availablePorts;
  }
  dir.setNameFilters(extList);
  // Gest list of found files
  filesInfo = dir.entryInfoList();
  // Construct the list of absolute file paths
  for(int i=0; i<filesInfo.size(); ++i){
    portNames << filesInfo.at(i).absoluteFilePath();
  }
  // Sort the list
  portNames = mdtAlgorithms::sortStringListWithNumericEnd(portNames);
  // For each port name, try to open the port and get some attributes (to see if it really exists)
  for(int i=0; i<portNames.size(); ++i){
    // Try to open port
    port = new mdtUsbtmcPort;
    Q_ASSERT(port != 0);
    port->setPortName(portNames.at(i));
    if(port->open() == mdtAbstractPort::NoError){
      availablePorts.append(portNames.at(i));
      port->close();
    }
    delete port;
  }

  return availablePorts;
}
*/

bool mdtUsbtmcPortManager::writeData(QByteArray data)
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Clear previous readen frames
  pvReadenFrames.clear();
  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return false;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data and add frame to write queue
  frame->setWaitAnAnswer(false);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_OUT);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData(data);
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return true;
}

bool mdtUsbtmcPortManager::sendReadRequest()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;

  // Get a frame in pool
  pvPort->lockMutex();
  if(pvPort->writeFramesPool().size() < 1){
    pvPort->unlockMutex();
    mdtError e(MDT_PORT_IO_ERROR, "No frame available in write frames pool", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
    e.commit();
    return false;
  }
  frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->writeFramesPool().dequeue());
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();
  // Store data, encode and add frame to write queue
  frame->setWaitAnAnswer(true);
  frame->setMsgID(mdtFrameUsbTmc::DEV_DEP_MSG_IN);
  // Increment bTag and enshure it's <> 0
  pvCurrentWritebTag++;
  if(pvCurrentWritebTag == 0){
    pvCurrentWritebTag++;
  }
  frame->setbTag(pvCurrentWritebTag);
  frame->setMessageData("");
  frame->encode();
  pvPort->addFrameToWrite(frame);
  pvPort->unlockMutex();

  return true;
}

bool mdtUsbtmcPortManager::waitReadenFrame(int timeout)
{
  Q_ASSERT(pvPort != 0);

  if(!sendReadRequest()){
    return false;
  }

  return mdtPortManager::waitReadenFrame(timeout);
}

void mdtUsbtmcPortManager::fromThreadNewFrameReaden()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *frame;
  QByteArray data;

  // Get frames in readen queue
  pvPort->lockMutex();
  while(pvPort->readenFrames().size() > 0){
    frame = dynamic_cast<mdtFrameUsbTmc*> (pvPort->readenFrames().dequeue());
    Q_ASSERT(frame != 0);
    // Check if frame is complete
    if(frame->isComplete()){
      // Copy data
      data.append(frame->messageData().data(), frame->messageData().size());
      pvReadenFrames.append(data);
    }
    // Put frame back into pool
    pvPort->readFramesPool().enqueue(frame);
  };
  pvPort->unlockMutex();
  emit(newReadenFrame());
}
