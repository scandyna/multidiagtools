#ifndef MDT_ABSTRAC_TDEVICE_FILE_H
#define MDT_ABSTRAC_TDEVICE_FILE_H

#include "mdtFrame.h"
#include "mdtDeviceFileConfig.h"
#include <QObject>
#include <QQueue>
#include <QMutex>

class mdtAbstractDeviceFile : public QObject
{
 Q_OBJECT

 public:

  mdtAbstractDeviceFile(QObject *parent = 0);
  ~mdtAbstractDeviceFile();

  /*! \brief Open the device file
   *
   * This method must be re-implemented in plateform specific subclass.
   * \param path Path to the device file
   * \return True on successfull configuration and open port
   */
  virtual bool open(mdtDeviceFileConfig &cfg) = 0;

  /*! \brief Close the device file
   *
   * This method must be re-implemented in plateform specific subclass.
   */
  virtual void close() = 0;

  /*! \brief Set the read data timeout
   *
   * This method must be re-implemented in plateform specific subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setReadTimeout(int timeout) = 0;

  /*! \brief Set the write data timeout
   *
   * This method must be re-implemented in plateform specific subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setWriteTimeout(int timeout) = 0;

  /*! \brief Wait until data is available at device
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the reader thread will be stopped.
   */
  virtual bool waitEventRead() = 0;

  /*! \brief Read data from device
   * 
   * This method must be implemented in plateform specific subclass.
   * \return Number of bytes that were really readen
   */
  virtual int readData(char *data, int maxLen) = 0;

  /*! \brief Flush the read (inut) flow
   * 
   * This method must be re-implemented in plateform specific subclass.
   */
  virtual void flushIn() = 0;

  /*! \brief Wait until data can be written to device
   *
   * This method must be re-implemented in plateform specific subclass.
   * \return False on error, in this case, the reader thread will be stopped.
   */
  virtual bool waitEventWriteReady() = 0;

  /*! \brief Write data to device
   * 
   * This method must be implemented in plateform specific subclass.
   * \return Number of bytes that were really written
   */
  virtual int writeData(const char *data, int maxLen) = 0;

  /*! \brief Flush the write (output) flow
   * 
   * This method must be re-implemented in plateform specific subclass.
   */
  virtual void flushOut() = 0;

  /*! \brief Update the read timeout state
   *
   *  This method must be called by system dependant waitEventRead() method
   *  When the read timeout state chages, the signal readTimeoutStateChanged() is emited
   */
  void updateReadTimeoutState(bool state);

  /*! \brief Update the write timeout state
   *
   *  This method must be called by system dependant waitEventWriteReady() method
   *  When the write timeout state chages, the signal writeTimeoutStateChanged() is emited
   */
  void updateWriteTimeoutState(bool state);

  /*! \brief Returns read timeout state
   * 
   * Note: this method locks the internal mutex
   */
  bool readTimeoutOccured();

  /*! \brief Returns write timeout state
   * 
   * Note: this method locks the internal mutex
   */
  bool writeTimeoutOccured();

  /*! \brief Get the readen frames Queue
   *
   * Readen frames queue contains frames that where received from device
   * Note that the mutex is not handled by this method
   */
  QQueue<mdtFrame*> &readenFrames();

  /*! \brief Get the read frames Queue pool
   *
   * Read frames queue pool contains frames that are ready to use for reception
   * Note that the mutex is not handled by this method
   */
  QQueue<mdtFrame*> &readFramesPool();

  /*! \brief Get the write frames Queue
   *
   * Write frames queue contains frames that must be sent
   * Note that the mutex is not handled by this method
   */
  QQueue<mdtFrame*> &writeFrames();

  /*! \brief Get the write frames Queue pool
   *
   * Write frames queue pool contains frames that are ready to use for transmission
   * Note that serialPort mutex is not handled by this method
   */
  QQueue<mdtFrame*> &writeFramesPool();

  /*! \brief Lock the mutex
   */
  void lockMutex();

  /*! \brief Unlock the mutex
   */
  void unlockMutex();

 signals:

  /*!  \brief Emited when read timeout state changed
   */
  void readTimeoutStateChanged(bool state);

  /*!  \brief Emited when write timeout state changed
   */
  void writeTimeoutStateChanged(bool state);

 protected:

  bool pvReadTimeoutOccured;
  bool pvReadTimeoutOccuredPrevious;
  bool pvWriteTimeoutOccured;
  bool pvWriteTimeoutOccuredPrevious;
  // Frames queues
  QQueue<mdtFrame*> pvReadenFrames;
  QQueue<mdtFrame*> pvReadFramesPool;
  QQueue<mdtFrame*> pvWriteFrames;
  QQueue<mdtFrame*> pvWriteFramesPool;

 private:

  // mutex
  QMutex pvMutex;
};

#endif  // #ifndef MDT_ABSTRAC_TDEVICE_FILE_H
