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
#include "mdtSerialPortManager.h"
#include "mdtSerialPortConfig.h"
#include "mdtError.h"
#include "mdtAlgorithms.h"
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <typeinfo>

#include <QDebug>

mdtSerialPortManager::mdtSerialPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  setConfig(new mdtSerialPortConfig);
  setPort(new mdtSerialPort);
}

mdtSerialPortManager::~mdtSerialPortManager()
{
  Q_ASSERT(pvConfig != 0);
  Q_ASSERT(pvPort != 0);

  closePort();

  delete pvConfig;
  pvConfig = 0;
  delete pvPort;
  pvPort = 0;
}

QStringList mdtSerialPortManager::scan()
{
  mdtSerialPort *port;
  QStringList portNames;
  QStringList availablePorts;

#ifdef Q_OS_UNIX
  QFileInfoList filesInfo;
  QDir dir;
  QStringList extList;

  // We only need system files
  dir.setFilter(QDir::Files | QDir::System);
  // Default Linux interfaces
  extList << "ttyS*";
  // USB interfaces
  extList << "ttyUSB*";
  // Moxa USB interfaces
  extList << "ttyMXUSB*";
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
#endif  // #ifdef Q_OS_UNIX

#ifdef Q_OS_WIN
  QString str;

  // I found nothing better than this ..
  for(int i=1; i<=20; i++){
    str = QString("COM%1").arg(i);
    portNames.append(str);
  }
#endif  // #ifdef Q_OS_WIN

  // For each port name, try to open the port and get some attributes (to see if it really exists)
  for(int i=0; i<portNames.size(); ++i){
    // Try to get port attributes
    port = new mdtSerialPort;
    Q_ASSERT(port != 0);
    if(port->setAttributes(portNames.at(i))){
      availablePorts.append(portNames.at(i));
    }
    delete port;
  }

  return availablePorts;
}

void mdtSerialPortManager::setPort(mdtSerialPort *port)
{
  Q_ASSERT(port != 0);

  pvCtlThread.setPort(port);
  mdtPortManager::setPort(port);
}

bool mdtSerialPortManager::openPort()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvConfig != 0);

  // We must typecast to the mdtSerialPort* classes
  mdtSerialPort *p = dynamic_cast<mdtSerialPort*>(pvPort);
  Q_ASSERT(p != 0);
  mdtSerialPortConfig *c = dynamic_cast<mdtSerialPortConfig*>(pvConfig);
  Q_ASSERT(c != 0);

  return p->open(*c);
}

bool mdtSerialPortManager::start()
{
  if(!pvCtlThread.start()){
    return false;
  }
  return mdtPortManager::start();
}

void mdtSerialPortManager::stop()
{
  mdtPortManager::stop();
  if(pvCtlThread.isRunning()){
    pvCtlThread.stop();
  }
}

mdtSerialPortConfig &mdtSerialPortManager::config()
{
  mdtSerialPortConfig *config = dynamic_cast<mdtSerialPortConfig*>(pvConfig);
  Q_ASSERT(config != 0);

  return *config;
}

mdtAbstractSerialPort &mdtSerialPortManager::port()
{
  Q_ASSERT(pvPort != 0);

  mdtAbstractSerialPort *sp = dynamic_cast<mdtAbstractSerialPort*>(pvPort);
  Q_ASSERT(sp != 0);

  return *sp;
}
