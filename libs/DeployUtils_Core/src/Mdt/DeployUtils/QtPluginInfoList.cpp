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
#include "QtPluginInfoList.h"
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace DeployUtils{

QtPluginInfoList::QtPluginInfoList(std::initializer_list<QtPluginInfo> list)
{
  std::unique_copy( list.begin(), list.end(), std::back_inserter(mList) );
}

void QtPluginInfoList::addPlugin(const QtPluginInfo & plugin)
{
  if(mList.contains(plugin)){
    return;
  }
  mList.append(plugin);
}

void QtPluginInfoList::addPlugins(const QtPluginInfoList & plugins)
{
  for(const auto & plugin : plugins){
    addPlugin(plugin);
  }
}

LibraryInfoList QtPluginInfoList::toLibraryInfoList() const
{
  LibraryInfoList list;

  list.reserve(mList.count());
  const auto f = [&list](const QtPluginInfo & qpi){
    list.addLibrary(qpi.libraryInfo());
  };
  std::for_each(mList.cbegin(), mList.cend(), f);

  return list;
}

}} // namespace Mdt{ namespace DeployUtils{
