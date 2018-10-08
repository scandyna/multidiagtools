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
#include <QMutex>
#include <QMutexLocker>
#include <QGlobalStatic>
#include <vector>
#include <algorithm>

namespace Mdt{ namespace Sql{

Q_GLOBAL_STATIC(std::vector<PluginInfo>, avaliablePlugins)
Q_GLOBAL_STATIC(QMutex, avaliablePluginsMutex)

PluginInfo PluginLoader::findPlugin(const QString& interface, const QString& driverName)
{
  Q_ASSERT(!interface.trimmed().isEmpty());
  Q_ASSERT(!driverName.trimmed().isEmpty());

  PluginInfo plugin;

  if(!fetchAvailablePluginsIfNot()){
    return plugin;
  }
  QMutexLocker lock(avaliablePluginsMutex);
  const auto pred = [interface, driverName](const PluginInfo & plugin){
    return (plugin.interface() == interface) && plugin.supportsDriver(driverName);
  };
  const auto & plugins = *avaliablePlugins;
  const auto it = std::find_if(plugins.cbegin(), plugins.cend(), pred);
  if(it == plugins.cend()){
    const QString msg = tr("Could not find a plugin that matches IID '%1' and supporting driver '%2'")
                        .arg(interface, driverName);
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return plugin;
  }
  plugin = *it;

  return plugin;
}

QObject* PluginLoader::loadPlugin(const PluginInfo& pluginInfo)
{
  Q_ASSERT(!pluginInfo.isNull());

  QPluginLoader loader(pluginInfo.filePath());
  QObject *plugin = loader.instance();
  if(plugin == nullptr){
    const QString msg = tr("Loading the plugin '%1' failed.")
                        .arg(pluginInfo.filePath());
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return nullptr;
  }

  return plugin;
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
      avaliablePlugins->push_back(plugin);
    }
  }
  if(avaliablePlugins->empty()){
    const auto pluginsPath = LibraryInfo::getPluginsPath();
    Q_ASSERT(pluginsPath);
    const QString msg = tr("Could not find any valid plugin in '%1'.")
                        .arg(*pluginsPath);
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return true;
}

bool PluginLoader::fetchAvailablePluginsIfNot()
{
  QMutexLocker lock(avaliablePluginsMutex);
  if(!avaliablePlugins->empty()){
    return true;
  }
  return fetchAvailablePlugins();
}

void PluginLoader::setLastError(const Error & error)
{
  Q_ASSERT(!error.isNull());

  mLastError = error;
}

}} // namespace Mdt{ namespace Sql{
