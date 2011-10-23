#ifndef _MDTSERIALPORTCONFIG_H
#define _MDTSERIALPORTCONFIG_H


#include <string>
using namespace std;

class mdtSerialPortConfig {
  private:
    int pvBaudRate;

    string pvInterface;

    parity_bit_state_t pvParityBit;

    int pvDataBitCount;

    int pvStopBitCount;

    flow_ctl_t pvFlowCtl;

    bool pvIsValid;


  public:
     mdtSerialPortConfig();

     ~mdtSerialPortConfig();

};
#endif
