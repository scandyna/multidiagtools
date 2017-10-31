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
#ifndef MDT_DEPLOY_UTILS_RPATH_H
#define MDT_DEPLOY_UTILS_RPATH_H

#include "RPathInfoList.h"
#include "OperatingSystem.h"
#include "Mdt/Error.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QObject>
#include <QString>

class QFileInfo;

namespace Mdt{ namespace DeployUtils{

  /*! \brief Utility class to get and set RPATH of executables and shared libraries that support it
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT RPath : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit RPath(QObject *parent = nullptr);

    /*! \brief Read RPATH from binary file
     */
    bool readRPath(const QString & binaryFilePath);

    /*! \brief Get RPATH
     *
     * \note returns a valid RPATH only after a successful
     *        call of readRPath() .
     */
    RPathInfoList rpath() const
    {
      return mRPath;
    }

    /*! \brief Set RPATH to origin
     *
     * Origin means that RPATH is set to the current directory
     *  where the executable or the shared library lives.
     *  On Linux, it is called $ORIGIN .
     */
    bool setRPathToOrigin(const QString & binaryFilePath);

    /*! \brief Set RPATH to binary file
     */
    bool setRPath(const RPathInfoList & rpath, const QString & binaryFilePath);

    /*! \brief Prepend \a path to the RPATH for a executable file or a shared library
     */
    bool prependPath(const QString & path, const QString & binaryFilePath);

    /*! \brief Prepend \a path to the RPATH for binaries in a directory
     */
    bool prependPathForBinaries(const QString & path, const QString & directoryPath);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    static bool isExecutable(const QFileInfo & fileInfo);
    void setLastError(const Mdt::Error & error);

    RPathInfoList mRPath;
    OperatingSystem mOs = OperatingSystem::Unknown;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_H
