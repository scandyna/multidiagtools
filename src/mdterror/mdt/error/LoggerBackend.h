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
#ifndef MDT_ERROR_LOGGER_BACKEND_H
#define MDT_ERROR_LOGGER_BACKEND_H

#include <QString>

class mdtErrorV2;

namespace mdt{ namespace error {

  /*! \brief Error Logger backend
   *
   * This class is a interface to create a error logger backend
   *  that will be used by error Logger to output errors.
   *
   * Notice that Logger executes from a non main thread,
   *  also take care that some functons must at least be reentrant.
   */
  class LoggerBackend
  {
   public:

    /*! \brief Constructor
     */
    LoggerBackend(){}

    /*! \brief Destructor
     */
    virtual ~LoggerBackend()
    {
    }

    /*! \brief Log given error
     *
     * This function must be reentrant, because its called from Logger thread
     *  (witch is not the main thread).
     */
    virtual void logError(const mdtErrorV2 & error) = 0;

    // Disable copy and move
    LoggerBackend(const LoggerBackend &) = delete;
    LoggerBackend(LoggerBackend &&) = delete;
    LoggerBackend & operator=(const LoggerBackend &) = delete;
    LoggerBackend & operator=(LoggerBackend &&) = delete;

   protected:

    /*! \brief Calls QObject::tr()
     */
    static QString tr(const char *text);
  };

}}  // namespace mdt{ namespace error {

#endif // #ifndef MDT_ERROR_LOGGER_BACKEND_H
