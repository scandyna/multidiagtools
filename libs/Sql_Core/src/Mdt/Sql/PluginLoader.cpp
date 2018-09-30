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
#include "PluginLoader.h"
#include "LibraryInfo.h"
#include <QFileInfo>
#include <QFileInfoList>
#include <QPluginLoader>
#include <QLatin1String>
#include <algorithm>

namespace Mdt{ namespace Sql{

AbstractErrorDriver* PluginLoader::loadErrorDriver(const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  return loadErrorDriver(db.driverName());
}

AbstractErrorDriver* PluginLoader::loadErrorDriver(const QString& sqlDriverName)
{
  Q_ASSERT(!sqlDriverName.trimmed().isEmpty());

  if(!fetchAvailablePluginsIfNot()){
    return nullptr;
  }
  const PluginInfo plugin = findPlugin(QLatin1String("Mdt.Sql.AbstractErrorDriver"), sqlDriverName);
  if(plugin.isNull()){
    const QString msg = tr("Could not find a plugin that implements Mdt.Sql.AbstractErrorDriver and supports '%1' driver.")
                        .arg(sqlDriverName);
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return nullptr;
  }
  QPluginLoader loader(plugin.filePath());
  auto *driver = qobject_cast<AbstractErrorDriver*>( loader.instance() );
  if(driver == nullptr){
    const QString msg = tr("Loading the plugin '%1' failed.")
                        .arg(plugin.filePath());
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return nullptr;
  }

  return driver;
}

bool PluginLoader::fetchAvailablePlugins()
{
  const auto pluginFileListEx = LibraryInfo::getAvailablePluginFiles();
  if(!pluginFileListEx){
    setLastError(pluginFileListEx.error());
    return false;
  }
  const QFileInfoList & pluginFileList = *pluginFileListEx;
  for(const QFileInfo & file : pluginFileList){
    QPluginLoader loader(file.absoluteFilePath());
    const auto plugin = PluginInfo::fromFileInfoAndMetaData(file, loader.metaData());
    if(!plugin.isNull()){
      mAvaliablePlugins.push_back(plugin);
    }
  }

  return true;
}

bool PluginLoader::fetchAvailablePluginsIfNot()
{
  if(mAvaliablePlugins.empty()){
    return fetchAvailablePlugins();
  }
  return true;
}

PluginInfo PluginLoader::findPlugin(const QString& interface, const QString& driverName)
{
  PluginInfo plugin;

  const auto pred = [interface, driverName](const PluginInfo & plugin){
    return (plugin.interface() == interface) && plugin.supportsDriver(driverName);
  };
  const auto it = std::find_if(mAvaliablePlugins.cbegin(), mAvaliablePlugins.cend(), pred);
  if(it != mAvaliablePlugins.cend()){
    plugin = *it;
  }

  return plugin;
}

void PluginLoader::setLastError(const Error & error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace Sql{
