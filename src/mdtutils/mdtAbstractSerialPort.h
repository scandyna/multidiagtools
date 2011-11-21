#ifndef MDT_ABSTRACT_SERIAL_PORT_H
#define MDT_ABSTRACT_SERIAL_PORT_H

#include "mdtSerialPortConfig.h"
#include <QObject>

class mdtAbstractSerialPort : public QObject
{
 Q_OBJECT

 public:

  mdtAbstractSerialPort(QObject *parent = 0);
  ~mdtAbstractSerialPort();

  /*! \brief Open the serial port with given configuration
   * This method must be re-implemented in plateform specific subclass.
   * \param cfg Contains the setup for the serial port to open
   * \return True on successfull configuration and open port
   */
  virtual bool openPort(mdtSerialPortConfig &cfg) = 0;

  /*! \brief Enable/diseable the RTS (Request To Send) signal
   * This method must be re-implemented in plateform specific subclass.
   * \param on If true, RTS will be enabled, diseabled else
   */
  virtual void setRts(bool on) = 0;

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   * This method must be re-implemented in plateform specific subclass.
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
};

#endif  // #ifndef MDT_ABSTRACT_SERIAL_PORT_H
