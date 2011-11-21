#ifndef _MDTSERIALPORTCTLTHREAD_H
#define _MDTSERIALPORTCTLTHREAD_H


#include "mdtSerialPortThread.h"

class mdtSerialPortCtlThread : public mdtSerialPortThread {
  public:
    void run();

     mdtSerialPortCtlThread(const QObject & parent = 0);

};
#endif
