#ifndef MDT_PORT_WRITE_THREAD_H
#define MDT_PORT_WRITE_THREAD_H

#include "mdtPortThread.h"
#include <QObject>

class mdtPortWriteThread : public mdtPortThread
{
 Q_OBJECT

 public:

  mdtPortWriteThread(QObject *parent = 0);

 signals:

  /*! \brief Emited when a complete frame was written
   * \sa mdtPortManager
   * \sa mdtPort
   */
  void frameWritten();

 private:

  // Get a new frame from TX frames
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_PORT_WRITE_THREAD_H
