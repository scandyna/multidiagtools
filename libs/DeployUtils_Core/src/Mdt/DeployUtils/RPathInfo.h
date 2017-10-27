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
#ifndef MDT_DEPLOY_UTILS_RPATH_INFO_H
#define MDT_DEPLOY_UTILS_RPATH_INFO_H

#include "MdtDeployUtils_CoreExport.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Holds informations about a RPATH
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT RPathInfo
  {
   public:

    RPathInfo() = delete;

    /*! \brief Construct a RPathInfo
     */
    RPathInfo(const QString & path, bool relative)
     : mIsRelative(relative),
       mPath(path)
    {
    }

    /*! \brief Copy contruct a RPathInfo from \a other
     */
    RPathInfo(const RPathInfo & other) = default;

    /*! \brief Copy assign \a other to this RPathInfo
     */
    RPathInfo & operator=(const RPathInfo & other) = default;

    /*! \brief Move contruct a RPathInfo from \a other
     */
    RPathInfo(RPathInfo && other) = default;

    /*! \brief Move assign \a other to this RPathInfo
     */
    RPathInfo & operator=(RPathInfo && other) = default;

    /*! \brief Get path
     *
     * \note A empty path can mean that
     *        this RPATH is not set at all,
     *        or this RPATH refers to origin.
     *
     * \sa isEmpty()
     * \sa isOrigin()
     */
    QString path() const
    {
      return mPath;
    }

    /*! \brief Check if this RPath is relative
     */
    bool isRelative() const
    {
      return mIsRelative;
    }

    /*! \brief Check if this RPATH is empty
     */
    bool isEmpty() const
    {
      return ( mPath.isEmpty() && !mIsRelative );
    }

    /*! \brief Check if this RPATH is origin
     *
     * Returns true if this RPATH refers to the directory
     *  where the executable or the shared library lives.
     *
     * On Linux, such RPATH is $ORIGIN .
     *
     * \note If this RPATH refers to a relative non empty path,
     *        this method reurns false.
     *        On linux: $ORIGIN is origin,
     *        $ORIGIN/lib is not origin.
     *
     * \sa isRelative()
     */
    bool isOrigin() const
    {
      return ( mPath.isEmpty() && mIsRelative );
    }

    /*! \brief Get a string representing this path
     *
     * If this path is relative,
     *  the string will contains $ORIGIN followed by the path.
     */
    QString toStringLinux() const;

   private:

    bool mIsRelative = false;
    QString mPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_INFO_H
