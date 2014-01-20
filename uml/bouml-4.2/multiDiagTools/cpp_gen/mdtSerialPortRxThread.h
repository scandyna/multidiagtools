#ifndef _MDTSERIALPORTRXTHREAD_H
#define _MDTSERIALPORTRXTHREAD_H


class mdtSerialPortRxThread : public mdtSerialPortThread {
  public:
     mdtSerialPortRxThread(const QObject & parent = 0);


  private:
    void run();

};
#endif
