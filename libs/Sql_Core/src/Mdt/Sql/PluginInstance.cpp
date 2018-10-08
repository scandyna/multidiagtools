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
#include "PluginInstance.h"
#include "PluginLoader.h"

namespace Mdt{ namespace Sql{

bool PluginInstance::currentPluginMatches(const QString& interfaceIid, const QString& driverName)
{
  if(mCurrentPlugin == nullptr){
    return false;
  }
  if(mCurrentPluginInfo.interface() != interfaceIid){
    return false;
  }
  if(!mCurrentPluginInfo.supportsDriver(driverName)){
    return false;
  }
  return true;
}

bool PluginInstance::loadMatchingPlugin(const QString& interfaceIid, const QString& driverName)
{
  PluginLoader loader;

  const PluginInfo pluginInfo = loader.findPlugin(interfaceIid, driverName);
  if(pluginInfo.isNull()){
    setLastError(loader.lastError());
    return false;
  }
  mCurrentPlugin = loader.loadPlugin(pluginInfo);
  if(mCurrentPlugin == nullptr){
    setLastError(loader.lastError());
    return false;
  }
  mCurrentPluginInfo = pluginInfo;

  return true;
}

void PluginInstance::setCastToPluginInterfaceError(const QString& interfaceIid, const QString & driverName)
{
  const QString msg = tr("Could load a plugin that matches IID '%1' and supports driver '%2', but casting to the given plugin interface failed.")
                      .arg(interfaceIid, driverName);
  const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
  setLastError(error);
}

void PluginInstance::setLastError(const Mdt::Error & error)
{
  Q_ASSERT(!error.isNull());

  mLastError = error;
}

}} // namespace Mdt{ namespace Sql{
