#ifndef _MDTSERIALPORTTXTHREAD_H
#define _MDTSERIALPORTTXTHREAD_H


#include "mdtSerialPortThread.h"

class mdtSerialPortTxThread : public mdtSerialPortThread {
  public:
    void run();

     mdtSerialPortTxThread(const QObject & parent = 0);

};
#endif
