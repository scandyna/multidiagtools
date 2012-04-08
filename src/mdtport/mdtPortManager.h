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
#include <QObject>
#include <QThread>
#include "mdtAbstractPort.h"
#include "mdtPortConfig.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtError.h"

#include "mdtFrame.h"
#include <QByteArray>

/*! \brief Port manager base class
 * 
 * This is the easiest way to use the port API.<br>
 * The read and write threads will be created by setPort() ,
 * and port affected to them.<br>
 * 
 * Example:
 * \code
 * mdtPortManager m;
 * 
 * // Setup
 * m.setConfig(new mdtPortConfig);
 * m.config().setFrameType(mdtFrame::FT_ASCII);
 * m.config().setEndOfFrameSeq("$");
 * 
 * // Init port manager
 * m.setPort(new mdtPort);
 * if(!m.setPortName("/dev/xyz")){
 *  // Handle error
 * }
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
 * // Do something with received data
 * qDebug() << m.lastReadenFrame();
 * 
 * // End
 * m.closePort();
 * 
 * \endcode
 * 
 * This is a base class for port manager.
 * Port type specific classes are available.
 * \sa mdtSerialPortManager
 * \sa mdtUsbtmcPortManager (Linux only)
 */
class mdtPortManager : public QThread
{
 Q_OBJECT

 public:

  mdtPortManager(QObject *parent = 0);
  ~mdtPortManager();

  /*! \brief Set port object
   * 
   * The read and write threads will be created if not allready exists,
   * port assigned to them, and signals/slots connection made.<br>
   * Please take care to never call this method while threads are running.
   * \pre port must be a valid pointer to the expected class instance (for ex: mdtSerialPort)
   */
  void setPort(mdtAbstractPort *port);

  /*! \brief Set port name
   *
   * Try to open given port. If ok, the port name
   * will be kept.<br>
   * mdtAbstractPort::setAttributes() will be called to fetch some attributes
   * (for example available baud rates if port is a mdtSerialPort object).
   * \pre Port must be set before with setPort()
   * \param portName Port to open
   * \return True on success, false else
   * \sa mdtAbstractPort
   */
  bool setPortName(const QString &portName);

  /*! \brief Set the config object
   * 
   * \pre config must be a valid pointer to expected a object type.
   */
  void setConfig(mdtPortConfig *config);

  /*! \brief Get the config object
   * 
   * Usefull to alter internal port configuration
   * \pre Config must be set with setConfig()
   */
  mdtPortConfig &config();

  /*! \brief Open the port
   * 
   * \return True on success, false else
   * \pre setPortName() must be called first
   * \pre Config must be set with setConfig()
   * \sa setPortName()
   * \sa mdtPort
   */
  virtual bool openPort();

  /*! \brief Close the port
   * 
   * This stops the threads (if exists) and close the port.<br>
   * If port was never set (with setPort() ), this method does nothing.
   */
  void closePort();

  /*! \brief Start the read thread
   * \return True on successfull start
   * \sa mdtThread
   * \sa mdtPortReadThread
   */
  bool startReading();

  /*! \brief Stop the read thread
   * 
   * \pre setPort() must be called before.
   * \sa startReading()
   */
  void stopReading();

  /*! \brief Start the write thread
   *
   * \pre setPort() must be called before.
   * \return True on successfull start
   * \sa mdtThread
   * \sa mdtPortWriteThread
   */
  bool startWriting();

  /*! \brief Stop the write thread
   * 
   * \pre setPort() must be called before.
   * \sa startWriting()
   */
  void stopWriting();

  /*! \brief Start reading and writing threads
   *
   * \pre setPort() must be called before.
   * \sa startReading()
   * \sa startWriting()
   */
  bool start();

  /*! \brief Stop reading and writing threads
   * 
   * \pre setPort() must be called before.
   * \sa start()
   */
  void stop();

  /*! \brief Write data by copy
   *
   * The data conatined in frame will be passed to the mdtPort's
   * write queue by copy. This method returns immediatly after enqueue,
   * and don't wait until data was written.
   * \param frame Frame to write
   * \return True on success. False if write queue is full
   */
  bool writeData(QByteArray data);

  /*! \brief Wait until a complete frame is available
   * 
   * This method will return when a complete frame was readen.<br>
   * This is usefull for query/answer protocols.<br>
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   * \param timeout Maximum wait time [ms]. Must be a multiple of 50 [ms]
   * \return True if Ok, false on timeout
   */
  bool waitReadenFrame(int timeout = 500);

  /*! \brief Get the last readen frame
   */
  QByteArray &lastReadenFrame();

 public slots:

  /*! \brief Called by the read thread whenn a complete frame was readen
   * \sa mdtPortReadThread
   */
  void newFrameReaden();

  /*! \brief Manage errors comming from port threads
   */
  void onThreadsErrorOccured(int error);

 protected:

  mdtAbstractPort *pvPort;
  mdtPortReadThread *pvReadThread;
  mdtPortWriteThread *pvWriteThread;
  mdtPortConfig *pvConfig;
  QByteArray pvLastReadenFrame;
  
 private:

  // Diseable copy
  mdtPortManager(mdtPortManager &other);
};
#endif  // #ifndef MDT_PORT_MANAGER_H
