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
#ifndef MDT_SQL_PLUGIN_INSTANCE_H
#define MDT_SQL_PLUGIN_INSTANCE_H

#include "PluginInfo.h"
#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief A instance of a plugin
   */
  class PluginInstance : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Create a instance of a implementation that matches a interface IID and a driver
     *
     * If this plugin instance has no plugin loaded,
     *  a plugin that can create the requested object will be serached and loaded first.
     *
     * If this plugin instance has a plugin loaded,
     *  it will be checked if it can create the requested object.
     *  If not, a plugin that can create the requested object will be serached and loaded first.
     *
     * \note If the plugin of this instance changes,
     *    the current plugin will not be unloaded.
     *    See QPluginLoader about load/unload plugins.
     * \pre \a interfaceIid must not be empty
     * \pre \a driverName must not be empty
     * \return A valid pointer to the created implementation on success,
     *     otherwise a nullptr
     * \sa PluginLoader
     */
    template<typename PluginInterface>
    QObject *create(const QString & interfaceIid, const QString & driverName)
    {
      Q_ASSERT(!interfaceIid.trimmed().isEmpty());
      Q_ASSERT(!driverName.trimmed().isEmpty());

      if(currentPluginMatches(interfaceIid, driverName)){
        return createImplementationInstance<PluginInterface>(interfaceIid, driverName);
      }
      if(!loadMatchingPlugin(interfaceIid, driverName)){
        return nullptr;
      }

      return createImplementationInstance<PluginInterface>(interfaceIid, driverName);
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    template<typename PluginInterface>
    QObject *createImplementationInstance(const QString & interfaceIid, const QString & driverName)
    {
      Q_ASSERT(mCurrentPlugin != nullptr);

      auto *plugin = qobject_cast<PluginInterface*>(mCurrentPlugin);
      if(plugin == nullptr){
        setCastToPluginInterfaceError(interfaceIid, driverName);
        return nullptr;
      }

      return plugin->create();
    }

    bool currentPluginMatches(const QString & interfaceIid, const QString & driverName);
    bool loadMatchingPlugin(const QString & interfaceIid, const QString & driverName);
    void setCastToPluginInterfaceError(const QString& interfaceIid, const QString & driverName);
    void setLastError(const Mdt::Error & error);

    QObject *mCurrentPlugin = nullptr;
    PluginInfo mCurrentPluginInfo;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_PLUGIN_INSTANCE_H
