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
#include <QMap>

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
 * m.setEnqueueReadenFrames(true);
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

  /*! \brief Set the enqueue readen frames flag
   *
   * If true, all incomming frames will be added in internal queue,
   *  and the readenFramesQueueSizeChanged() signal will be sent.
   *
   * By default this flag is false.
   */
  void setEnqueueReadenFrames(bool enqueue);

  /*! \brief Set the incomming frame notification flag
   *
   * If true, the newReadenFrame(int, QByteArray) signal
   *  will be emitted for each incomming frame.
   *
   * By default this flag is false.
   */
  void setNotifyNewReadenFrame(bool notify);

  /*! \brief Scan for available ports
   *
   * This method is implemented is port's specific subclass.
   *  Default implementation returns a empty list.
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
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
  void setPortName(const QString &portName);

  /*! \brief Set port info
   *
   * Store given port info, and call setPortName() with
   *  port info's stored port name (see mdtPortInfo::portName() ).
   *
   * Setting a port info can be usefull if other informations are
   *  needed later in application (f.ex. mdtPortInfo::displayText() ).
   * You can get port informations later with portInfo().
   */
  void setPortInfo(mdtPortInfo info);

  /*! \brief Get port info
   */
  mdtPortInfo portInfo();

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
   * \param data Data to write
   * \return 0 on success or value < 0 on error. In this implementation,
   *          the only possible error is mdtAbstractPort::WriteQueueEmpty .
   *          Some subclass can return a frame ID on success,
   *          or a other error. See subclass documentation for details.
   * \pre Port must be set with setPort() before use of this method.
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   *  Typically usefull if some encoding is needed before the
   *  frame is submitted to port.
   *  A frame must be taken from port's write frames pool with mdtAbstractPort::writeFramesPool()
   *  dequeue() method (see Qt's QQueue documentation for more details on dequeue() ),
   *  then added to port's write queue with mdtAbstractPort::addFrameToWrite() .
   *  If protocol supports frame identification (like MODBUS's transaction ID or USBTMC's bTag),
   *   it should be returned here and incremented.
   */
  virtual int writeData(QByteArray data);

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
   */
  bool waitReadenFrame(int timeout = 500);

  /*! \brief Wait on readen frame with defined ID
   *
   * Will return when frame with given ID was read or after timeout.
   *
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   *
   * \param id Frame ID. Depending on protocol, this can be a transaction ID or what else.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return Frame's data on success or empty QByteArray on timeout
   */
  QByteArray waitOnFrame(int id, int timeout = 500);

  /*! \brief Get data by frame ID
   *
   * The frame ID is a protocol specific identification.
   *  F.ex. in MODBUS/TCP, the transaction ID is used,
   *  or bTag for USBTMC.
   *
   * If found, the frame is removed from received queue.
   *
   * If ID was not found, a empty QByteArray is returned.
   */
  QByteArray readenFrame(int id);

  /*! \brief Get all readen data
   *
   * Get all currently available data.
   *  Once data are readen, don't forget to clear incoming
   *  frames list with clearReadenFrames().
   *
   * To prevent data loss, call clearReadenFrames() just after
   *  data read. Do not use QApplication::processEvent() before,
   *  because this could insert a new frame, wich will be cleared.
   *  (see slot fromThreadNewFrameReaden() ).
   *
   * Yout can iterate over returned list, like:
   * \code
   * int i;
   *
   * // Get all frames
   * for(i=0; i<manager.readenFrames().size(); i++){
   *   qDebug() << "Data[" << i << "]: " << manager.readenFrames().at(i);
   * }
   * // Clear readen frames
   * manager.clearReadenFrames();
   * \endcode
   * In this example, no deep copy of frames list should occur.
   *
   * A other way is to reference returned list to a temporary QList:
   * \code
   * QList<QByteArray> list;
   *
   * // Get all frames
   * list = manager.readenFrames();
   * for(i=0; i<list.size(); i++){
   *   qDebug() << "Data[" << i << "]: " << list.at(i);
   * }
   * // Clear readen frames
   * list.clear();
   * // Here, a deep copy occured, we must clear manager's internal list
   * manager.clearReadenFrames();
   * \endcode
   *
   * For more details you should read Qt's documentation about implicit-sharing.
   */
  const QList<QByteArray> readenFrames() const;

  /*! \brief Clear readen frames
   *
   * \sa readenFrames()
   */
  void clearReadenFrames();

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
  virtual void fromThreadNewFrameReaden();

  /*! \brief Manage errors comming from port threads
   */
  void onThreadsErrorOccured(int error);

 signals:

  /*! \brief Emitted when new frame was readen
   *
   * The id is the same than returned by writeData().
   *
   * Emitted only if notifyNewReadenFrame flag is true.
   *
   * \sa setNotifyNewReadenFrame()
   */
  void newReadenFrame(int id, QByteArray data);

  /*! \brief Emitted when internal queue of readen frames size has changed
   *
   * \sa readenFrame()
   * \sa readenFrames()
   */
  void readenFramesQueueSizeChanged(int newSize);

 protected:

  /*! \brief Store frame data in queue and/or emit newReadenFrame() signal
   *
   * Dependeing on pvEnqueueReadenFrames and pvNotifyNewReadenFrame,
   *  frame will be enqueued and/or the newReadenFrame(int, QByteArray) will be emitted.
   *
   * Subclass notes:<br>
   *  The subclass should use this method to handle new incomming frames.
   */
  void commitFrame(int id, QByteArray data);

  mdtAbstractPort *pvPort;
  QList<mdtPortThread*> pvThreads;

 private:

  mdtPortInfo pvPortInfo;
  quint16 pvLastReadenFrameId;    // Used if protocol does not contain a frame id.
  QMap<quint16, QByteArray> pvReadenFrames; // Hold a copy of each frame readen by port
  bool pvEnqueueReadenFrames;
  bool pvNotifyNewReadenFrame;

  // Diseable copy
  Q_DISABLE_COPY(mdtPortManager);
};
#endif  // #ifndef MDT_PORT_MANAGER_H
