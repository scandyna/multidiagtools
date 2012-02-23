#ifndef MDT_SERIAL_PORT_CONFIG_H
#define MDT_SERIAL_PORT_CONFIG_H

#include <QString>
#include <QByteArray>
#include "mdtFrame.h"

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

  /*!\brief Set the RX frame size
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full. A full frame that never received
   * a EOF condition is considered invalid
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setRxFrameSize(int size);

  /*!\brief Get the RX frame size
   * \sa setRxFrameSize()
   */
  int rxFrameSize();

  /*!\brief Set the RX Queue size
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be received before they are threated
   * \pre size must be a positive value
   */
  void setRxQueueSize(int size);

  /*!\brief Get the RX Queue size
   * \sa setRxQueueSize()
   */
  int rxQueueSize();

  /*!\brief Set the TX frame size
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full.
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setTxFrameSize(int size);

  /*!\brief Get the TX frame size
   * \sa setTxFrameSize()
   */
  int txFrameSize();

  /*!\brief Set the TX Queue size
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be enqueued to serial port output.
   * \pre size must be a positive value
   */
  void setTxQueueSize(int size);

  /*!\brief Get the TX Queue size
   */
  int txQueueSize();

  /*! \brief Set frame type
   * Frame type can be an ASCII frame, binary frame, ...
   * \sa mdtFrame
   */
  void setFrameType(mdtFrame::type_t type);

  /*! \brief Get frame type
   * \sa setFrameType()
   */
  mdtFrame::type_t frameType();

  /*! \brief Set the end of frame sequence (applies on ASCII frames)
   */
  void setEofSeq(QByteArray eof);
  void setEofSeq(char eof);

  /*! \brief Get the end of frame sequence
   * 
   * \sa setEofSeq()
   */
  QByteArray eofSeq();

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
  // Frame and frame FIFO (queue) size
  int pvRxFrameSize;    // Maximum data length to store before a frame is considered invalid
  int pvRxQueueSize;    // Maximum number of frames that can be stored
  int pvTxFrameSize;    // Maximum data length to store before a frame is considered invalid
  int pvTxQueueSize;    // Maximum number of frames that can be stored
  // Frame type
  mdtFrame::type_t pvFrameType;
  QByteArray pvEofSeq;  // End of frame sequence (valid for ASCII frames)
  //flow_ctl_t pvFlowCtl;
  bool pvIsValid;

};

#endif  // #ifndef MDT_SERIAL_PORT_CONFIG_H
