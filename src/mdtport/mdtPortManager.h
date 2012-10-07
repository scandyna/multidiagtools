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
#ifndef MDT_PORT_MANAGER_H
#define MDT_PORT_MANAGER_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QThread>
#include "mdtAbstractPort.h"
#include "mdtPortConfig.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtError.h"

#include "mdtFrame.h"
#include <QByteArray>

#include <QList>
#include "mdtPortInfo.h"

class mdtPortThread;

/*! \brief Port manager base class
 *
 * Manages a port based on mdtAbstractPort an several threads based on mdtPortThread.
 *  The goal is to hide the complexity of the port API.
 *
 * Example:
 * \code
 * mdtPortManager m;
 * mdtPort *port;
 * mdtPortConfig *config;
 *
 * // Setup
 * config = new mdtPortConfig;
 * config->setFrameType(mdtFrame::FT_ASCII);
 * config->setEndOfFrameSeq("$");
 *
 * // Init port
 * port = new mdtPort;
 * port->setConfig(config);
 *
 * // Init port manager
 * m.setPort(port);
 * m.addThread(mew mdtPortWriteThread);
 * m.addThread(mew mdtPortReadThread);
 * m.setPortName("/dev/xyz"));
 * if(!m.openPort()){
 *  // Handle error
 * }
 *
 * // Start threads
 * if(!m.start()){
 *  // Handle error
 * }
 *
 * // Send some data
 * if(!m.writeData("Test$")){
 *  // Handle error
 * }
 *
 * // Wait on answer - Timout: 1500 [ms]
 * if(!m.waitReadenFrame(1500)){
 *  // Timout , handle error
 * }
 *
 * // Do something with received data
 * for(int i=0; i<m.readenFrames().size(); i++){
 *  qDebug() << m.readenFrames().at(i);
 * }
 *
 * // Cleanup - detachPort() will delete port and threads objects
 * m.detachPort(true, true);
 * delete config;
 *
 * \endcode
 *
 * A alternative of using waitReadenFrame() is to connect the newReadenFrame()
 *  signal to a slot, and get data with readenFrames() from this slot.
 *
 * \sa mdtSerialPortManager
 * \sa mdtUsbtmcPortManager (Linux only)
 */
class mdtPortManager : public QThread
{
 Q_OBJECT

 public:

  mdtPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * If a port was set, the manager will stop (if running), and port will be closed (if open).
   *
   * Note that port set by setPort() and threads are not deleted.
   */
  virtual ~mdtPortManager();

  /*! \brief Scan for available ports
   *
   * This method is implemented is port's specific subclass.
   *  Default implementation returns a empty list.
   *
   * \pre Manager must no running
   */
  virtual QList<mdtPortInfo*> scan();

  /*! \brief Set port object
   *
   * \pre port must be a valid pointer to the expected class instance (for ex: mdtSerialPort).
   * \pre Manager must no running
   */
  virtual void setPort(mdtAbstractPort *port);

  /*! \brief Detach port
   *
   * Will detach port from each thread and from port manager.
   *  If port was not set, this method does nothing.
   *  If manager is running, it will be stopped.
   *  If port is open, it will be closed.
   *
   * \param deletePort If true, the port object that was set with setPort() will be deleted.
   * \param deleteThreads If true, each thread added with addThread() will be deleted.
   */
  void detachPort(bool deletePort, bool deleteThreads);

  /*! \brief Add a thread and assign it to port
   *
   * \pre Port must be set with setPort before using this method
   * \pre Manager must no running
   * \pre thread must be a valid pointer
   */
  void addThread(mdtPortThread *thread);

  /*! \brief Detach threads from port and remove threads
   *
   * \param releaseMemory If true, all threads are deleted
   * \pre Port manager must not running.
   */
  void removeThreads(bool releaseMemory);

  /*! \brief Start threads
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  bool start();

  /*! \brief Get the running state
   *
   * If one of the threads is running, true is returned.
   *
   * If port was not set, it returns false.
   */
  bool isRunning();

