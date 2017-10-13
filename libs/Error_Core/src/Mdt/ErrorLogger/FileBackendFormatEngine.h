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
#ifndef MDT_ERROR_LOGGER_FILE_BACKEND_FORMAT_ENGINE_H
#define MDT_ERROR_LOGGER_FILE_BACKEND_FORMAT_ENGINE_H

#include "FormatEngine.h"
#include "Mdt/Error.h"
#include "MdtError_CoreExport.h"
#include <QCoreApplication>
#include <QString>

namespace Mdt{ namespace ErrorLogger {

  /*! \brief FileBackendFormatEngine is used by FileBackend to output a Mdt::Error
   */
  class MDT_ERROR_CORE_EXPORT FileBackendFormatEngine : public FormatEngine
  {
   Q_DECLARE_TR_FUNCTIONS(FileBackendFormatEngine)

   public:

    /*! \brief Constructor
     */
    FileBackendFormatEngine() = default;

    // Copy disabled
    FileBackendFormatEngine(const FileBackendFormatEngine &) = delete;
    FileBackendFormatEngine & operator=(const FileBackendFormatEngine &) = delete;
    // Move disabled
    FileBackendFormatEngine(FileBackendFormatEngine &&) = delete;
    FileBackendFormatEngine & operator=(FileBackendFormatEngine &&) = delete;

    /*! \brief Format \a error to its string representation
     */
    QString formatError(const Mdt::Error & error) const override;

   private:

    /*! \brief Get error level text
     */
    QString errorLevelText(Mdt::Error::Level level) const;

  };

}} // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_FILE_BACKEND_FORMAT_ENGINE_H
