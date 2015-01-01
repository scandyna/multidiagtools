/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtUsbPortThreadNew.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QTimer>


//#include <QDebug>

mdtUsbPortThreadNew::mdtUsbPortThreadNew(libusb_context *ctx, QObject *parent)
 : QObject(parent),
   pvStateMachine(State_t::Stopped),
   pvUsbContext(ctx),
   pvDeviceHandle(0),
   pvbInterfaceNumber(0)
{
  Q_ASSERT(pvUsbContext != 0);
  connect(this, SIGNAL(threadStateChanged()), this, SLOT(onThreadStateChanged()));
}

mdtUsbPortThreadNew::~mdtUsbPortThreadNew()
{
  stop();
}

bool mdtUsbPortThreadNew::start(const mdtUsbDeviceDescriptor & deviceDescriptor, uint8_t bInterfaceNumber)
{
  pvStateMachine.setCurrentState(State_t::Starting);
  // Store given device settings
  pvDeviceDescriptor = deviceDescriptor;
  pvbInterfaceNumber = bInterfaceNumber;
  // Start port thread and wait until starting finishes
  pvThread = std::thread(&mdtUsbPortThreadNew::run, std::ref(*this));
  waitOnStateElseThan(State_t::Starting);
  // Check current state and return result regarding it
  if(pvStateMachine.currentState() != State_t::Running){
    stop();
    return false;
  }

  return true;
}

void mdtUsbPortThreadNew::stop()
{
  setCurrentState(State_t::Stopping);
  // Close device if one was open
  if(pvDeviceHandle != 0){
    aboutToClose();
    int err = libusb_release_interface(pvDeviceHandle, pvbInterfaceNumber);
    if(err != 0){
      std::lock_guard<std::mutex> lg(pvLastErrorMutex);
      pvLastError.setError(tr("Could not release interface."), mdtError::Warning);
      pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbPortThreadNew");
      pvLastError.commit();
    }
    libusb_close(pvDeviceHandle);
    pvDeviceHandle = 0;
  }
  // Device is closed, join should not block to long
  if(pvThread.joinable()){
    pvThread.join();
  }
}

bool mdtUsbPortThreadNew::waitOnState(mdtUsbPortThreadNew::State_t s, int timeout)
{
  pvWaitOnStateTimedOut = false;
  if(timeout > 0){
    QTimer::singleShot(timeout, this, SLOT(setWaitTimeoutFlag()));
  }
  while(currentState() != s){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    if(pvWaitOnStateTimedOut){
      return false;
    }
  }
  return true;
}

bool mdtUsbPortThreadNew::waitOnStateElseThan(mdtUsbPortThreadNew::State_t s, int timeout)
{
  pvWaitOnStateTimedOut = false;
  if(timeout > 0){
    QTimer::singleShot(timeout, this, SLOT(setWaitTimeoutFlag()));
  }
  while(currentState() == s){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    if(pvWaitOnStateTimedOut){
      return false;
    }
  }
  return true;
}

bool mdtUsbPortThreadNew::openDevice()
{
  Q_ASSERT(pvUsbContext != 0);
  Q_ASSERT(!pvDeviceDescriptor.isEmpty());

  int err;

  // Open device
  pvDeviceHandle = pvDeviceDescriptor.open();
  if(pvDeviceHandle == 0){
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    pvLastError = pvDeviceDescriptor.lastError();
    return false;
  }
  Q_ASSERT(pvDeviceHandle != 0);
  // Tell libusb that we let him detach possibly loaded kernel driver itself
  err = libusb_set_auto_detach_kernel_driver(pvDeviceHandle, 1);
  if(err != 0){
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    pvLastError.setError(QObject::tr("Your platform does not support kernel driver detach."), mdtError::Warning);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbPortThreadNew");
    pvLastError.commit();
  }
  // Claim interface
  err = libusb_claim_interface(pvDeviceHandle, pvbInterfaceNumber);
  if(err != 0){
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    pvLastError.setError(QObject::tr("Could not claim interface."), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbPortThreadNew");
    pvLastError.commit();
    libusb_close(pvDeviceHandle);
    pvDeviceHandle = 0;
    return false;
  }

  return true;
}

void mdtUsbPortThreadNew::aboutToClose()
{
}

void mdtUsbPortThreadNew::run()
{
  int err;

  // Open USB device
  if(!openDevice()){
    setCurrentState(State_t::Error);
    return;
  }
  // Tell main thread that we are now running and run..
  setCurrentState(State_t::Running);
  while(currentState() == State_t::Running){
    err = libusb_handle_events(pvUsbContext);
    if(err != 0){
      std::lock_guard<std::mutex> lg(pvLastErrorMutex);
      pvLastError.setError(QObject::tr("USB event handling failed (libusb_handle_events())"), mdtError::Error);
      pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbPortThreadNew");
      pvLastError.commit();
      setCurrentState(State_t::Error);
      return;
    }
    // Some event woke us up - Check that it was not a stop request
    if(currentState() != State_t::Running){
      break;
    }
  }
  // Tell main thread that we are now stopped
  setCurrentState(State_t::Stopped);
}

void mdtUsbPortThreadNew::onThreadStateChanged()
{
}

void mdtUsbPortThreadNew::setWaitTimeoutFlag()
{
  pvWaitOnStateTimedOut = true;
}
