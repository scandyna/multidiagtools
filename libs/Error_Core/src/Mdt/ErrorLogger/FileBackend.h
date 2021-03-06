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
#ifndef MDT_ERROR_LOGGER_FILE_BACKEND_H
#define MDT_ERROR_LOGGER_FILE_BACKEND_H

#include "Backend.h"
#include "Mdt/Error.h"
#include "MdtError_CoreExport.h"
#include <QString>

class QFile;

namespace Mdt{ namespace ErrorLogger {

  /*! \brief File backend for error Logger
   */
  class MDT_ERROR_CORE_EXPORT FileBackend : public Backend
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    FileBackend(QObject *parent = nullptr);

    /*! \brief Destructor
     */
    ~FileBackend();

    /*! \brief Set path to log file
     *
     * \param path Path to the log file.
     *             Note that a backup is made in same directory than file
     *             given by path, with a other exention.
     * \param maxFileSize Maximum size allowed for log file [Byte]
     *             Also note that the file can be some bytes greater
     *             than specified size.
     * \note This function should only be called
     *       before this LoggerFileBackend is passed to the Logger
     *       (changing file path while Logger
     *        is calling logError() is undefined behaviour).
     */
    bool setLogFilePath(const QString & path, qint64 maxFileSize = 1024*1024);

    /*! \brief Get log file path
     */
    QString logFilePath() const;

    /*! \brief Get backup log file path
     */
    QString backupLogFilePath() const;

    /*! \brief Get maximum log file size
     */
    qint64 maxFileSize() const;

    /*! \brief Log given error
     */
    void logError(const Error & error) override;

    /*! \brief Cleanup
     */
    void cleanup() override;

   private:

    /*! \brief Open log file
     */
    bool openFile(QFile & file);

    /*! \brief Backup log file
     */
    void backupLogFile();

    qint64 mMaxFileSize;
    QString mFilePath;
  };

}}  // namespace Mdt{ namespace ErrorLogger {

#endif // #ifndef MDT_ERROR_LOGGER_FILE_BACKEND_H
