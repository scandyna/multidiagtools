/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SERIAL_PORT_CONFIG_H
#define MDT_SERIAL_PORT_CONFIG_H

#include <QString>
#include <QByteArray>
#include "mdtFrame.h"
#include "mdtPortConfig.h"

// Default Xon and Xoff char
#define MDT_DEF_XON  0x11   // DC1
#define MDT_DEF_XOFF 0x13   // DC3

class mdtSerialPortConfig : public mdtPortConfig
{
 public:

  enum parity_t { NoParity, ParityOdd, ParityEven };
  //enum flow_ctl_t { NoFlowCtl, FlowCtlRtsCts, FlowCtlXoXoff };

  mdtSerialPortConfig();
  ~mdtSerialPortConfig();

  /*! \brief Set default configuration
   * Default configuration is:
   *  - Interface: /dev/ttyS0 (Posix) or COM1 (Windows)
   *  - Baud rate: 9600
   *  - Data bits: 8
   *  - Stop bits: 1
   *  - Parity: none
   *  - Flow control: none
   */
  void setDefault();

  void setBaudRate(int baudRate);

  int baudRate() const;

  void setParity(parity_t parity);

  parity_t parity() const;

  void setDataBitsCount(int n);

  int dataBitsCount() const;

  void setStopBitsCount(int n);

  int stopBitsCount() const;

  void enableFlowCtlRtsCts();
  void diseableFlowCtlRtsCts();
  bool flowCtlRtsCtsEnabled() const;

  void enableFlowCtlXonXoff(char _xonChar =  MDT_DEF_XON, char _xoffChar = MDT_DEF_XOFF);
  void diseableFlowCtlXonXoff();
  bool flowCtlXonXoffEnabled() const;
  char xonChar() const;
  char xoffChar() const;

  bool isValid() const;

  /// NOTE: pas claire (héritage.. + test passe toujours..)
  bool operator==(const mdtSerialPortConfig &other);
  bool operator!=(const mdtSerialPortConfig &other);

 private:

  bool matches(const mdtSerialPortConfig &other);

  int pvBaudRate;
  parity_t pvParity;
  int pvDataBitCount;
  int pvStopBitCount;
  bool pvFlowCtlRtsCtsEnabled;
  bool pvFlowCtlXonXoffEnabled;
  char pvXonChar;
  char pvXoffChar;
  bool pvIsValid;
};

#endif  // #ifndef MDT_SERIAL_PORT_CONFIG_H
