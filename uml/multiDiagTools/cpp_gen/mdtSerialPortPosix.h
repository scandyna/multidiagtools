#ifndef _MDTSERIALPORTPOSIX_H
#define _MDTSERIALPORTPOSIX_H


class mdtSerialPortPosix : public mdtAbstractSerialPort {
  public:
    mdtSerialPortPosix();

    ~mdtSerialPortPosix();


  private:
    int pvSerialPortFd;

    struct termios pcTermios;

    bool setBaudRate(int rate);

    int baudRate();

    bool setDataBits(int n);

    int dataBits();

    bool setStopBits(int n);

    int stopBits();

    void setFlowCtlRtsCts(bool on);

    bool flowCtlRtsCtsOn();

    void setFlowCtlXonXoff(bool on);

    bool flowCtlXonXoffOn();

};
#endif
