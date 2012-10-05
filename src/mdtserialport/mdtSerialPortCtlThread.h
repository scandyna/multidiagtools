#ifndef MDT_SERIAL_PORT_CTL_THREAD_H
#define MDT_SERIAL_PORT_CTL_THREAD_H

#include "mdtPortThread.h"
#include "mdtSerialPort.h"
#include <QObject>

class mdtSerialPortCtlThread : public mdtPortThread
{
 public:

  mdtSerialPortCtlThread(QObject *parent = 0);

  /*! \brief Returns true if this thread reads data and send the newFrameReaden() signal
   *
   * mdtPortManager can handle many threads. It needs to know wich one will send the
   *  newFrameReaden() signal, so it can connect it to his slot.
   */
  bool isReader() const;

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_THREAD_H
