#ifndef MDT_PORT_READ_THREAD_H
#define MDT_PORT_READ_THREAD_H

#include "mdtPortThread.h"
#include "mdtFrame.h"
#include <QObject>

class mdtPortReadThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtPortReadThread(QObject *parent = 0);

 signals:

  /*! \brief Emited when a complete frame is available
   * 
   * This signal is emited when a complete frame is available.<br>
   * To get the frame, the simplest way is to use a mdtPortManager.<br>
   * It's also possible to use mdtPort, but this solution needs to handle
   * the mutex, verify the readen queue state, ...
   * \sa mdtPortManager
   * \sa mdtPort
   */
  void newFrameReaden();

 private:

  // Get a new frame from RX frames pool
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_PORT_READ_THREAD_H
