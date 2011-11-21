#ifndef MDT_SERIAL_PORT_CONFIG_H
#define MDT_SERIAL_PORT_CONFIG_H

#include <QString>

// Default Xon and Xoff char
#define MDT_DEF_XON  0x11   // DC1
#define MDT_DEF_XOFF 0x13   // DC3

using namespace std;

class mdtSerialPortConfig
{
 public:

  enum parity_t { NoParity, ParityOdd, ParityEven };
  //enum flow_ctl_t { NoFlowCtl, FlowCtlRtsCts, FlowCtlXoXoff };

  mdtSerialPortConfig();
  ~mdtSerialPortConfig();
  mdtSerialPortConfig(const mdtSerialPortConfig &orig);

  /*! \brief Set default configuration
   * Default configuration is:
   *  - Interface: /dev/ttyS0 (Posix) or COM1 (Windows)
   *  - Baud rate: 9600
   *  - Data bits: 8
   *  - Stop bits: 1
   *  - Parity: none
   *  - Flow contrôl: none
   */
  void setDefault();
  
  /*! \brief Set interface
   * Interface can be /dev/ttyS0 (on Posix) ,
   * or COM1 (On Windows)
   */
  void setInterface(const QString &iface);
  
  /*! \brief Get intarfce
   * \sa setInterface()
   */
  QString interface();
  
  void setBaudRate(int baudRate);

  int baudRate();

  void setParity(parity_t parity);
  
  parity_t parity();

  void setDataBitsCount(int n);

  int dataBitsCount();

  void setStopBitsCount(int n);

  int stopBitsCount();

  void enableFlowCtlRtsCts();
  void diseableFlowCtlRtsCts();
  bool flowCtlRtsCtsEnabled();

  void enableFlowCtlXonXoff(char _xonChar =  MDT_DEF_XON, char _xoffChar = MDT_DEF_XOFF);
  void diseableFlowCtlXonXoff();
  bool flowCtlXonXoffEnabled();
  char xonChar();
  char xoffChar();

  bool isValid();
  
  void operator=(const mdtSerialPortConfig &orig);
  
  bool operator==(const mdtSerialPortConfig &other);
  bool operator!=(const mdtSerialPortConfig &other);

 private:

  // Copy members from orig
  void copy(const mdtSerialPortConfig &orig);
  bool matches(const mdtSerialPortConfig &other);

  QString pvInterface;    // Interface, like: /dev/ttyS0 or COM1
  int pvBaudRate;
  parity_t pvParity;
  int pvDataBitCount;
  int pvStopBitCount;
  bool pvFlowCtlRtsCtsEnabled;
  bool pvFlowCtlXonXoffEnabled;
  char pvXonChar;
  char pvXoffChar;
  //flow_ctl_t pvFlowCtl;
  bool pvIsValid;

};

#endif  // #ifndef MDT_SERIAL_PORT_CONFIG_H
