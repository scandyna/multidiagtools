/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ABSTRACT_PORT_H
#define MDT_ABSTRACT_PORT_H

#include "mdtFrame.h"
#include "mdtFrameModbusTcp.h"
#include "mdtFrameAscii.h"

#include "mdtPortConfig.h"
#include <QObject>
#include <QString>
#include <QQueue>
#include <QMutex>

/*! \brief Base class for port I/O
 */
class mdtAbstractPort : public QObject
{
 Q_OBJECT

 public:

  mdtAbstractPort(QObject *parent = 0);
  virtual ~mdtAbstractPort();

  /*! \brief Set the port attributes
   * 
   * Open the given port name and get his attributes.
   * This method must be re-implemented in subclass.
   * \param portName Name of the port to open (f.ex: /dev/ttyS0 , COM1, ...)
   */
  virtual bool setAttributes(const QString &portName) = 0;

  /*! \brief Open the port
   *
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Close previous opened ressource
   *  - Lock the mutex with lockMutex()
   *  - Do the specific work
   *  - Set the read/write timeouts. See the mdtPortConfig to know how to get these timeouts.
   *  - Call this open method (with mdtAbstractPort::open() ).
   * At this last step, the queues will be initialized, and mutex unocked.
   * \return True on successfull configuration and open port
   * \sa mdtPortConfig
   */
  virtual bool open(mdtPortConfig &cfg);

  /*! \brief Close the port
   *
   * This method must be re-implemented in subclass.<br>
   * To handle the port correctly, the subclass method must:
   *  - Lock the mutex with lockMutex()
   *  - Do the specific work
   *  - Call this close method (with mdtAbstractPort::close() ).
   * At this last step, the queues will be deleted, and mutex unocked.
   */
  virtual void close();

  /*! \brief Get the stored configuration
   */
  mdtPortConfig &config();

  /*! \brief Set the read data timeout
   *
   * This method must be re-implemented in subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setReadTimeout(int timeout) = 0;

  /*! \brief Set the write data timeout
   *
   * This method must be re-implemented in subclass.
   * The subclass can convert and store the value in system specific type
   * (f.ex: timeval struct on Posix)
   * \param timeout Timeout [ms]
   */
  virtual void setWriteTimeout(int timeout) = 0;

  /*! \brief Wait until data is available at device
   *
   * This method must be re-implemented in subclass.
   * The read timeout state must be updated with updateReadTimeoutState()<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Notes about mutex handling:
   *  - Caller: Mutex must be locked before calling this method.
   *  - Subclass implementation: Mutex must be released during wait, and relocked befor return.
   * \return False on error, in this case, the reader thread will emit errorOccured()
   * \sa mdtPortThread
   * \sa mdtTcpSocketThread
   */
  virtual bool waitForReadyRead() = 0;

  /*! \brief Wait until data is available at device
   *
   * This method calls setReadTimeout() and waitForReadyRead()
   * (it is a little bit slower than setting timeout one time, and call waitForReadyRead() ).<br>
   * Note that the reader thread will call waitForReadyRead() without argument.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex: see waitForReadyRead()
   * \return False on error, in this case, the reader thread will be stopped.
   * \sa waitForReadyRead()
   */
  bool waitForReadyRead(int msecs);

  /*! \brief Read data from port
   * 
   * This method must be implemented in subclass.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex is not handled by this method.
   * \return Number of bytes readen, or <0 on error
   */
  virtual qint64 read(char *data, qint64 maxSize) = 0;

  /*! \brief Just for special cases
   * 
   * In some case, this method is implemented in subclass.
   * Default implementation does nothing.
   * \sa mdtUsbtmcPort
   */
  virtual void readOneFrame();

  /*! \brief Just for special cases
   * 
   * In some case, this method is implemented in subclass.
   * Default implementation does nothing.
   * \sa mdtUsbtmcPort
   */
  virtual void writeOneFrame();

  /*! \brief Wait until data can be written to device
   *
   * This method must be re-implemented in subclass.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Notes about mutex handling:
   *  - Caller: Mutex must be locked before calling this method.
   *  - Subclass implementation: Mutex must be released during wait, and relocked befor return.
   * \return False on error, in this case, the reader thread will be stopped.
   */
  virtual bool waitEventWriteReady() = 0;

  /*! \brief Write data to port
   * 
   * This method must be implemented in subclass.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex is not handled by this method.
   * \return Number of bytes written, or <0 on error
   */
  virtual qint64 write(const char *data, qint64 maxSize) = 0;

  /*! \brief Update the read timeout state
   *
   * This method must be called by system dependant waitEventRead() method
   * When the read timeout state chages, the signal readTimeoutStateChanged() is emited.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex is not handled by this method.
   */
  void updateReadTimeoutState(bool state);

  /*! \brief Update the write timeout state
   *
   * This method must be called by system dependant waitEventWriteReady() method
   * When the write timeout state chages, the signal writeTimeoutStateChanged() is emited.<br>
   * Note: this method is called from thread , and should not be used directly<br>
   * Mutex is not handled by this method.
   */
  void updateWriteTimeoutState(bool state);

  /*! \brief Returns read timeout state
   * 
   * Mutex is not handled by this method.
   */
  bool readTimeoutOccured();

  /*! \brief Returns write timeout state
   * 
   * Mutex is not handled by this method.
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
  // Configuration
  mdtPortConfig pvConfig;
  // Attributes
  QString pvName;     // Port name, like /dev/ttyS0 , COM1, ...
  // mutex
  QMutex pvMutex;
};

#endif  // #ifndef MDT_ABSTRACT_PORT_H
