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

void Logger::logError(const mdtErrorV2 & error)
{
  Q_ASSERT(!error.isNull());
  instance().logErrorImpl(error);
}

void Logger::cleanup()
{
  instance().stop();
  instance().pvBackends.clear();
}

Logger::Logger()
{
}

Logger& Logger::instance()
{
  static Logger logger;
  return logger;
}

void Logger::logErrorImpl(const mdtErrorV2 & error)
{
  Q_ASSERT(!error.isNull());

  // Start thread if needed
  if(!pvThread.joinable()){
    start();
  }
  // Queue error ond wake up thread
  std::unique_lock<std::mutex> lock(pvMutex);
  pvErrorQueue.push_back(error);
  pvNewWork.notify_one();
}

void Logger::start()
{
  Q_ASSERT(!pvThread.joinable());

  ///std::unique_lock<std::mutex> lock(pvMutex);
  // Create thread
  pvRunning = true;
  pvThread = std::thread(&Logger::run, std::ref(*this));
}

void Logger::stop()
{
  // Let thread log pending errors
  std::unique_lock<std::mutex> lock(pvMutex);
  while(!pvErrorQueue.empty()){
    qDebug() << "stop(): notify new work ...";
    pvNewWork.notify_one();
    qDebug() << "stop(): waiting job done ...";
    pvWorkDone.wait(lock);
  }
  // Now finish
  pvRunning = false;
  pvNewWork.notify_one();
  if(pvThread.joinable()){
    pvThread.join();
  }
}

mdtErrorV2 Logger::takeError()
{
  mdtErrorV2 error;
  std::unique_lock<std::mutex> lock(pvMutex);

  if(pvErrorQueue.empty()){
    return error;
  }
  error = pvErrorQueue.back();
  pvErrorQueue.pop_back();

  return error;
}

void Logger::run()
{
  // Take a error from queue
  mdtErrorV2 error;

  qDebug() << "started...";
  // Work..
  while(pvRunning){
    // Wait for new job or end
    qDebug() << "Going to wait ...";
    std::unique_lock<std::mutex> lock(pvMutex);
    pvNewWork.wait(lock);
    // Log all available errors
    do{
      error = takeError();
      // Output error for each backend
      for(const auto & backend : pvBackends){
        Q_ASSERT(backend);
        backend->logError(error);
      }
    }while(!error.isNull());
    // Notify work done
    pvWorkDone.notify_one();
  }

  
  qDebug() << "END";
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
