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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_H
#define MDT_DEPLOY_UTILS_LIBRARY_NAME_H

#include "LibraryVersion.h"
#include "OperatingSystem.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringRef>

class QStringRef;

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a shared library name
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT LibraryName
  {
  public:

    /*! \brief Construct a null library name
     */
    LibraryName() = default;

    /*! \brief Construct a library name from a name
     *
     * \a fullName can be passed without any prefix, extension, version.
     * Example: Qt5Core
     * A more platform specific name can also be passed,
     *  for example libQt5Core.so
     * A versionned version can also be passed,
     *  for example libQt5Core.so.5 , libQt5Core.so.5.5 , libQt5Core.so.5.5.1
     */
    LibraryName(const QString & fullName);

    /*! \brief Copy construct a library name from a other
     */
    LibraryName(const LibraryName &) = default;

    /*! \brief Copy assign to this library name from a other
     */
    LibraryName & operator=(const LibraryName &) = default;

    /*! \brief Move construct a library name from a other
     */
    LibraryName(LibraryName &&) = default;

    /*! \brief Move assign to this library name from a other
     */
    LibraryName & operator=(LibraryName &&) = default;

    /*! \brief Check is this library name is null
     */
    bool isNull() const
    {
      return mFullName.isEmpty();
    }

    /*! \brief Get library name prefix
     *
     * Returns the library name prefix if it was set.
     */
    QString prefix() const
    {
      return mPrefix;
    }

    /*! \brief Get library name
     *
     * Returns the library name without any prefix or extension or version.
     *  For example, if this library name was constructed with libQt5Core.so.5.5.1
     *  this method returns Qt5Core
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Get library name extension
     *
     * Returns the extension if it was set.
     *  For versionned so names, only 'so' is returned.
     */
    QString extension() const
    {
      return mExtension;
    }

    /*! \brief Get library version
     */
    LibraryVersion version() const
    {
      return mVersion;
    }

    /*! \brief Get library full name
     *
     * Returns the library name as it was set.
     */
    QString fullName() const
    {
      return mFullName;
    }

    /*! \brief Get the full library name for Linux platform
     *
     * Returns the lib prefix, name, .so extension,
     *  and the version if available
     */
    QString toFullNameLinux() const;

    /*! \brief Deduce the operating system of a library from its file name
     *
     * Will use the file extension of \a fullName to deduce
     *  the library operating system.
     */
    static OperatingSystem operatingSystem(const QString & fullName);

  private:

    static LibraryVersion parseExtension(const QString & fullName, QStringRef & extension);
    static bool compareExtension(const QStringRef & extension, const char * const s);

    QString mFullName;
    QString mPrefix;
    QString mName;
    QString mExtension;
    LibraryVersion mVersion;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_H
