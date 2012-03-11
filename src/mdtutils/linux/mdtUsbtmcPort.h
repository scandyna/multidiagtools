#ifndef MDT_USBTMC_PORT_H
#define MDT_USBTMC_PORT_H


#include "mdtPort.h"
#include <QObject>
#include <QWaitCondition>
#include <QMutex>

/*! \brief USBTMC device file calss
 * 
 * With USBTMC, when we try to read, and no data are available,
 * both read and write threads are blocked.<br>
 * USBTMC seems not to handle select() call,
 * so we re-implement waitEventRead() and waitEventWriteReady().<br>
 * To start the read, call the readOneFrame() method.<br>
 * To start the write, call the writeOneFrame() method.
 */
class mdtUsbtmcPort : public mdtPort
{
 Q_OBJECT

 public:

  mdtUsbtmcPort(QObject *parent = 0);
  ~mdtUsbtmcPort();

  // This method is called the reader thread
  bool waitEventRead();

  // This method is called the reader thread
  bool waitEventWriteReady();

  // Tell the thread that a frame can be read
  void writeOneFrame();

 public slots:

  /*! \brief Tell the thread that a frame can be read
   * 
   * In mdtUsbtmcPortManager, this slot is connected to
   * the write thread, and called each time a frame ws written.<br>
   * If the waitingFrame flag is true, the read thread will be
   * notified that data must be read.
   * \sa setWaitingFrame()
   * \sa mdtPortWriteThread
   */
  void readOneFrame();

 private:

  QMutex pvReadWriteMutex;
  QWaitCondition pvReadCondition;
  QWaitCondition pvWriteCondition;
};

#endif  // #ifndef MDT_USBTMC_PORT_H
