#ifndef _MDTSERIALPORTTHREAD_H
#define _MDTSERIALPORTTHREAD_H


class mdtSerialPort;

class mdtSerialPortThread : public QThread {
  private:
    mdtSerialPort * pvSerialPort;


  public:
     mdtSerialPortThread(const QObject & parent = 0);

    void setSerialPort(const mdtSerialPort & serialPort);

    void stop();


  private:
    volatile bool pvRunning;

};
#endif
