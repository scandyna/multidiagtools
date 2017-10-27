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
#ifndef MDT_DEPLOY_UTILS_SEARCH_PATH_LIST_H
#define MDT_DEPLOY_UTILS_SEARCH_PATH_LIST_H

#include "PathList.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief SearchPathList provides additional tools to PathList
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT SearchPathList
  {
   public:

    /*! \brief Specify if path prefixes must be part of the search paths
     *
     * By default, path prefixes are not part of the search path.
     */
    void setIncludePathPrefixes(bool include);

    /*! \brief Set a list of path prefix
     *
     * A path prefix is typically the root of a library distribution,
     *  which then contains subdirectories like bin, lib, ...
     */
    void setPathPrefixList(const PathList & pathList);

    /*! \brief Specify subdirectories to check
     *
     * Specify subdirectories to check below each directory
     *  in those specified with setPathPrefixList().
     *  For example, if /opt/liba and /opt/libb have been set as
     *  path prefixes, and \a suffixList contains bin qt5/bin,
     *  path list will contain
     *  /opt/liba/bin, /opt/liba/qt5/bin, /opt/libb/bin, /opt/libb/qt5/bin .
     *  If includePathPrefixes is set, path list will contain
     *  /opt/liba, /opt/liba/bin, /opt/liba/qt5/bin, /opt/libb, /opt/libb/bin, /opt/libb/qt5/bin .
     *
     * \sa setPathPrefixList()
     * \sa setIncludePathPrefixes()
     */
    void setPathSuffixList(const QStringList & suffixList);

    /*! \brief Append a path
     *
     * Add \a path to the end of this list.
     *  Later, \a path will be returned as is,
     *  without any suffix.
     */
    void appendPath(const QString & path);

    /*! \brief Prepend a path
     *
     * Add \a path to the beginnig of this list.
     *  Later, \a path will be returned as is,
     *  without any suffix.
     */
    void prependPath(const QString & path);

    /*! \brief Get a list with all paths
     */
    PathList pathList() const
    {
      return mPathList;
    }

    /*! \brief Get a string list with all paths
     */
    QStringList toStringList() const
    {
      return mPathList.toStringList();
    }

    /*! \brief Clear this list
     */
    void clear();

   private:

    void updatePathList();

    bool mIncludePathPrefixes = false;
    PathList mPathList;
    PathList mPathListToPrepend;
    PathList mPathListToAppend;
    PathList mPathPrefixList;
    QStringList mPathSuffixList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SEARCH_PATH_LIST_H
