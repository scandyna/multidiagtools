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
#ifndef MDT_ABSTRACT_SERIAL_PORT_H
#define MDT_ABSTRACT_SERIAL_PORT_H

#include "mdtSerialPortConfig.h"
#include "mdtAbstractPort.h"
#include "mdtFrame.h"
#include <QObject>
#include <QQueue>
#include <QList>

/// NOTE: \todo Comment this class
class mdtAbstractSerialPort : public mdtAbstractPort
{
 Q_OBJECT

 public:

  // Source: http://lxr.oss.org.cn/source/include/linux/serial.h#L33
  /*! \brief UART type
   */
  enum sp_uart_type_t { UT_UNKNOW,
                        UT_8250,
                        UT_16450,
                        UT_16550,
                        UT_16550A,
                        UT_CIRRUS,
                        UT_16650,
                        UT_16650V2,
                        UT_16750,
                        UT_STARTECH,
                        UT_16C950,
                        UT_16654,
                        UT_16850,
                        UT_RSA
                      };

  mdtAbstractSerialPort(QObject *parent = 0);
  virtual ~mdtAbstractSerialPort();

  /*! \brief Open the port
   *
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Close previous opened ressource
   *  - Lock the mutex with lockMutex()
   *  - Do the specific work
   *  - Set the read/write timeouts. See the mdtSerialPortConfig to know how to get these timeouts.
   *  - Call this open method (with mdtAbstractPort::open() ).
   * At this last step, the queues will be initialized, and mutex unocked.
   * \return True on successfull configuration and open port
   * \sa mdtPortConfig
   */
  virtual bool open(mdtSerialPortConfig &cfg) = 0;

  /*! \brief Get UART type
   */
  sp_uart_type_t uartType();
  QString uartTypeStr();

  /*! \brief Get the list of available baud rates
   */
  QList<int> &availableBaudRates();

  /*! \brief Wait until a control (modem line) signal state changes
   *
   * This method must be re-implemented in plateform specific subclass.
   *
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventCtl() = 0;

  /// NOTE: \todo Update name to : updateCtlStates ?
  /*! \brief Get the control (modem line) signal states and update member flags
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool getCtlStates() = 0;

  /*! \brief Get the CAR (CD) state
   *
   * The mutex is handled in this method.
   */
  bool carIsOn();

  /*! \brief Get the DSR state
   *
   * The mutex is handled in this method.
   */
  bool dsrIsOn();

  /*! \brief Get the CTS state
   *
   * The mutex is handled in this method.
   */
  bool ctsIsOn();

  /*! \brief Get the RNG (RI) state
   *
   * The mutex is handled in this method.
   */
  bool rngIsOn();

 public slots:

  /*! \brief Enable/diseable the RTS (Request To Send) signal
   *
   * This method must be re-implemented in plateform specific subclass.<br>
   * If port is not open, this method must make no system call.<br>
   * The mutex is handled in this method.
   *
   * \param on If true, RTS will be enabled, diseabled else
   */
  virtual void setRts(bool on) = 0;

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   *
   * This method must be re-implemented in plateform specific subclass.<br>
   * If port is not open, this method must make no system call.<br>
   * The mutex is handled in this method.
   *
   * \param on If true, DTR will be enabled, diseabled else
   */
  virtual void setDtr(bool on) = 0;

 signals:

  /*! \brief Emited whenn CAR (CD) status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void carChanged(bool on);

  /*! \brief Emited whenn DSR status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void dsrChanged(bool on);

  /*! \brief Emited whenn CTS status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void ctsChanged(bool on);

  /*! \brief Emited whenn RNG (RI) status changed
   *  \param on When true, line signal's new state is enabled (diseabled else)
   */
  void rngChanged(bool on);

 protected:

  // Control signals states
  bool pvCarIsOn;
  bool pvDsrIsOn;
  bool pvCtsIsOn;
  bool pvRngIsOn;
  // Attributes
  sp_uart_type_t pvUartType;
  QList<int> pvAvailableBaudRates;

 private:

  // Diseable copy
  mdtAbstractSerialPort(mdtAbstractSerialPort &other);
};

#endif  // #ifndef MDT_ABSTRACT_SERIAL_PORT_H
