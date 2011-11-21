#ifndef MDT_SERIAL_PORT_THREAD_H
#define MDT_SERIAL_PORT_THREAD_H

#include "mdtSerialPort.h"
#include <QThread>
#include <QObject>

class mdtSerialPortThread : public QThread
{
 public:

  mdtSerialPortThread(QObject *parent = 0);

  /*! \brief Set the serial port instance
   *  \pre serialPort must be a valid pointer
   */
  void setSerialPort(mdtSerialPort *serialPort);

  /*! \brief Start the thread
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  void start();

  /*! \brief Stop the running thread
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  void stop();

 protected:

  volatile bool pvRunning;
  mdtSerialPort *pvSerialPort;
};

#endif  // #ifndef MDT_SERIAL_PORT_THREAD_H
