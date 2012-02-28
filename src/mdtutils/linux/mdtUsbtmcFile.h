#ifndef MDT_USBTMC_FILE_H
#define MDT_USBTMC_FILE_H


#include "mdtDeviceFile.h"
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
class mdtUsbtmcFile : public mdtDeviceFile
{
 public:

  mdtUsbtmcFile(QObject *parent = 0);
  ~mdtUsbtmcFile();

  // This method is called the reader thread
  bool waitEventRead();

  // Tell the thread that a frame can be read
  void readOneFrame();

  // This method is called the reader thread
  bool waitEventWriteReady();

  // Tell the thread that a frame can be read
  void writeOneFrame();

 private:

  QMutex pvReadWriteMutex;
  QWaitCondition pvReadCondition;
  QWaitCondition pvWriteCondition;
};

#endif  // #ifndef MDT_USBTMC_FILE_H
