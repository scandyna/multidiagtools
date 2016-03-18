/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ERROR_LOGGER_CONSOLE_BACKEND_H
#define MDT_ERROR_LOGGER_CONSOLE_BACKEND_H

#include "LoggerBackend.h"
#include <QString>

namespace mdt{ namespace error {

  /*! \brief Console backend for error Logger
   */
  class LoggerConsoleBackend : public LoggerBackend
  {
   public:

    /*! \brief Destructor
     */
    ~LoggerConsoleBackend();

    /*! \brief Log given error
     */
    void logError(const mdtError & error);

   private:

    /*! \brief Get error informations
     */
    QString getErrorString(const mdtError & error) const;

    /*! \brief Get error stack string
     */
    QString getErrorStackString(const mdtError & error) const;
  };

}}  // namespace mdt{ namespace error {

#endif // #ifndef MDT_ERROR_LOGGER_CONSOLE_BACKEND_H
