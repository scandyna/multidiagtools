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
#ifndef MDT_ERROR_LOGGER_H
#define MDT_ERROR_LOGGER_H

#include "mdtErrorV2.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <queue>

/// \todo Check if a limit of number of errors in queue should be implemented

namespace mdt{ namespace error {

  class LoggerBackend;

  /*! \brief Helper class to log mdtError objects
  */
  class Logger
  {
   public:

    // Disable copy and move
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger & operator=(const Logger &) = delete;
    Logger & operator=(Logger &&) = delete;

    /*! \brief Add a logger backend
     */
    static void addBackend(const std::shared_ptr<LoggerBackend> & backend);

    /*! \brief Log given error
     *
     * This function is thread safe
     * \pre error must not be null
     */
    static void logError(const mdtErrorV2 & error);

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
    void logErrorImpl(const mdtErrorV2 & error);

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
    mdtErrorV2 takeError();

    /*! \brief Output error to each backend
     */
    void outputErrorToBackends(const mdtErrorV2 & error);

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
    std::queue<mdtErrorV2> pvErrorQueue;
    std::vector<std::shared_ptr<LoggerBackend>> pvBackends;
    std::thread pvThread;
  };

  /*! \brief Scope guard for error Logger
  *
  * Typical usage:
  * \code
  * int main()
  * {
  *   using mdt::error::Logger;
  *   using mdt::error::LoggerGuard;
  *
  *   LoggerGuard loggerGuard;
  * 
  *   // .. application code
  * 
  *   // Here, Logger::cleanup() will be called by loggerGuard.
  * }
  * \endcode
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

}}  // namespace mdt{ namespace error {

#endif // #ifndef MDT_ERROR_LOGGER_H
