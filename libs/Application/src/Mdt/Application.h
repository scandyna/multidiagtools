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
#ifndef MDT_APPLICATION_H
#define MDT_APPLICATION_H

#include "QtSolutions/QtSingleApplication"
#include <QString>

/*! \brief Gloab Application instance
 */
#if defined(MdtApp)
 #undef MdtApp
 #endif
#define MdtApp (reinterpret_cast<Mdt::Application*>(QCoreApplication::instance()))

namespace Mdt{

  /*! \brief Application adds some helper for application initialization
   *
   * \sa QtSingleApplication
   */
  class Application : public QtSingleApplication
  {
   Q_OBJECT

   public:

    /*! \brief Multiple instance permission
     *
     * \note Multiple instance permission
     *        refers to application process instances.
     *        Multiple instances of Application in one
     *        process is not allowed at all.
     */
    enum MultipleInstancePermission
    {
      SingleInstanceOnly,     /*!< Only one instance of the application is allowed */
      MultipleInstanceAllowed /*!< Multiple instances of the application is allowed */
    };

    /*! \brief Creates a Application object
     *
     * To complete initialization,
     *  call init()
     */
    Application(int & argc, char **argv);

    /*! \brief Destructor
     */
    ~Application();

    // Copy disabled
    Application(const Application &) = delete;
    Application & operator=(const Application &) = delete;
    // Move disabled
    Application(Application &&) = delete;
    Application & operator=(Application &&) = delete;

    /*! \brief Do initializations
     *
     * Several things are done here:
     *  - If multipleInstancePermission is set to SingleInstanceOnly,
     *     and a other program instance is running, this method returns false.
     *  - The system data directory is serached in several predefined paths.
     *     If it was not found, this method returns false.
     *  - A log directory is created if required.
     *     If this part fails, a warning is printed to the console, but this method returns true.
     *  - The error system is also initilized.
     *     If this fails, this method returns false.
     *  - A cache directory is created if required.
     *     If this fails, this method returns false.
     *
     * \todo Implement loading translations..
    */
    bool init(MultipleInstancePermission multipleInstancePermission = SingleInstanceOnly);

    /*! \brief Get multiple instance permission
     */
    static MultipleInstancePermission multipleInstancePermission()
    {
      return MdtApp->pvMultipleInstancePermission;
    }

    /*! \brief Get the multi diag tools library version
     */
    static QString libraryVersion();

    /*! \brief Get system data directory path
     */
    static QString systemDataDirectoryPath()
    {
      Q_ASSERT(MdtApp != nullptr);
      return MdtApp->pvSystemDataDirectory;
    }

    /*! \brief Get log directory path
     */
    static QString logDirectoryPath()
    {
      Q_ASSERT(MdtApp != nullptr);
      return MdtApp->pvLogDirectory;
    }

    /*! \brief Get log file path
     *
     * mdtError (more precisely Mdt::Error::LoggerFileBackend)
     *  will write to this file.
     */
    static QString logFilePath()
    {
      Q_ASSERT(MdtApp != nullptr);
      return MdtApp->pvLogFile;
    }

    /*! \brief Get cache directory path
     */
    static QString cacheDirectoryPath()
    {
      Q_ASSERT(MdtApp != nullptr);
      return MdtApp->pvCacheDirectory;
    }

    /*! \brief Debug environment
     *
     * Will print various informations,
     *  like library version, data directory,
     *  etc.. to the console.
     */
    static void debugEnvironnement();

   private:

    /*! \brief Search system data directory
     */
    bool findSystemDataDirectory();

    /*! \brief Check if given path is system data direrctory
     */
    bool isSystemDataDirectory(const QString & path);

    /*! \brief Create log directory if required and define pvLogDirectory
     */
    bool initLogDirectory();

    /*! \brief Init error system
     */
    bool initErrorSystem();

    /*! \brief Create cache directory if required and define pvCacheDirectory
     */
    bool initCacheDirectory();

    MultipleInstancePermission pvMultipleInstancePermission;
    QString pvSystemDataDirectory;
    QString pvLogDirectory;
    QString pvLogFile;
    QString pvCacheDirectory;
  };

} // namespace Mdt{

#endif // #ifndef
