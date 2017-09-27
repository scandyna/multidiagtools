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
#ifndef MDT_ERROR_LOGGER_H
#define MDT_ERROR_LOGGER_H

#include "Mdt/Error.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <queue>

/// \todo Check if a limit of number of errors in queue should be implemented

namespace Mdt{ namespace ErrorLogger {

  class Backend;

  /*! \brief Helper class to log Error objects
  */
  class Logger
  {
   public:

    /*! \brief Execution thread of a backend
     *
     * When adding a backend to the logger,
     *  it is possible to choose in which thread
     *  it must run.
     */
    enum ExecutionThread
    {
      ExecuteInMainThread,      /*!< The backend will run in the main thread.
                                      The Qt's signal/slot is used with a auto connection,
                                      so that errors logged using logError()
                                      will allways call Backend::logError()
                                      from the main thread event loop,
                                      regardless of the caller thread. */
      ExecuteInSeparateThread   /*!< The backend will run in logger's separate thread.
                                      A call to logError() will allways just queue the error
                                      and return. The logger's separated thread will then
                                      call Backend::logError(). */
    };

    // Disable copy and move
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger & operator=(const Logger &) = delete;
    Logger & operator=(Logger &&) = delete;

    /*! \brief Add a logger backend
     */
    static void addBackend(const std::shared_ptr<Backend> & backend);

    /*! \brief Add a logger backend
     *
     * \todo Document about ownership + do not touch returned pointer when logging a error
     */
    template<typename T>
    static T *addBackend(ExecutionThread executionThread)
    {
      switch(executionThread){
        case ExecuteInMainThread:
          instance().mMainThreadBackends.emplace_back( std::make_unique<T>() );
          /// \todo singal/slot connection once possible (in a separate method)
          return instance().mMainThreadBackends.back().get();
          break;
        case ExecuteInSeparateThread:
          instance().mSeparateThreadBackends.emplace_back( std::make_unique<T>() );
          return instance().mSeparateThreadBackends.back().get();
          break;
      }
      // Should not happen
      return nullptr;
    }

    /*! \brief Log given error
     *
     * This function is thread safe
     * \pre error must not be null
     */
    static void logError(const Error & error);

    /*! \brief Cleanup
    *
    * \note This function must be called before
    *       returning from main(). Not doing so
    *       conducts to undefined behaviour.
    *       Consider using a LoggerGuard.
    */
    static void cleanup();

  private:

    // mdtErrorLogger is a singleton
    Logger();

    /*! \brief Get unique instance of error logger
    */
    static Logger & instance();

    /*! \brief Enqueue error and start thread if needed
     */
    void logErrorImpl(const Error & error);

    /*! \brief Start worker thread
     */
    void start();

    /*! \brief Stop worker thread
     */
    void stop();

    /*! \brief Take a error from queue
     *
     * Returns a null error if queue was empty
     */
    Error takeError();

    /*! \brief Output error to each backend
     */
    void outputErrorToBackends(const Error & error);

    /*! \brief Worker thread function
     */
    void run();

    enum Event
    {
      NoEvent,
      ErrorsToLog,
      End
    };

    std::mutex pvMutex;
    std::condition_variable pvCv;
    Event pvEventForThread;
    bool pvAllErrorsLogged;
    std::queue<Error> pvErrorQueue;
    std::vector<std::shared_ptr<Backend>> pvBackends;
    std::vector< std::unique_ptr<Backend> > mMainThreadBackends;
    std::vector< std::unique_ptr<Backend> > mSeparateThreadBackends;
    std::thread pvThread;
  };

  /*! \brief Scope guard for error Logger
   *
   * Typical usage:
   * \code
   * int main()
   * {
   *   using Mdt::Error::Logger;
   *   using Mdt::Error::LoggerGuard;
   *
   *   LoggerGuard loggerGuard;
   * 
   *   // .. application code
   * 
   *   // Here, Logger::cleanup() will be called by loggerGuard.
   * }
   * \endcode
   *
   * \note When using one of the Mdt Application,
   *       the error logger is initialized,
   *       and loggers cleanup is also called in its destructor.
   *
   * \sa Mdt::CoreApplication
   * \sa Mdt::SingleCoreApplication
   * \sa Mdt::Application
   * \sa Mdt::SingleApplication
   */
  class LoggerGuard
  {
  public:

    /*! \brief Constructor
    */
    LoggerGuard();

    /*! \brief Destructor
    *
    * Will call cleanup on error logger
    */
    ~LoggerGuard();
  };

}}  // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_H
