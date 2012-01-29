
#include "mdtAlgorithms.h"
#include "mdtSerialPortManagerPosix.h"
#include "mdtError.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <cerrno>
#include <cstring>

mdtSerialPortManagerPosix::mdtSerialPortManagerPosix()
{
}

mdtSerialPortManagerPosix::~mdtSerialPortManagerPosix()
{
  qDeleteAll(pvInterfaces);
}


bool mdtSerialPortManagerPosix::scan()
{
  QDir dir;
  QStringList extList;
  QStringList filesPath;
  QFileInfoList filesInfo;
  int portFd;
  mdtSerialPortInterface *iface = 0;
  mdtSerialPortInterface::sp_uart_type_t _uartType;

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
    return false;
  }
  dir.setNameFilters(extList);
  // Gest list of found files
  filesInfo = dir.entryInfoList();

  // Construct the list of absolute file paths
  for(int i=0; i<filesInfo.size(); ++i){
    filesPath << filesInfo.at(i).absoluteFilePath();
  }
  // Sort the list
  filesPath = mdtAlgorithms::sortStringListWithNumericEnd(filesPath);
  // For each path, try to open the interface (to see if it really exists)
  // and get some info
  for(int i=0; i<filesPath.size(); ++i){
    portFd = openPort(filesPath.at(i));
    if(portFd >= 0){
      // We have access to port, lets find UART type
      _uartType = uartType(portFd);
      if(_uartType != mdtSerialPortInterface::UT_UNKNOWN){
        // The interface really exists
        iface = new mdtSerialPortInterface;
        iface->setUartType(_uartType);
        iface->setPortNumber(i+1);
        iface->setPortName(filesPath.at(i));
        iface->setAvailableBaudRates(availableBaudRates(_uartType));
        // Add this interface to list
        pvInterfaces.append(iface);
      }
      // We have finish with this port
      close(portFd);
    }
  }

  return true;
}

int mdtSerialPortManagerPosix::openPort(const QString &path)
{
  int portFd;

  // Open interface:
  //  O_RDWR : Read/write access
  //  O_NOCTTY: not a terminal
  //  O_NDELAY: ignore DCD signal
  portFd = open(path.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if(portFd < 0){
    mdtError e(MDT_UNDEFINED_ERROR, "can not open interface '" + path + "'", mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtSerialPortManagerPosix");
    e.commit();
    return false;
  }

  return portFd;
}

mdtSerialPortInterface::sp_uart_type_t mdtSerialPortManagerPosix::uartType(int fd)
{
  struct serial_struct serinfo;

  if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0){
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
    case PORT_CIRRUS:
      return mdtSerialPortInterface::UT_CIRRUS;
    case PORT_16650:
      return mdtSerialPortInterface::UT_16650;
    case PORT_16650V2:
      return mdtSerialPortInterface::UT_16650V2;
    case PORT_16750:
      return mdtSerialPortInterface::UT_16750;
    case PORT_STARTECH:
      return mdtSerialPortInterface::UT_STARTECH;
    case PORT_16C950:
      return mdtSerialPortInterface::UT_16C950;
    case PORT_16654:
      return mdtSerialPortInterface::UT_16654;
    case PORT_16850:
      return mdtSerialPortInterface::UT_16850;
    case PORT_RSA:
      return mdtSerialPortInterface::UT_RSA;
    default:
      return mdtSerialPortInterface::UT_UNKNOWN;
  }
}

QList<int> mdtSerialPortManagerPosix::availableBaudRates(mdtSerialPortInterface::sp_uart_type_t _uartType)
{
  QList<int> baudRates;

  // Add the standard baud rates
  baudRates << 50;
  baudRates << 75;
  baudRates << 110;
  baudRates << 134; // Note: is 134.5
  baudRates << 150;
  baudRates << 200;
  baudRates << 300;
  baudRates << 600;
  baudRates << 1200;
  baudRates << 1800;
  baudRates << 2400;
  baudRates << 4800;
  baudRates << 9600;
  baudRates << 19200;
  baudRates << 38400;
#ifdef  B57600
  baudRates << 57600;
#endif
#ifdef B76800
  baudRates << 76800;
#endif
#ifdef  B115200
  baudRates << 115200;
#endif
#ifdef  B230400
  baudRates << 230400;
#endif
#ifdef  B460800
  baudRates << 460800;
#endif
#ifdef  B500000
  baudRates << 500000;
#endif
#ifdef  B576000
  baudRates << 576000;
#endif
#ifdef  B921600
  baudRates << 921600;
#endif
#ifdef  B1000000
  baudRates << 1000000;
#endif
#ifdef  B1152000
  baudRates << 1152000;
#endif
#ifdef  B1500000
  baudRates << 1500000;
#endif
#ifdef  B2000000
  baudRates << 2000000;
#endif
#ifdef  B2500000
  baudRates << 2500000;
#endif
#ifdef  B3000000
  baudRates << 3000000;
#endif
#ifdef  B3500000
  baudRates << 3500000;
#endif
#ifdef  B4000000
  baudRates << 4000000;
#endif

  return baudRates;
}
