#ifndef MDT_PORT_THREAD_H
#define MDT_PORT_THREAD_H

#include "mdtPort.h"
#include <QThread>
#include <QObject>

class mdtPortThread : public QThread
{
 public:

  mdtPortThread(QObject *parent = 0);
  ~mdtPortThread();

  /*! \brief Set the device file instance
   *  \pre deviceFile must be a valid pointer
   */
  void setPort(mdtPort *port);

  /*! \brief Start the thread
   *  \return True on sucsessfull start, or flase on start timeout
   *  \pre Serial port instance must be defined. \see setSerialPort()
   */
  bool start();

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
  mdtPort *pvPort;
  int pvReadMinWaitTime;
  int pvWriteMinWaitTime;
};

#endif  // #ifndef MDT_PORT_THREAD_H
