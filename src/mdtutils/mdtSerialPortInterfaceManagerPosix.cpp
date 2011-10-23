
#include "mdtAlgorithms.h"
#include "mdtSerialPortInterfaceManagerPosix.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include <sys/ioctl.h>
#include <linux/serial.h>

mdtSerialPortInterfaceManagerPosix::mdtSerialPortInterfaceManagerPosix()
{
  pvFd = -1;  // Make file descriptor invalid as default
}

mdtSerialPortInterfaceManagerPosix::~mdtSerialPortInterfaceManagerPosix()
{
}


bool mdtSerialPortInterfaceManagerPosix::scan()
{
  QDir dir;
  QStringList extList;
  QStringList filesPath;
  QFileInfoList filesInfo;
  
  // We only need system files
  dir.setFilter(QDir::Files | QDir::System);

  // Default Linux interfaces
  extList << "ttyS*";
  // Moxa interfaces
  extList << "ttyMXUSB*";
  
  // Common device directory
  dir.setPath("/dev");
  if(!dir.exists()){
    qDebug() << "mdtSerialPortInterfaceManagerPosix::scan(): directory '" << dir.dirName() << "' not exists";
    return false;
  }
  dir.setNameFilters(extList);
  // Gest list of found files
  filesInfo = dir.entryInfoList();
  
  // Get found files and sort the list
  //files = dir.entryList();


  // Construct the list of files
  for(int i=0; i<filesInfo.size(); ++i){
    filesPath << filesInfo.at(i).absoluteFilePath();
  }
  // Sort the list
  filesPath = mdtAlgorithms::sortStringListWithNumericEnd(filesPath);

  for(int i=0; i<filesPath.size(); ++i){
    qDebug() << "Found interface: " << filesPath.at(i);
    openPort(filesPath.at(i));
  }
  
  qDebug() << "Nb ifaces found: " << pvInterfaces.count();
  
  return true;
}

bool mdtSerialPortInterfaceManagerPosix::openPort(const QString &path)
{
  mdtSerialPortInterface *iface = 0;
  
  // Open interface:
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  pvFd = open(path.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if(pvFd < 0){
    qDebug() << "mdtSerialPortInterfaceManagerPosix::openPort(): can not open interface " << path;
    return false;
  }
  // Try to get UART type
  qDebug() << "UART type: " << uartType();
  // If we can determin UART type, then interface really exists
  if(uartType() != mdtSerialPortInterface::UT_UNKNOWN){
    iface = new mdtSerialPortInterface;
    pvInterfaces.append(iface);
  }
  
  // Temp
  if(pvFd >= 0){
    close(pvFd);
  }
  
  return true;
}

mdtSerialPortInterface::sp_uart_type_t mdtSerialPortInterfaceManagerPosix::uartType()
{
  struct serial_struct serinfo;

  if(ioctl(pvFd, TIOCGSERIAL, &serinfo) < 0){
    return mdtSerialPortInterface::UT_UNKNOWN;
  }
  // Map the type
  switch(serinfo.type){
    case PORT_8250:
      return mdtSerialPortInterface::UT_8250;
    case PORT_16450:
      return mdtSerialPortInterface::UT_16450;
    case PORT_16550:
      return mdtSerialPortInterface::UT_16550;
    case PORT_16550A:
      return mdtSerialPortInterface::UT_16550A;
    // NOTE: to complete !
    default:
      return mdtSerialPortInterface::UT_UNKNOWN;
  }
}
