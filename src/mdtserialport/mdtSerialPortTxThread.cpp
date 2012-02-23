
#include "mdtSerialPortTxThread.h"
#include <QDebug>
#include <QApplication>

mdtSerialPortTxThread::mdtSerialPortTxThread(QObject *parent)
 : mdtSerialPortThread(parent)
{
}

void mdtSerialPortTxThread::run()
{
  Q_ASSERT(pvSerialPort != 0);

  // Set the running flag
  pvSerialPort->lockMutex();
  pvRunning = true;
  pvSerialPort->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    pvSerialPort->lockMutex();
    if(!pvRunning){
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();

    // Wait on TX ready event
    if(!pvSerialPort->waitEventTxReady()){
      pvSerialPort->lockMutex();
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvSerialPort->txTimeoutOccured()){
      pvSerialPort->lockMutex();
      
      
      if(!pvSerialPort->writeData("Just for test", 14)){
        pvRunning = false;
        pvSerialPort->unlockMutex();
        break;
      }
      pvSerialPort->unlockMutex();
    }
  }
}
