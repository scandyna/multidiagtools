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
#ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_H
#define MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_H

#include "LibraryInfo.h"
#include "LibraryName.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QMetaType>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Data value class that stores informations about a Qt plugin
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QtPluginInfo
  {
   public:

    /*! \brief Set absolute file path
     */
    void setAbsoluteFilePath(const QString & path);

    /*! \brief Get absolute file path
     */
    QString absoluteFilePath() const
    {
      return mLibraryInfo.absoluteFilePath();
    }

    /*! \brief Set library name
     *
     * \a name should be the full library file name (f.ex. libQt5Core.so, or Qt5Core.dll).
     *  To specify only the library name (like Qt5Core), use setLibraryName().
     *
     * \pre \a name must at least have a library name and a extension.
     */
    void setLibraryPlatformName(const QString & name);

    /*! \brief Set library name
     *
     * \sa LibraryName
     */
    void setLibraryName(const LibraryName & libraryName);

    /*! \brief Get library name
     */
    LibraryName libraryName() const
    {
      return mLibraryInfo.libraryName();
    }

    /*! \brief Set directory name
     *
     * Qt plugins are generally stored in a subdirectory called plugins.
     *  They are organized in directories,
     *  like platforms, audio, etc...,
     *  which is the meaning of directory name here.
     *
     */
    void setDirectoryName(const QString & name);

    /*! \brief Get directory name
     *
     * \sa setDirectoryName()
     */
    QString directoryName() const
    {
      return mDirectoryName;
    }

    /*! \brief Get the library info hold in this plugin info
     */
    LibraryInfo libraryInfo() const
    {
      return mLibraryInfo;
    }

    /*! \brief Check if this plugin info is null
     */
    bool isNull() const
    {
      return ( mLibraryInfo.isNull() || mDirectoryName.isEmpty() );
    }

    /*! \brief Check if Qt plugin info a and b are equal
     */
    friend
    bool operator==(const QtPluginInfo & a, const QtPluginInfo & b)
    {
      if( QString::compare( a.mDirectoryName, b.mDirectoryName, Qt::CaseSensitive ) != 0 ){
        return false;
      }
      return (a.mLibraryInfo == b.mLibraryInfo);
    }

    /*! \brief Check if Qt plugin info a and b are different
     */
    friend
    bool operator!=(const QtPluginInfo & a, const QtPluginInfo & b)
    {
      return !(a == b);
    }

   private:

    LibraryInfo mLibraryInfo;
    QString mDirectoryName;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::QtPluginInfo)

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_INFO_H
