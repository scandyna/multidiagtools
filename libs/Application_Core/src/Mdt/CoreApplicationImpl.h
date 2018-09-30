/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_CORE_APPLICATION_IMPL_H
#define MDT_CORE_APPLICATION_IMPL_H

#include "StandardPaths.h"
#include "MdtApplication_CoreExport.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>

namespace Mdt{

  /*! \brief Implementation for CoreApplication and derived classes
   */
  class MDT_APPLICATION_CORE_EXPORT CoreApplicationImpl
  {
    Q_DECLARE_TR_FUNCTIONS(CoreApplicationImpl)

   public:

    /*! \brief Log file name format
     */
    enum LogFileNameFormat
    {
      ApplicationName,        /*!< Log file name will be applicationName.log */
      ApplicationNameAndPid   /*!< Log file name will be applicationName_pid.log */
    };

    /*! \brief Constructor
     */
    CoreApplicationImpl();

    /*! \brief Destructor
     */
    ~CoreApplicationImpl();

    // Copy disabled
    CoreApplicationImpl(const CoreApplicationImpl &) = delete;
    CoreApplicationImpl & operator=(const CoreApplicationImpl &) = delete;
    // Move disabled
    CoreApplicationImpl(CoreApplicationImpl &&) = delete;
    CoreApplicationImpl & operator=(CoreApplicationImpl &&) = delete;

    /*! \brief Enable file logging
     */
    void enableFileLogging(LogFileNameFormat format);

    /*! \brief Check if file logging is enabled
     */
    bool isFileLoggingEnabled() const
    {
      return !mLogFilePath.isEmpty();
    }

    /*! \brief Get the path to log file directory
     *
     * Returns StandardPaths::logDirectoryPath()
     */
    static QString logDirectoryPath()
    {
      return StandardPaths::logDirectoryPath();
    }

    /*! \brief Get the path to the log file
     */
    QString logFilePath()
    {
      return mLogFilePath;
    }

    /*! \brief Get path to the cache directory
     *
     * Returns StandardPaths::cacheDirectoryPath()
     */
    static QString cacheDirectoryPath()
    {
      return StandardPaths::cacheDirectoryPath();
    }

    /*! \brief Get Qt library version
     */
    static QString qtVersion();

    /*! \brief Get Mdt library version
     */
    static QString mdtVersion();

    /*! \brief Debug environment
     */
    void debugEnvironment(const QStringList & entries);

    /*! \brief Get common environment entries
     */
    QStringList commonEnvironmentEntries();

   private:

    void initErrorSystem();
    void initCacheDirectory();
    void addPluginPathToLibrariesPath();

    QString mLogFilePath;
  };

} // namespace Mdt{

#endif // #ifndef MDT_CORE_APPLICATION_IMPL_H
