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
#include "QtModule.h"
#include "QtModuleList.h"
#include "PathList.h"
#include "Mdt/Error.h"

// #include "Mdt/Expected.h"

#include <QObject>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief QtLibrary offers some utilities specific to Qt libraries
   */
  class QtLibrary : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Find plugins for a Qt library
     *
     * Will first deduce Qt plugins directories attached to \a qtLibrary
     *  using getPluginsDirectories(QtModule),
     *  the find the existing ones.
     *
     * \a searchFirstPathPrefixList is a list of paths to the root of a Qt library.
     * Plugins are serached first in each item of \a searchFirstPathPrefixList,
     *  then in system library prefix paths (/usr/lib, /usr/lib/x86_64-linux-gnu, ...)
     *
     * Searching is done by trying known plugins subdirectories for each prefix path
     *  (qt5/plugins, plugins).
     */
    LibraryInfoList findLibraryPlugins(const LibraryInfo & qtLibrary, const PathList & searchFirstPathPrefixList);

    /*! \brief Find plugins for a list of Qt libraries
     *
     * \sa findLibraryPlugins()
     */
    LibraryInfoList findLibrariesPlugins(const LibraryInfoList & qtLibraries, const PathList & searchFirstPathPrefixList);

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

   private:

    static bool isQtLibrary(const LibraryInfo & libraryInfo);
    static bool compareLibraries(const QString & a, const char * const b);
    LibraryInfoList findPluginsInDirectories(const QString & pathPrefix, const QStringList & directories);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_LIBRARY_H
