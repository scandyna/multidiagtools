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
#ifndef MDT_DEPLOY_UTILS_FILE_COPIER_H
#define MDT_DEPLOY_UTILS_FILE_COPIER_H

#include "LibraryInfo.h"
#include "LibraryInfoList.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Provides utilities for files and directories manipulation
   */
  class FileCopier : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit FileCopier(QObject* parent = nullptr);

    /*! \brief Create a directory
     *
     * Will create a directory designed by \a directoryPath
     *  if it not allready exists.
     *  If parent directories are missing,
     *  they will also be created.
     */
    bool createDirectory(const QString & directoryPath);

    /*! \brief Copy a list of libraries to a directory
     *
     * If directory designed by \a destinationDirectoryPath does not exist,
     *  it will first be created using createDirectory().
     */
    bool copyLibraries(const LibraryInfoList & libraries, const QString & destinationDirectoryPath);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    bool copyLibrary(const LibraryInfo & sourceLibrary, const QString & destinationDirectoryPath);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_COPIER_H
