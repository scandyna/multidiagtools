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
#ifndef MDT_DEPLOY_UTILS_MDT_LIBRARY_H
#define MDT_DEPLOY_UTILS_MDT_LIBRARY_H

#include "LibraryInfo.h"
#include "LibraryInfoList.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QObject>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief MdtLibrary offers some utilities specific to Mdt libraries
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT MdtLibrary : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Get Mdt libraries out of \a libraries
     */
    static LibraryInfoList getMdtLibraries(const LibraryInfoList & libraries);

    /*! \brief Get the Mdt library base name from \a library
     *
     * Example: for Mdt0Application_Core, Application_Core is returned.
     */
    static QString baseName(const LibraryInfo & library);

    /*! \brief Get the Mdt library base name for \a libraries
     *
     * \sa baseName()
     */
    static QStringList getBaseNames(const LibraryInfoList & libraries);

   private:

    static bool isMdtLibrary(const LibraryInfo & libraryInfo);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_MDT_LIBRARY_H
