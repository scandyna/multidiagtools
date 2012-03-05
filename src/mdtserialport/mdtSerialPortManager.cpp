
#include "mdtSerialPortManager.h"
#include "mdtSerialPortConfig.h"
#include "mdtError.h"
#include "mdtAlgorithms.h"
#include <QDir>
#include <QFileInfo>
#include <QString>

#include <QDebug>

mdtSerialPortManager::mdtSerialPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  pvSerialPort = 0;
}

mdtSerialPortManager::~mdtSerialPortManager()
{
}

void mdtSerialPortManager::setSerialPortObj(mdtAbstractSerialPort *sp)
{
  Q_ASSERT(sp != 0);

  pvSerialPort = sp;
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
  
  qDebug() << "Available serial ports: " << availablePorts;
  return availablePorts;
}
