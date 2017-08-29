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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H

#include "LibraryInfoList.h"
#include "PathList.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Find dependencies for a executable or a library
   */
  class BinaryDependencies : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependencies(QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~BinaryDependencies();

    /*! \brief Find dependencies for a executable or a library
     *
     * For target platforms that do not support RPATH (like DLL based systems),
     *  dependencies must be searched in several directories.
     *  Dependning on implementation, dependencies will be searched in the directory of \a binaryFilePath first.
     *  Then, dependencies will be searched, for each path in \a searchFirstPathPrefixList ,
     *  in known subdirectories (like lib, bin, qt5/lib, qt5/bin).
     *  Finally, depending of the implementation, dependencies can also be serached in
     *  in system paths.
     */
    bool findDependencies(const QString & binaryFilePath, const PathList & searchFirstPathPrefixList);

    /*! \brief Get dependencies
     */
    LibraryInfoList dependencies() const
    {
      return mDependencies;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    void setLastError(const Mdt::Error & error);

    LibraryInfoList mDependencies;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