  /*! \brief Stop threads
   * 
   * \pre Port must be set with setPort() before use of this method.
   */
  void stop();

  /*! \brief Set port name
   *
   * Set the port name to internally port object.
   * Does nothing else. To open the port, use openPort().
   *
   * \pre Port must be set before with setPort()
   * \sa mdtAbstractPort
   */
  virtual void setPortName(const QString &portName);

  /*! \brief Get the port's config object
   * 
   * Usefull to alter internal port configuration
   *
   * \pre Port must be set with setPort() before use of this method.
   */
  virtual mdtPortConfig &config();

  /*! \brief Open the port
   *
   * Will try to open port defined with setPortName().
   *
   * \return True on success, false else.
   * \pre Port must be set with setPort() before use of this method.
   */
  bool openPort();

  /*! \brief Close the port
   * 
   * This stops the threads (if exists) and close the port.
   *
   * If port was never set (with setPort() ), this method does nothing.
   */
  void closePort();

  /*! \brief Write data by copy
   *
   * Data will be passed to the mdtPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * Note that internal previous readen frames are cleared,
   *  so readenFrames() will return a empty list just after this call.
   *
   * \param data Data to write
   * \return True on success. False if write queue is full.
   * \pre Port must be set with setPort() before use of this method.
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   *  Typically usefull if some encoding is needed before the
   *  frame is submitted to port.
   *  A frame must be taken from port's write frames pool with mdtAbstractPort::writeFramesPool()
   *  dequeue() method (see Qt's QQueue documentation for more details on dequeue() ),
   *  wthen added to port's write queue with mdtAbstractPort::addFrameToWrite() .
   */
  virtual bool writeData(QByteArray data);

  /*! \brief Wait until a complete frame is available
   *
   * This method will return when a complete frame was readen.
   *  This is usefull for query/answer protocols.
   *
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   *
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True if Ok, false on timeout
   * \sa newReadenFrame()
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   */
  virtual bool waitReadenFrame(int timeout = 500);

  /*! \brief Get all readen data
   *
   * Get a copy of all currently available data.
   *  Note: the list of data must be cleared explicitly
   *   with QList::clear() after data are used.
   *   (or remove each item with, for.ex. QList::takeFirst() )
   */
  QList<QByteArray> &readenFrames();

  /*! \brief Wait some time without break the GUI's event loop
   *
   * This is a helper method that provide a blocking wait.
   *  Internally, a couple of sleep and event processing
   *  is done, avoiding freesing the GUI.
   *
   * This wait method is not precise.
   *
   * \param msecs Time to wait [ms]
   * \param granularity Sleep time between each call of event processing [ms]<br>
   *                     A little value needs more CPU and big value can freese the GUI.
   *                     Should be between 50 and 100, and must be > 0.
   *                     Note that msecs must be a multiple of granularity.
   * \pre granularity must be > 0.
   */
  void wait(int msecs, int granularity = 50);

 public slots:

  /*! \brief Cancel read and write operations
   *
   * Default implementation calls mdtAbstractPort::flush().
   */
  virtual void abort();

  /*! \brief Try to 
   * 
   */
  

  /*! \brief Called by the read thread whenn a complete frame was readen
   *
   * \sa mdtPortThread
   */
  void fromThreadNewFrameReaden();

  /*! \brief Manage errors comming from port threads
   */
  void onThreadsErrorOccured(int error);

 signals:

  /*! \brief Emitted when new frame was readen
   *
   * \sa waitReadenFrame()
   */
  void newReadenFrame();

 protected:

  mdtAbstractPort *pvPort;
  QList<mdtPortThread*> pvThreads;
  QList<QByteArray> pvReadenFrames;       // Hold a copy of each frame readen by port
  QList<QByteArray> pvReadenFramesCopy;   // Hold a copy of each frame readen by port, this will be returned by readenFrames()

 private:

  // Diseable copy
  Q_DISABLE_COPY(mdtPortManager);
};
#endif  // #ifndef MDT_PORT_MANAGER_H
