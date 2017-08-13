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

#include "Platform.h"

#include "OperatingSystem.h"
#include "LibraryInfoList.h"
#include "PathList.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  class BinaryDependenciesImplementationInterface;

  /*! \brief Find dependencies for a executable or a library
   */
  class BinaryDependencies : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * Will instanciate a implementation depending on \a targetOperatingSystem .
     *  The target operating system means the OS of the binary file to parse.
     */
    BinaryDependencies(OperatingSystem targetOperatingSystem, QObject* parent = nullptr);

    /*! \brief Destructor
     */
    ~BinaryDependencies();

    /*! \brief Check if valid
     *
     * Returns true if a implementation for native platform could be loaded,
     *  otherwise false.
     */
    bool isValid() const;

    /*! \brief Set a list of paths where to search dependencies first
     *
     * For target platforms which supports RPATH (like Linux),
     *  this has no effects, because the implementation will use ldd,
     *  which returns directly the full path for each dependency.
     *
     * For other target platforms, like Windows,
     *  use this method to tell where to find libraries
     *  that are not installed in a standard library location
     *  (like Qt5, for example),
     *  or a alternative library version must be used.
     */
    void setLibrarySearchFirstPathList(const PathList & pathList);

    /*! \brief Get the list of paths where to search dependencies first
     *
     * \sa setLibrarySearchPrefixPathList()
     */
    PathList librarySearchFirstPathList() const;

    /*! \brief Find dependencies for a executable or a library
     */
    bool findDependencies(const QString & binaryFilePath);

    /*! \brief Get dependencies
     */
    LibraryInfoList dependencies() const;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    std::unique_ptr<BinaryDependenciesImplementationInterface> mImpl;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
