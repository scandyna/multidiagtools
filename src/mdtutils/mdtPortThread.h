#ifndef MDT_PORT_THREAD_H
#define MDT_PORT_THREAD_H

#include "mdtAbstractPort.h"
#include "mdtError.h"
#include <QThread>
#include <QObject>

class mdtPortThread : public QThread
{
 Q_OBJECT

 public:

  mdtPortThread(QObject *parent = 0);
  ~mdtPortThread();

  /*! \brief Set the device file instance
   *  \pre deviceFile must be a valid pointer
   */
  void setPort(mdtAbstractPort *port);

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

 signals:

  /*! \brief Emitted on error
   * 
   * When a error occurs, this signal is emited.<br>
   * This can happen, for example, when a USB device becomes not present.<br>
   * mdtPortManager uses this signal.
   * \sa mdtPortManager
   */
  void errorOccured(int error);

 protected:

  volatile bool pvRunning;
  mdtAbstractPort *pvPort;
  int pvReadMinWaitTime;
  int pvWriteMinWaitTime;
  bool pvBytePerByteWrite;
};

#endif  // #ifndef MDT_PORT_THREAD_H
