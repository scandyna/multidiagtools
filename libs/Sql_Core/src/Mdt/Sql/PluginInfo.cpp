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
#include "PluginInfo.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QLatin1String>

namespace Mdt{ namespace Sql{

void PluginInfo::setFilePath(const QFileInfo& file)
{
  Q_ASSERT(file.isAbsolute());

  mFilePath = file.absoluteFilePath();
}

void PluginInfo::setInterface(const QString& interface)
{
  Q_ASSERT(!interface.isEmpty());

  mInterface = interface;
}

void PluginInfo::setCompatibleDrivers(const QStringList& drivers)
{
  Q_ASSERT(!drivers.isEmpty());

  mCompatibleDrivers = drivers;
}

PluginInfo PluginInfo::fromFileInfoAndMetaData(const QFileInfo& file, const QJsonObject& metaData)
{
  Q_ASSERT(file.isAbsolute());

  PluginInfo plugin;

  const auto interface = interfaceFromJsonObject(metaData).trimmed();
  if(interface.isEmpty()){
    return plugin;
  }
  const auto drivers = driversFromJsonObject(metaData);
  if(drivers.isEmpty()){
    return plugin;
  }

  plugin.setFilePath(file);
  plugin.setInterface(interface);
  plugin.setCompatibleDrivers(drivers);

  return plugin;
}

QString PluginInfo::interfaceFromJsonObject(const QJsonObject & json)
{
  return json.value(QLatin1String("IID")).toString();
}

QStringList PluginInfo::driversFromJsonObject(const QJsonObject & json)
{
  QStringList drivers;

  const QJsonObject metaData = json.value(QLatin1String("MetaData")).toObject();
  const QJsonArray jsonKeys = metaData.value(QLatin1String("Keys")).toArray();
  for(const QJsonValue & jsonKey : jsonKeys){
    const QString key = jsonKey.toString().trimmed();
    if(!key.isEmpty()){
      drivers.append(key);
    }
  }

  return drivers;
}

}} // namespace Mdt{ namespace Sql{
