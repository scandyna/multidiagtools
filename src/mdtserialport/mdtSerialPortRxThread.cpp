
#include "mdtSerialPortRxThread.h"
#include <QDebug>
#include <QApplication>

mdtSerialPortRxThread::mdtSerialPortRxThread(QObject *parent)
 : mdtSerialPortThread(parent)
{
}

void mdtSerialPortRxThread::run()
{
  Q_ASSERT(pvSerialPort != 0);

  // Set the running flag
  pvSerialPort->lockMutex();
  pvRunning = true;
  pvSerialPort->unlockMutex();

  /// NOTE: provisiore
  char buffer[20];

  // Run...
  while(1){
    // Read thread state
    pvSerialPort->lockMutex();
    if(!pvRunning){
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();

    // Wait on RX event
    if(!pvSerialPort->waitEventRx()){
      pvSerialPort->lockMutex();
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    // Event occured, get the data from port - Check timeout state first
    if(!pvSerialPort->rxTimeoutOccured()){
      pvSerialPort->lockMutex();
      if(!pvSerialPort->readData(buffer, 19)){
        pvRunning = false;
        pvSerialPort->unlockMutex();
        break;
      }
      pvSerialPort->unlockMutex();
      buffer[19] = '\0';
      qDebug() << "RX: " << buffer;
    }
  }
}
