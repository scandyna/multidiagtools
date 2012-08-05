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

class mdtPortThread;

/// NOTE: \todo Comment this class
/*! \brief Interface class for serial port
 *
 * This is a abstract class used to build system specific implementation.<br>
 * The subclass should have mdtSerialPort as name, so it will be compatible
 *  with several classes (f.ex. mdtSerialPortManager).<br>
 * To begin a implementation, please take a look at mdtAbstractPort at first.<br>
 *
 * \sa mdtAbstractPort
 */
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

  /*! \brief Close the serial port
   *
   * Close serial port and clear flags (f.ex. UART Type is set to unknow).
   *
   * The mutex is not handled by this method.
   */
  void close();

  /*! \brief Get the stored configuration
   */
  mdtSerialPortConfig &config();

  /*! \brief Get UART type
   *
   * The mutex is not handled by this method.
   *
   * If UART type is unknow and port is open,
   *  mapUartType() will be called.
   */
  sp_uart_type_t uartType();
  QString uartTypeStr();

  /*! \brief Get the list of available baud rates
   *
   * The mutex is not handled by this method.
   *
   * If the internal baud rates list is empty and port is open,
   *  buildAvailableBaudRates() will be called.
   */
  QList<int> &availableBaudRates();

  /*! \brief Set the baud rate
   *
   * The mutex is not handled by this method.
   *
   * \return True if baud rate is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The rate should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setBaudRate(int rate) = 0;

  /*! \brief Get the baud rate
   *
   * The mutex is not handled by this method.
   *
   * \return Configured baud rate, or 0 on error.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The rate should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual int baudRate() = 0;

  /*! \brief Set number of data bits.
   *
   * The mutex is not handled by this method.
   *
   * \return True if data bits count is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The data bits count should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setDataBits(int n) = 0;

  /*! \brief Get number of data bits.
   *
   * The mutex is not handled by this method.
   *
   * \return Configured data bits count or 0 on error.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The data bits count should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual int dataBits() = 0;

  /*! \brief Set number of stop bits.
   *
   * The mutex is not handled by this method.
   *
   * \return True if stop bits count is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The stop bits count should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setStopBits(int n) = 0;

  /*! \brief Get number of stop bits.
   *
   * The mutex is not handled by this method.
   *
   * \return Configured stop bits count or -1 on error.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The stop bits count should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual int stopBits() = 0;

  /*! \brief Set parity check.
   *
   * The mutex is not handled by this method.
   *
   * \return True if parity check is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The parity check should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setParity(mdtSerialPortConfig::parity_t p) = 0;

  /*! \brief Get configured parity
   *
   * The mutex is not handled by this method.
   *
   * \return Configured parity or NoParity on error.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The parity check should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual mdtSerialPortConfig::parity_t parity() = 0;

  /*! \brief Enable/diseable RTS/CTS flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if RTS/CTS flow control is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The RTS/CTS flow control should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setFlowCtlRtsCts(bool on) = 0;

  /*! \brief Get configured state of RTS/CTS flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if RTS/CTS flow control is enabled (and no error occured).
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The RTS/CTS flow control should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool flowCtlRtsCtsOn() = 0;

  /*! \brief Enable/diseable Xon/Xoff flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if Xon/Xoff flow control is supported and could be set.
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The Xon/Xoff flow control should be applied directly to system
   *  (f.ex. with tcsetattr() on POSIX) and checked.
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool setFlowCtlXonXoff(bool on, char xonChar, char xoffChar) = 0;

  /*! \brief Get configured state of Xon/Xoff flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if Xon/Xoff flow control is enabled (and no error occured).
   *
   * Subclass notes:<br>
   * This method must be implemented in subclass.
   * The Xon/Xoff flow control should be get with system call
   *  (f.ex. tgetattr() on POSIX).
   * The mdtError system should be used (on error) to keep trace in logfile.
   */
  virtual bool flowCtlXonXoffOn() = 0;

  /*! \brief Wait until a control (modem line) signal state changes
   *
   * This method is called from mdtSerialPortCtlThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   *
   * Subclass notes:<br>
   * This method must be re-implemented in plateform specific subclass.<br>
   * To handle port correctly, a sequence should be something like:<br>
   *  - Do some needed calls/init
   *  - Unlock the mutex
   *  - Enable thread's canclel state with setCancelStateEnabled()
   *  - Call system's wait function
   *  - Diseable thread's canclel state with setCancelStateEnabled()
   *  - Lock the mutext again
   *
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventCtl(/*mdtPortThread *thread*/) = 0;

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
   * If port is not open, this method must make no system call.
   *
   * The mutex is handled in this method.
   *
   * \param on If true, RTS will be enabled, diseabled else
   */
  virtual void setRts(bool on) = 0;

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   *
   * This method must be re-implemented in plateform specific subclass.<br>
   * If port is not open, this method must make no system call.
   *
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

  /*! \brief Map the system defined UART type to internal one.
   *
   * In this class, the UART type is defined in sp_uart_type_t enum.
   * The subclass must map the system's internal type to sp_uart_type_t.
   * On error (f.ex. by system calls), the method should use the mdtError
   *  system, and set the UART type to UT_UNKNOW.
   * Note that the precondition must be checked with Q_ASSERT(isOpen())
   *
   * \pre Port must be open before this call.
   */
  virtual void mapUartType() = 0;

  /*! \brief Build the list of available baud rates.
   *
   * The subclass must search about available baud rates for current port,
   *  and should check if it can be really be used.
   * For.ex. , on POSIX, termios defines some baud rates, but all are not
   *  necessarily supported by port. To be shure, a solution is to
   *  try each baudrate (with cfsetispeed()/cfsetospeed() on POSIX) before
   *  storing it in container.
   * Note that the precondition must be checked with Q_ASSERT(isOpen())
   *
   * \pre Port must be open before this call.
   */
  virtual void buildAvailableBaudRates() = 0;

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
