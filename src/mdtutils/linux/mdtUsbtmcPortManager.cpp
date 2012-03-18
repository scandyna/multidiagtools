
#include "mdtUsbtmcPortManager.h"
#include "mdtAlgorithms.h"
#include "mdtError.h"
#include "mdtPortConfig.h"
#include "mdtFrame.h"
#include <QDir>
#include <QFileInfoList>
#include <QString>
#include <QApplication>

mdtUsbtmcPortManager::mdtUsbtmcPortManager(QObject *parent)
 : mdtPortManager(parent)
{
  // Alloc a port and connect signals
  setPort(new mdtUsbtmcPort);
  Q_ASSERT(pvWriteThread != 0);
  connect(pvWriteThread, SIGNAL(frameWritten()), this, SLOT(frameWritten()));
  // Set USBTMC specific configuration
  pvConfig.setFrameType(mdtFrame::FT_ASCII);
  pvConfig.setEndOfFrameSeq("\n");
  pvConfig.setReadFrameSize(512);
  pvConfig.setWriteFrameSize(512);
  // Flags
  pvFrameWritten = false;
  pvWaitingFrame = false;
}

mdtUsbtmcPortManager::~mdtUsbtmcPortManager()
{
}

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
    // Try to get port attributes
    port = new mdtUsbtmcPort;
    Q_ASSERT(port != 0);
    if(port->setAttributes(portNames.at(i))){
      availablePorts.append(portNames.at(i));
    }
    delete port;
  }

  return availablePorts;
}

bool mdtUsbtmcPortManager::writeData(QByteArray data, bool waitAnswer)
{
  Q_ASSERT(pvPort != 0);

  // Set flags
  pvFrameWritten = false;
  pvWaitingFrame = waitAnswer;

  if(!mdtPortManager::writeData(data)){
    return false;
  }

  // Enable write thread flag
  pvPort->writeOneFrame();
  // Wait until frame was written
  while(!pvFrameWritten){
    qApp->processEvents();
    msleep(50);
  }

  return true;
}

void mdtUsbtmcPortManager::frameWritten()
{
  Q_ASSERT(pvPort != 0);
  
  pvFrameWritten = true;
  if(pvWaitingFrame){
    pvPort->readOneFrame();
  }
}
