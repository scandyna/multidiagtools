#ifndef MDT_ABSTRACT_SERIAL_PORT_H
#define MDT_ABSTRACT_SERIAL_PORT_H

#include "mdtSerialPortConfig.h"
#include "mdtPort.h"
#include "mdtFrame.h"
#include <QObject>
#include <QQueue>
#include <QList>

class mdtAbstractSerialPort : public mdtPort
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
  ~mdtAbstractSerialPort();

  /*! \brief Set the port attributes
   * 
   * Open the given port name and get his attributes.
   * This method must be re-implemented in plateform specific subclass.
   * \param portName Name of the port to open (f.ex: /dev/ttyS0 , COM1, ...)
   * \return True if given port is a serial port
   */
  virtual bool setAttributes(const QString &portName) = 0;

  /*! \brief Open the serial port with given configuration
   *
   * This method must be re-implemented in plateform specific subclass.
   * The implemented method must call mdtPort::open() at the right time
   * \param cfg Contains the setup for the serial port to open
   * \return True on successfull configuration and open port
   * \sa mdtPort
   */
  virtual bool open(mdtSerialPortConfig &cfg) = 0;

  /*! \brief Close the serial port
   *
   * This method must be re-implemented in plateform specific subclass.
   * Multiple call of this method is possible without any test
   * The implemented method must call mdtPort::close() at the right time
   * \sa mdtPort
   */
  virtual void close() = 0;

  /*! \brief Get UART type
   */
  sp_uart_type_t uartType();
  QString uartTypeStr();

  /*! \brief Get the list of available baud rates
   */
  QList<int> availableBaudRates();
  
  /*! \brief Enable/diseable the RTS (Request To Send) signal
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param on If true, RTS will be enabled, diseabled else
   */
  virtual void setRts(bool on) = 0;

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param on If true, DTR will be enabled, diseabled else
   */
  virtual void setDtr(bool on) = 0;

  /*! \brief Wait until a control (modem line) signal state changes
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool waitEventCtl() = 0;

  /*! \brief Get the control (modem line) signal states and update member flags
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the thread will be stopped.
   */
  virtual bool getCtlStates() = 0;

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
};

#endif  // #ifndef MDT_ABSTRACT_SERIAL_PORT_H
