/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_USB_PORT_THREAD_NEW_H
#define MDT_USB_PORT_THREAD_NEW_H

#include "mdtUsbDeviceDescriptor.h"
#include "mdtBasicStateMachine.h"
#include "mdtError.h"
#include <QObject>
#include <thread>
#include <atomic>
#include <mutex>
#include <libusb-1.0/libusb.h>


/*! \brief Thread for USB
 *
 * Handle USB event in a other thread than GUI one.
 */
class mdtUsbPortThreadNew : public QObject
{
 Q_OBJECT

 public:

  /*! \brief USB port thread state
   */
  enum class State_t
  {
    Stopped = 0,  /*!< Thread is stopped */
    Starting,     /*!< Thread is staring */
    Running,      /*!< Thread is running (ready for I/O) */
    Stopping,     /*!< Thread is stopping */
    Error         /*!< Thread is stopped due to a error */
  };

  /*! \brief Constructor
   *
   * Callable from GUI thread.
   *
   * \pre ctx must be a valid pointer.
   */
  mdtUsbPortThreadNew(libusb_context *ctx, QObject *parent = 0);

  /*! \brief Destructor
   *
   * Callable from GUI thread.
   */
  virtual ~mdtUsbPortThreadNew();

  // Disable copy
  mdtUsbPortThreadNew(const mdtUsbPortThreadNew & rhs) = delete;
  mdtUsbPortThreadNew & operator=(const mdtUsbPortThreadNew & rhs) = delete;

  /*! \brief Start thread
   *
   * Callable from GUI thread.
   */
  bool start(const mdtUsbDeviceDescriptor & deviceDescriptor, uint8_t bInterfaceNumber);

  /*! \brief Stop thread
   *
   * Callable from GUI thread.
   */
  void stop();

  /*! \brief Get last error
   */
  mdtError lastError()
  {
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    return pvLastError;
  }

 signals:

  /*! \brief Emitted by port thread when state has changed
   */
  void threadStateChanged();

  /*! \brief Emitted when a error occured (see specific subclass for details)
   */
  void errorOccured(const mdtError & error);

 protected:

  /*! \brief Set current state
   *
   * Will also emit threadStateChanged() signal.
   */
  void setCurrentState(State_t s){
    pvStateMachine.setCurrentState(s);
    emit threadStateChanged();
  }

  /*! \brief Get current state
   */
  inline State_t currentState() const
  {
    return pvStateMachine.currentState();
  }

  /*! \brief Access libusb context
   */
  inline libusb_context *usbContext(){
    return pvUsbContext;
  }

  /*! \brief Access libusb device handle
   *
   * Note: will be valid after successfull start(),
   *  and null after stop().
   */
  inline libusb_device_handle *deviceHandle()
  {
    return pvDeviceHandle;
  }

  /*! \brief Access device descriptor
   */
  const mdtUsbDeviceDescriptor & deviceDescriptor() const
  {
    return pvDeviceDescriptor;
  }

  /*! \brief Get requested bInterfaceNumber
   */
  uint8_t bInterfaceNumber() const
  {
    return pvbInterfaceNumber;
  }

  /*! \brief Wait until a state is reached or timeout without breacking main event loop
   *
   * \param s state to wait on
   * \param timeout Timeout [ms]. 0 means infinite timeout.
   * \return True if state was reached, false on timeout
   */
  bool waitOnState(State_t s, int timeout = 0);

  /*! \brief Wait until state is exited or timeout without breacking main event loop
   *
   * \param s state to wait on
   * \param timeout Timeout [ms]. 0 means infinite timeout.
   * \return True if state was reached, false on timeout
   */
  bool waitOnStateElseThan(State_t s, int timeout = 0);

  /*! \brief Open device
   *
   * Will open device that was passed to start().
   *  Subclass can use this helper function.
   *
   * Should be called from port thread ( in run() ).
   */
  bool openDevice();

  /*! \brief Actions to perform before device closes
   *
   * This function is called from stop(),
   *  and is executed in GUI thread.
   *  Should be non blocking.
   *
   * This default implementation does nothing.
   */
  virtual void aboutToClose();

  /*! \brief Called from port thread.
   *
   * Specific sub-class must implement this function.
   *
   * Default implementation is minimal and does nothing really
   *  interresting (just used for unit testing).
   */
  virtual void run();

  /*! \brief Last error object
   *
   * See mdtError documentation for details.
   *  A lock_guard that acts on pvLastErrorMutex should be used before modifying the error.
   */
  mdtError pvLastError;

  /*! \brief Mutext to serialize pvLastError access.
   */
  std::mutex pvLastErrorMutex;

 private slots:

  /*! \brief Tell that port thread state has changed
   *
   * Does nothing, but this generates a event
   *  that unblock QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents).
   */
  void onThreadStateChanged();

  /*! \brief Set wait timeout flag
   *
   * Used by waitOnState() and waitOnStateElseThan().
   */
  void setWaitTimeoutFlag();

 private:

  std::thread pvThread;
  mdtBasicStateMachine<State_t> pvStateMachine;
  libusb_context *pvUsbContext;
  libusb_device_handle *pvDeviceHandle;
  mdtUsbDeviceDescriptor pvDeviceDescriptor;
  uint8_t pvbInterfaceNumber;
  bool pvWaitOnStateTimedOut; // Used by waitOnState() and waitOnStateElseThan()
};

#endif // #ifndef MDT_USB_PORT_THREAD_NEW_H
