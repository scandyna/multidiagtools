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

  /*! \brief Returns true if the thread is running
   *  This function overloads the QThread::isRunning() function.
   *  Note for subclass: when the thread is started and ready, the private member pvRunning
   *  must be set to true.
   */
  bool isRunning() const;
  
  /*! \brief Returns false if the thread is running
   *  \see isRunning()
   */
  bool isFinished() const;

 protected:

  volatile bool pvRunning;
  mdtSerialPort *pvSerialPort;
};

#endif  // #ifndef MDT_SERIAL_PORT_THREAD_H
