/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_LIBRARY_VERSION_H
#define MDT_DEPLOY_UTILS_LIBRARY_VERSION_H

#include <QString>
#include <QtGlobal>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a version of a library
   */
  class LibraryVersion
  {
   public:

    /*! \brief Construct a null version
     */
    LibraryVersion() = default;

    /*! \brief Construct a library version
     *
     * \pre versionMajor must be >= 0
     * \pre if versionPatch is >=0 , versionMinor must also be >= 0
     */
    LibraryVersion(qint8 versionMajor, qint8 versionMinor = -1, qint8 versionPatch = -1);

    /*! \brief Construct a library version from a version string
     *
     * If \a version does not represent a valid version string,
     *  this will be a null library version.
     *
     * \sa isNull()
     */
    LibraryVersion(const QString & version);

    /*! \brief Copy construct a version from a other
     */
    LibraryVersion(const LibraryVersion &) = default;

    /*! \brief Copy assign to this version from a other
     */
    LibraryVersion & operator=(const LibraryVersion &) = default;

    /*! \brief Move construct a version from a other
     */
    LibraryVersion(LibraryVersion &&) = default;

    /*! \brief Move assign to this version from a other
     */
    LibraryVersion & operator=(LibraryVersion &&) = default;

    /*! \brief Check is this library version is null
     */
    bool isNull() const
    {
      return (mVersionMajor < 0);
    }

    /*! \brief Get version major
     */
    qint8 versionMajor() const
    {
      return mVersionMajor;
    }

    /*! \brief Get version minor
     */
    qint8 versionMinor() const
    {
      return mVersionMinor;
    }

    /*! \brief Get version patch
     */
    qint8 versionPatch() const
    {
      return mVersionPatch;
    }

    /*! \brief Get a string representation of this library version
     */
    QString toString() const;

//     /*! \brief Set library version
//      *
//      * \pre versionMajor must be >= 0
//      * \pre if versionPatch is >=0 , versionMinor must also be >= 0
//      */
//     void setVersion(qint8 versionMajor, qint8 versionMinor = -1, qint8 versionPatch = -1);

   private:

    qint8 mVersionMajor = -1;
    qint8 mVersionMinor = -1;
    qint8 mVersionPatch = -1;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_VERSION_H
