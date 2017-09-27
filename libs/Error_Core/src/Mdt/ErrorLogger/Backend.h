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
#ifndef MDT_ERROR_LOGGER_BACKEND_H
#define MDT_ERROR_LOGGER_BACKEND_H

#include <QObject>
#include <QString>

namespace Mdt{

 class Error;

 namespace ErrorLogger {

  /*! \brief Error Logger backend
   *
   * This class is a interface to create a error logger backend
   *  that will be used by error Logger to output errors.
   *
   * Notice that Logger can be executed from a non main thread,
   *  also take care that some functions must at least be reentrant.
   */
  class Backend : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    Backend(QObject *parent = nullptr);

    // Disable copy and move
    Backend(const Backend &) = delete;
    Backend(Backend &&) = delete;
    Backend & operator=(const Backend &) = delete;
    Backend & operator=(Backend &&) = delete;

    /*! \brief Log given error
     *
     * This function must be reentrant, because it can be called from Logger thread
     *  (witch is not the main thread).
     */
    virtual void logError(const Error & error) = 0;

  };

}}  // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_BACKEND_H
