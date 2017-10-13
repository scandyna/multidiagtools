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
#ifndef MDT_ERROR_LOGGER_FORMAT_ENGINE_H
#define MDT_ERROR_LOGGER_FORMAT_ENGINE_H

#include "MdtError_CoreExport.h"
#include <QString>

namespace Mdt{

 class Error;

 namespace ErrorLogger {

  /*! \brief FormatEngine is used by a Backend to output a Mdt::Error
   */
  class MDT_ERROR_CORE_EXPORT FormatEngine
  {
   public:

    /*! \brief Constructor
     */
    FormatEngine() = default;

    /*! \brief Destructor
     */
    virtual ~FormatEngine() = default;

    // Copy disabled
    FormatEngine(const FormatEngine &) = delete;
    FormatEngine & operator=(const FormatEngine &) = delete;
    // Move disabled
    FormatEngine(FormatEngine &&) = delete;
    FormatEngine & operator=(FormatEngine &&) = delete;

    /*! \brief Format \a error to its string representation
     *
     * This method must be reentrant, because it can be called from Logger thread
     *  (which is not the main thread).
     */
    virtual QString formatError(const Mdt::Error & error) const = 0;

  };

}} // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_FORMAT_ENGINE_H
