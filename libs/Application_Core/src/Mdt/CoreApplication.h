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
#ifndef MDT_CORE_APPLICATION_H
#define MDT_CORE_APPLICATION_H

#include <QCoreApplication>
#include <QString>
#include <memory>

namespace Mdt{

  class CoreApplicationImpl;

  /*! \brief CoreApplication adds some helper to QCoreApplication for application initialization
   */
  class CoreApplication : public QCoreApplication
  {
   Q_OBJECT

   public:

    /*! \brief Construct a core application
     */
    CoreApplication(int & argc, char** argv);

    /*! \brief Destroy the core application object
     */
    ~CoreApplication();

    /*! \brief Enable file logging
     *
     * After file logging was enabled,
     *  errors that are committed using Mdt::Error::commit()
     *  are added to the log file \a logFilePath() .
     */
    void enableFileLogging();

    /*! \brief Get the path to the log file
     */
    QString logFilePath();

    /*! \brief Get path to the cache directory
     *
     * Returns StandardPaths::cacheDirectoryPath()
     */
    static QString cacheDirectoryPath();

    /*! \brief Get Qt library version
     */
    static QString qtVersion();

    /*! \brief Get Mdt library version
     */
    static QString mdtVersion();

    /*! \brief Debug environment
     *
     * Will print various informations,
     *  like libraries versions, paths to some directories,
     *  etc.. to the console.
     */
    void debugEnvironment();

   private:

    std::unique_ptr<CoreApplicationImpl> mImpl;
  };

} // namespace Mdt{

#endif // #ifndef MDT_CORE_APPLICATION_H
