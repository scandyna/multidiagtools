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
#include <memory>

class QFileInfo;

namespace Mdt{ namespace DeployUtils{

  class BinaryDependenciesImplementationInterface;

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

    /*! \brief Find dependencies for a list of executables and/or libraries
     *
     * \sa findDependencies(const QString &, const PathList &)
     * \note It is assumed that each binary file has the same binary format (PA, or elf, or ...)
     */
    bool findDependencies(const QStringList & binariesFilePaths, const PathList & searchFirstPathPrefixList);

    /*! \brief Find dependencies for a lits of libraries
     *
     * \sa findDependencies(const QString &, const PathList &)
     * \note It is assumed that each library has the same binary format (PA, or elf, or ...)
     * \pre Each library list libraries must have its full path set
     */
    bool findDependencies(const LibraryInfoList & libraries, const PathList & searchFirstPathPrefixList);

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

    std::unique_ptr<BinaryDependenciesImplementationInterface> initImpl(const QFileInfo & binaryFileInfo, const PathList & searchFirstPathPrefixList);
    void setLastError(const Mdt::Error & error);

    LibraryInfoList mDependencies;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
