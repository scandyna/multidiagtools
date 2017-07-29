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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
#define MDT_DEPLOY_UTILS_LIBRARY_INFO_H

#include "LibraryName.h"
#include <QString>
#include <QMetaType>
// #include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Data value class that stores informations about a library
   */
  class LibraryInfo
  {
   public:

//     /*! \brief Construct a null library info
//      */
//     LibraryInfo() = default;

    /*! \brief Set absolute file path
     */
    void setAbsoluteFilePath(const QString & path);

    /*! \brief Get absolute file path
     */
    QString absoluteFilePath() const
    {
      return mAbsoluteFilePath;
    }

    /*! \brief Set library name
     *
     * \a name can be passed without any prefix, extension, version.
     * Example: Qt5Core
     * A more platform specific name can also be passed,
     *  for example libQt5Core.so
     * A versionned version can also be passed,
     *  for example libQt5Core.so.5 , libQt5Core.so.5.5 , libQt5Core.so.5.5.1
     */
    void setLibraryPlatformName(const QString & name);

    /*! \brief Set library name
     */
    void setLibraryName(const LibraryName & libraryName);

    /*! \brief Get library name
     */
    LibraryName libraryName() const
    {
      return mLibraryName;
    }

    /*! \brief Check if this library info is null
     */
    bool isNull() const
    {
      return ( mLibraryName.isNull() || mAbsoluteFilePath.isEmpty() );
    }

    /*! \brief Check if library info a and b are equal
     */
    friend
    bool operator==(const LibraryInfo & a, const LibraryInfo & b)
    {
      if( QString::compare( a.mAbsoluteFilePath, b.mAbsoluteFilePath, Qt::CaseSensitive ) != 0 ){
        return false;
      }
      if( QString::compare( a.mLibraryName.name(), b.mLibraryName.name(), Qt::CaseSensitive ) != 0 ){
        return false;
      }
      return true;
    }

    /*! \brief Check if library info a and b are different
     */
    friend
    bool operator!=(const LibraryInfo & a, const LibraryInfo & b)
    {
      return !(a == b);
    }

   private:

    LibraryName mLibraryName;
    QString mAbsoluteFilePath;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::LibraryInfo)

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
