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
#include <QObject>
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
  class Logger : public QObject
  {
   Q_OBJECT

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
     *
     * \code
     * using Mdt::ErrorLogger::Logger;
     *
     * auto backend = Logger::addBackend<FileBackend>(Logger::ExecuteInSeparateThread);
     * backend->setLogFilePath("some/path/to/logfile");
     * \endcode
     *
     * A backend of type T is instanciated and added to the
     *  list of backends runing on specified \a executionThread .
     *  A pointer to the created backend is returned,
     *  so that some setup can be done on the backend.
     *  The logger has the ownership of the backend
     *  (it will delete it).
     *
     * \warn Accessing the backend referenced by the returned pointer is only possible:
     *        - If it runs on separate thread, before any error is logged ( by calling logError() )
     *        - For all cases, before calling cleanup()
     */
    template<typename T>
    static T *addBackend(ExecutionThread executionThread)
    {
      switch(executionThread){
        case ExecuteInMainThread:
        {
          instance().mMainThreadBackends.emplace_back( std::make_unique<T>() );
          auto *backend = reinterpret_cast<T*>( instance().mMainThreadBackends.back().get() );
          connect(&instance(), &Logger::errorSubmitted, backend, &T::logError, Qt::AutoConnection);
          return backend;
        }
        case ExecuteInSeparateThread:
          instance().mSeparateThreadBackends.emplace_back( std::make_unique<T>() );
          return reinterpret_cast<T*>( instance().mSeparateThreadBackends.back().get() );
      }
      // Should not happen
      return nullptr;
    }

    /*! \brief Log given error
     *
     * This function is thread safe
     *
     * \pre \a error must not be null
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

    /*! \internal Stop worker thread
     *
     * Used for unit tests
     */
    static void stopForTest();

   signals:

    /*! \internal Emitted whenever logError() was called
     */
    void errorSubmitted(const Mdt::Error & error);

   private:

    // Logger is a singleton
    Logger();

    /*! \brief Get unique instance of error logger
     */
    static Logger & instance();

    /*! \brief Enqueue error and start thread if needed
     */
    void logErrorToSeparateThread(const Error & error);

    /*! \brief Start worker thread
     */
    void start();

    /*! \internal Stop worker thread
     */
    void stop();

    /*! \brief Take a error from queue
     *
     * Returns a null error if queue was empty
     */
    Error takeError();

    /*! \brief Output error to each backend
     */
    void outputErrorToSeparateThreadBackends(const Error & error);

    /*! \brief Worker thread function
     */
    void run();

    enum Event
    {
      NoEvent,
      ErrorsToLog,
      End
    };

    std::mutex mMutex;
    std::condition_variable mCv;
    Event mEventForThread;
    bool mAllErrorsLogged;
    std::queue<Error> mErrorQueue;
    std::vector< std::unique_ptr<Backend> > mMainThreadBackends;
    std::vector< std::unique_ptr<Backend> > mSeparateThreadBackends;
    std::thread mThread;
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
   *       and logger's cleanup is also called in its destructor.
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
