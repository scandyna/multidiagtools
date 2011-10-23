#ifndef MDT_SERIAL_PORT_CONFIG_H
#define MDT_SERIAL_PORT_CONFIG_H

#include <string>

using namespace std;

class mdtSerialPortConfig
{
 public:

  enum parity_bit_state_t { NoParity, ParityOdd, ParityEven };
  enum flow_ctl_t { NoFlowCtl, FlowCtlXoXoff, FlowCtlRtsCts };
   
  mdtSerialPortConfig();
  ~mdtSerialPortConfig();

 private:

  int pvBaudRate;
  string pvInterface;
  parity_bit_state_t pvParityBit;
  int pvDataBitCount;
  int pvStopBitCount;
  flow_ctl_t pvFlowCtl;
  bool pvIsValid;

};

#endif  // #ifndef MDT_SERIAL_PORT_CONFIG_H
