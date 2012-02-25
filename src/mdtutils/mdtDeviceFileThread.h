#ifndef MDT_DEVICE_FILE_THREAD_H
#define MDT_DEVICE_FILE_THREAD_H

#include "mdtDeviceFile.h"
#include <QThread>
#include <QObject>

class mdtDeviceFileThread : public QThread
{
 public:

  mdtDeviceFileThread(QObject *parent = 0);

  /*! \brief Set the device file instance
   *  \pre deviceFile must be a valid pointer
   */
  void setDeviceFile(mdtDeviceFile *deviceFile);

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
  mdtDeviceFile *pvDeviceFile;
};

#endif  // #ifndef MDT_DEVICE_FILE_THREAD_H
