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
#include "Logger.h"
#include "LoggerBackend.h"
#include <QDebug>

namespace mdt{ namespace error {

void Logger::addBackend(const std::shared_ptr<LoggerBackend> & backend)
{
  Q_ASSERT(backend);
  instance().pvBackends.push_back(backend);
}

void Logger::logError(const mdtError & error)
{
  Q_ASSERT(!error.isNull());
  instance().logErrorImpl(error);
}

void Logger::cleanup()
{
  instance().stop();
  instance().pvBackends.clear();
  if(!instance().pvErrorQueue.empty()){
    qWarning() << "mdt::error::Logger::cleanup(): not all errors could be logged (this is a bug)";
  }
}

Logger::Logger()
{
}

Logger& Logger::instance()
{
  static Logger logger;
  return logger;
}

void Logger::logErrorImpl(const mdtError & error)
{
  Q_ASSERT(!error.isNull());

  // Queue error and wake up thread
  {
    std::unique_lock<std::mutex> lock(pvMutex);
    // Start thread if needed
    if(!pvThread.joinable()){
      start();
    }
    pvErrorQueue.push(error);
    pvEventForThread = ErrorsToLog;
    pvAllErrorsLogged = false;
  }
  pvCv.notify_one();
}

void Logger::start()
{
  Q_ASSERT(!pvThread.joinable());

  pvEventForThread = NoEvent;
  pvAllErrorsLogged = false;
  pvThread = std::thread(&Logger::run, std::ref(*this));
}

void Logger::stop()
{
  // If thread was never started, nothing to do
  if(!pvThread.joinable()){
    return;
  }
  // Wait until all errors are logged
  {
    std::unique_lock<std::mutex> lock(pvMutex);
    if(!pvAllErrorsLogged){
      pvCv.wait(lock, [=]{return pvAllErrorsLogged;});
    }
    // Tell thread that it must stop
    pvEventForThread = End;
    pvCv.notify_one();
  }
  // Join thread
  if(pvThread.joinable()){
    pvThread.join();
  }
}

mdtError Logger::takeError()
{
  mdtError error;
  std::unique_lock<std::mutex> lock(pvMutex);

  if(pvErrorQueue.empty()){
    return error;
  }
  error = pvErrorQueue.front();
  pvErrorQueue.pop();

  return error;
}

void Logger::outputErrorToBackends(const mdtError & error)
{
  Q_ASSERT(!error.isNull());

  for(const auto & backend : pvBackends){
    Q_ASSERT(backend);
    backend->logError(error);
  }
}

void Logger::run()
{
  mdtError error;
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
      std::unique_lock<std::mutex> lock(pvMutex);
      pvCv.wait(lock, [=]{return pvEventForThread != NoEvent;});
    }
    // Log all available errors
    do{
      error = takeError();
      if(!error.isNull()){
        outputErrorToBackends(error);
      }
    }while(!error.isNull());
    // Notify that we logged all available errors
    {
      std::unique_lock<std::mutex> lock(pvMutex);
      // Check special case of stopping
      if(pvEventForThread == End){
        running = false;
      }else{
        pvEventForThread = NoEvent;
      }
      pvAllErrorsLogged = true;
    }
    pvCv.notify_one();
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

}}  // namespace mdt{ namespace error {
