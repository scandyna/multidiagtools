#ifndef _MDTSERIALPORTRXTHREAD_H
#define _MDTSERIALPORTRXTHREAD_H


#include "mdtSerialPortThread.h"

class mdtSerialPortRxThread : public mdtSerialPortThread {
  public:
    void run();

     mdtSerialPortRxThread(const QObject & parent = 0);

};
#endif
