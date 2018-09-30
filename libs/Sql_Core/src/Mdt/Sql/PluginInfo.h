/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_PLUGIN_INFO_H
#define MDT_SQL_PLUGIN_INFO_H

#include "MdtSql_CoreExport.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QJsonObject>

namespace Mdt{ namespace Sql{

  /*! \brief Hold informations about a plugin
   */
  class MDT_SQL_CORE_EXPORT PluginInfo
  {
   public:

    /*! \brief Construct a null plugin info
     */
    PluginInfo() = default;

    /*! \brief Copy construct a plugin info from \a other
     */
    PluginInfo(const PluginInfo & other) = default;

    /*! \brief Copy assign \a other to this plugin info
     */
    PluginInfo & operator=(const PluginInfo & other) = default;

    /*! \brief Move construct a plugin info from \a other
     */
    PluginInfo(PluginInfo && other) = default;

    /*! \brief Move assign \a other to this plugin info
     */
    PluginInfo & operator=(PluginInfo && other) = default;

    /*! \brief Set file path
     *
     * \pre \a file must be a absolute path
     */
    void setFilePath(const QFileInfo & file);

    /*! \brief Get file path
     */
    QString filePath() const
    {
      return mFilePath;
    }

    /*! \brief Set interface
     *
     * \pre \a interface must nut be empty
     */
    void setInterface(const QString & interface);

    /*! \brief Get interface
     */
    QString interface() const
    {
      return mInterface;
    }

    /*! \brief Set compatible drivers
     *
     * \pre \a drivers must not be empty
     */
    void setCompatibleDrivers(const QStringList & drivers);

    /*! \brief Get compatible drivers
     */
    QStringList compatibleDrivers() const
    {
      return mCompatibleDrivers;
    }

    /*! \brief Check if this plugin supports a driver
     */
    bool supportsDriver(const QString & driverName) const noexcept
    {
      return mCompatibleDrivers.contains(driverName, Qt::CaseSensitive);
    }

    /*! \brief Check if this plugin info is null
     */
    bool isNull() const
    {
      return mFilePath.isEmpty() || mInterface.isEmpty() || mCompatibleDrivers.isEmpty();
    }

    /*! \brief Get a plugin info from a file path and meta data
     */
    static PluginInfo fromFileInfoAndMetaData(const QFileInfo & file, const QJsonObject & metaData);

   private:

    static QString interfaceFromJsonObject(const QJsonObject & json);
    static QStringList driversFromJsonObject(const QJsonObject & json);

    QString mFilePath;
    QString mInterface;
    QStringList mCompatibleDrivers;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_PLUGIN_INFO_H
