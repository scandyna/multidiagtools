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

#include <mutex>

namespace mdt{ namespace error {

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

    std::mutex pvMutex;
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
