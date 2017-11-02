/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_QT_LIBRARY_H
#define MDT_DEPLOY_UTILS_QT_LIBRARY_H

#include "LibraryInfo.h"
#include "LibraryInfoList.h"
#include "QtPluginInfo.h"
#include "QtPluginInfoList.h"
#include "QtModule.h"
#include "QtModuleList.h"
#include "PathList.h"
#include "OperatingSystem.h"
#include "Mdt/Error.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QObject>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief QtLibrary offers some utilities specific to Qt libraries
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QtLibrary : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Find plugins for a Qt library
     *
     * Will first deduce Qt plugins directories attached to \a qtLibrary
     *  using getPluginsDirectories(QtModule),
     *  then find the existing ones.
     *
     * \a searchFirstPathPrefixList is a list of paths to the root of a Qt installation.
     * Plugins are serached first in each item of \a searchFirstPathPrefixList,
     *  then in system library prefix paths.
     *  For each path in all mentionned path prefixes, plugins are searched in
     *  qt5/plugins, then plugins subdirectories.
     */
    QtPluginInfoList findLibraryPlugins(const LibraryInfo & qtLibrary, const PathList & searchFirstPathPrefixList);

    /*! \brief Find plugins for a list of Qt libraries
     *
     * \sa findLibraryPlugins()
     */
    QtPluginInfoList findLibrariesPlugins(const LibraryInfoList & qtLibraries, const PathList & searchFirstPathPrefixList);

    /*! \brief Get Qt libraries out of \a libraries
     */
    static LibraryInfoList getQtLibraries(const LibraryInfoList & libraries);

    /*! \brief Get the module to which \a qtLibrary is part of
     */
    static QtModule module(const LibraryInfo & qtLibrary);

    /*! \brief Get a list that contains modules for \a qtLibraries
     */
    static QtModuleList getModules(const LibraryInfoList & qtLibraries);

    /*! \brief Get plugins directories for \a module
     *
     * Will return a list of directory names for \a module ,
     *  without any path.
     */
    static QStringList getPluginsDirectories(QtModule module);

    /*! \brief Get plugins directories for \a modules
     *
     * Will return a list of directory names for \a modules ,
     *  without any path.
     */
    static QStringList getPluginsDirectories(const QtModuleList & modules);

    /*! \brief Find the root directory of plugins
     *
     * For a given path prefix, a subdirectory, named plugins,
     *  will be located in known subdirectories,
     *  like plugins or qt5/plugins .
     *
     * If \a pathPrefixList contains at least 1 non empty path,
     *  plugins directory will only be located starting
     *  from prefixes in it.
     *  Else, system path prefixes will be used.
     */
    static QString findPluginsRoot(const PathList & pathPrefixList);

   private:

    static bool isQtLibrary(const LibraryInfo & libraryInfo);
    static bool compareLibraries(const QString & a, const char * const b);
    static bool compareStringsCi(const QString & a, const char * const b);
    static bool compareStringsCi(const QString & a, const QString & b);
    static QtPluginInfoList findPluginsInDirectories(const QString & pluginsRoot, const QStringList & directories, const LibraryInfo & qtLibrary);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_LIBRARY_H
