#ifndef _MDTSERIALPORTCONFIG_H
#define _MDTSERIALPORTCONFIG_H


#include <string>
using namespace std;

class mdtSerialPortConfig {
  private:
    int pvBaudRate;

    string pvInterface;

    parity_t pvParity;

    int pvDataBitCount;

    int pvStopBitCount;

    flow_ctl_t pvFlowCtl;

    bool pvIsValid;


  public:
     mdtSerialPortConfig();

     ~mdtSerialPortConfig();

    void setBaudRate(int baudRate);

    int baudRate();

    void setParity(const parity_t & parity);

    void setDataBitsCount(int n);

    int dataBitsCount();

    void setStopBitsCount(int n);

    int stopBitsCount();

    void setFlowCtl(const flow_ctl_t & flowCtl);

    flow_ctl_t flowCtl();

    bool isValid();

};
#endif
