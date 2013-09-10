/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_USB_PORT_MANAGER_H
#define MDT_USB_PORT_MANAGER_H

#include "mdtPortManager.h"
#include "mdtFrameUsbControl.h"

/*! \brief Helper class that simplify access to USB port API
 *
 * Based on mdtPortManager, some USB specific methods are added.
 *  The internal configuration (a mdtPortConfig object)
 *  is keeped default.
 */
class mdtUsbPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Contruct a USB port manager
   *
   * Will create mdtUsbPort and setup.
   *  Thread will be created at first call of openPort().
   */
  mdtUsbPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  virtual ~mdtUsbPortManager();

  /*! \brief Get a list of attached USB devices
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Port manager must be closed
   */
  virtual QList<mdtPortInfo*> scan();

  /*! \brief Get a list of attached USB devices
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \param bDeviceClass If >= 0, only devices with matching bDeviceClass are listed
   * \param bDeviceSubClass If >= 0, only devices with matching bDeviceSubClass are listed
   * \param bDeviceProtocol If >= 0, only devices with matching bDeviceProtocol are listed
   *
   * \pre Port manager must be closed
   */
  QList<mdtPortInfo*> scan(int bDeviceClass, int bDeviceSubClass, int bDeviceProtocol);

  /*! \brief Open the port
   *
   * Overloads mdtPortManager::start().
   *
   * Will construct the thread if not allready exists.
   *  This is because mdtUsbtmcPortManager uses a different thread,
   *  so this task can not be done in constructor.
   *
   * Internally, mdtPortManager::start() is called.
   */
  bool start();

  /*! \brief Check if port manager is ready
   *
   * Because mdtUsbPortManager is a base class,
   *  it acts like mdtPortManager::isReady() .
   * A specific subclass (like mdtUsbtmcPortManager will reimplement isReady() ).
   */

  /*! \brief Request port to read until a short frame is received
   *
   * This is, for example, used by USBTMC bulk in abort process.
   *  If such request is pending, thread will read
   *  until a short frame is received.
   *
   * Will send the request and wait until it is finished
   *  (will not break GUI event loop).
   *
   * \pre Port must be set with setPort() with a mdtUsbPort (or subclass) before use of this method.
   */
  void readUntilShortPacketReceived();

  /*! \brief Send a control request by copy
   *
   * Request will be passed to the mdtPort's control request queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param frame Frame containing request to send.
   * \param setwIndexAsbInterfaceNumber If true, wIndex will be set with USB port's current bInterfaceNumber
   * \return 0 on success or value < 0 on error. In this implementation,
   *          the only possible error is mdtAbstractPort::WriteQueueEmpty .
   *          Some subclass can return a frame ID on success,
   *          or a other error. See subclass documentation for details.
   * \pre Port must be set with setPort() with a mdtUsbPort (or subclass) before use of this method.
   *
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   *  Typically usefull if a frame ID must be generated for each request (f.ex. bTag on USBTMC).
   *  A frame must be taken from port's request frames pool with mdtAbstractPort::controlFramesPool()
   *  dequeue() method (see Qt's QQueue documentation for more details on dequeue() ),
   *  then added to port's request queue with mdtAbstractPort::addControlRequest() .
   *  If protocol supports frame identification (like USBTMC's bTag),
   *   it should be returned here and incremented.
   */
  virtual int sendControlRequest(const mdtFrameUsbControl &request, bool setwIndexAsbInterfaceNumber = false);

  /*! \brief Wait until a control response is readen
   *
   * This method will return when a control response was readen.
   *
   * Internally, a couple of sleep and process event are called, so 
   * Qt's event loop will not be broken.
   *
   * This method can return if timeout occurs, or for other
   *  reason depending on specific port (port timeout, read canceled, ...).
   *
   * \return True if Ok, false on timeout
   */
  bool waitReadenControlResponse();

  /*! \brief Get all readen control responses
   *
   * Note that calling this methode will clear the internal queue.
   *  (A second call will return a empty list).
   */
  QList<mdtFrameUsbControl> readenControlResponses();

  /*! \brief Flush input buffers
   *
   * \param flushPortManagerBuffers If true, port manager buffers will be cleared.
   * \param flushPortBuffers If true, port's buffers will be cleared.
   * \pre Port must be set with setPort() before calling this method.
   */
  void flushIn(bool flushPortManagerBuffers, bool flushPortBuffers);

  /*! \brief Flush output buffers
   *
   * \param flushPortManagerBuffers If true, port manager buffers will be cleared.
   *                                 (Has no effect in this implementation, but some subclass can have output buffers).
   * \param flushPortBuffers If true, port's buffers will be cleared.
   * \pre Port must be set with setPort() before calling this method.
   */
  void flushOut(bool flushPortManagerBuffers, bool flushPortBuffers);

 public slots:

  /*! \brief Called from USB thread when a control reply was received
   *
   * Will add a copy of received control responses to internal queue
   *  and restore frame back to port's pool.
   */
  void fromThreadControlResponseReaden();

  /*! \brief Called from USB thread when a message IN was received ( on additionnal interrupt IN endpoint).
   */
  void fromThreadMessageInReaden();

  /*! \brief Called by thread when the read until a short packet process is finished
   */
  void fromThreadReadUntilShortPacketReceivedFinished();

 protected slots:

  /*! \brief Manage errors comming from usb port thread
   *
   * This implementation will handle some USB specific errors,
   *  change the current state emiting transistion signal.
   *  If state has changed, stateChanged() signal is emited.
   *
   * Some errors are handled here. For others, mdtPortManager::onThreadsErrorOccured() is called.
   */
  void onThreadsErrorOccured(int error);

 private:

  Q_DISABLE_COPY(mdtUsbPortManager);

  QQueue<mdtFrameUsbControl> pvReadenControlResponses;  // Copy of received control responses
  bool pvReadUntilShortPacketReceivedFinished;
};

#endif  // #ifndef MDT_USB_PORT_MANAGER_H
