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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_H
#define MDT_DEPLOY_UTILS_LIBRARY_H

#include "LibraryInfo.h"
#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/Error.h"
#include <QString>
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Provides some helper methods for libraries
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT Library : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Tell if search must include system paths or not
     */
    enum SearchInSystemPaths
    {
      IncludeSystemPaths, /*!< Also search in system path */
      ExcludeSystemPaths  /*!< Do not search in system path */
    };

    /*! \brief Constructor
     */
    explicit Library(QObject* parent = nullptr);

    // Copy disabled
    Library(const Library &) = delete;
    Library & operator=(const Library &) = delete;
    // Move disabled
    Library(Library &&) = delete;
    Library & operator=(Library &&) = delete;

    /*! \brief Find a library
     *
     * \param name Name of the library.
     *     Can be a name without any prefix or suffix (ex: Qt5Core),
     *     or a more platform specific name (ex: libQt5Core.so).
     *     For platform that supports versions in the library file name,
     *     a versionned name can be done (ex: libQt5Core.so.5 , libQt5Core.so.5.6.2)
     * \param pathList List of paths where to search the library
     * \param searchInSystemPaths Tell if the search must include system paths
     * \return True if the library was found, false if it was not found, or a error occured.
     * \pre name must be a non empty string.
     * \pre If \a searchInSystemPaths is ExcludeSystemPaths, pathList must contain at least 1 path
     */
    bool findLibrary(const QString & name, const Mdt::FileSystem::PathList pathList = Mdt::FileSystem::PathList(), SearchInSystemPaths searchInSystemPaths = IncludeSystemPaths);

    /*! \brief Get library info
     *
     * Returns the information of the library
     *  after a success call of findLibrary()
     */
    LibraryInfo libraryInfo() const
    {
      return mLibraryInfo;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }


   private:

    void addSystemPaths(Mdt::FileSystem::PathList & pathList);

    LibraryInfo mLibraryInfo;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_H
