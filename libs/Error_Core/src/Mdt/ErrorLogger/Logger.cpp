/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "Logger.h"
#include "Backend.h"
#include <QCoreApplication>
#include <QDebug>

namespace Mdt{ namespace ErrorLogger {

void Logger::logError(const Error & error)
{
  Q_ASSERT(!error.isNull());

  emit instance().errorSubmitted(error);
  instance().logErrorToSeparateThread(error);
}

void Logger::cleanup()
{
  instance().stop();
  for(auto & backend : instance().mSeparateThreadBackends){
    backend->cleanup();
  }
  instance().mSeparateThreadBackends.clear();
  if(!instance().mSeparateThreadBackends.empty()){
    qWarning() << "Mdt::Error::Logger::cleanup(): not all errors could be logged from backend running in separate thread (this is a bug)";
  }
  for(auto & backend : instance().mMainThreadBackends){
    backend->cleanup();
  }
  instance().mMainThreadBackends.clear();
  if(!instance().mMainThreadBackends.empty()){
    qWarning() << "Mdt::Error::Logger::cleanup(): not all errors could be logged from backend running in main thread (this is a bug)";
  }
}

void Logger::stopForTest()
{
  instance().stop();
}

Logger::Logger()
{
}

Logger& Logger::instance()
{
  static Logger logger;
  return logger;
}

void Logger::logErrorToSeparateThread(const Error & error)
{
  Q_ASSERT(!error.isNull());

  // Queue error and wake up thread
  {
    std::unique_lock<std::mutex> lock(mMutex);
    // Start thread if needed
    if(!mThread.joinable()){
      start();
    }
    mErrorQueue.push(error);
    mEventForThread = ErrorsToLog;
    mAllErrorsLogged = false;
  }
  mCv.notify_one();
}

void Logger::start()
{
  Q_ASSERT(!mThread.joinable());

  mEventForThread = NoEvent;
  mAllErrorsLogged = false;
  mThread = std::thread(&Logger::run, std::ref(*this));
}

void Logger::stop()
{
  // If thread was never started, nothing to do
  if(!mThread.joinable()){
    return;
  }
  // Wait until all errors are logged to separate thread backends
  {
    std::unique_lock<std::mutex> lock(mMutex);
    if(!mAllErrorsLogged){
      mCv.wait(lock, [=]{return mAllErrorsLogged;});
    }
    // Tell thread that it must stop
    mEventForThread = End;
    mCv.notify_one();
  }
  // Join thread
  if(mThread.joinable()){
    mThread.join();
  }
  // Wait until all errors are logged to main thread backends
  QCoreApplication::processEvents(QEventLoop::AllEvents);
}

Error Logger::takeError()
{
  Error error;
  std::unique_lock<std::mutex> lock(mMutex);

  if(mErrorQueue.empty()){
    return error;
  }
  error = mErrorQueue.front();
  mErrorQueue.pop();

  return error;
}

void Logger::outputErrorToSeparateThreadBackends(const Error & error)
{
  Q_ASSERT(!error.isNull());

  for(const auto & backend : mSeparateThreadBackends){
    Q_ASSERT(backend);
    backend->logError(error);
  }
}

void Logger::run()
{
  Error error;
  bool running = true;

  // Work..
  while(running){
    /*
     * Wait for new job or end:
     * We must lock the mutex before wait.
     * wait will then relese it.
     * Once wait returns, the mutex is locked again.
     * We only want to lock it to take a error from queue,
     * not all the time.
     */
    {
      std::unique_lock<std::mutex> lock(mMutex);
      mCv.wait(lock, [=]{return mEventForThread != NoEvent;});
    }
    // Log all available errors
    do{
      error = takeError();
      if(!error.isNull()){
        outputErrorToSeparateThreadBackends(error);
      }
    }while(!error.isNull());
    // Notify that we logged all available errors
    {
      std::unique_lock<std::mutex> lock(mMutex);
      // Check special case of stopping
      if(mEventForThread == End){
        running = false;
      }else{
        mEventForThread = NoEvent;
      }
      mAllErrorsLogged = true;
    }
    mCv.notify_one();
  }
}

/*
 * mdtErrorLoggerGuard implementation
 */

LoggerGuard::LoggerGuard()
{
}

LoggerGuard::~LoggerGuard()
{
  Logger::cleanup();
}

}}  // namespace Mdt{ namespace Error {
