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
#include "SearchPathList.h"
#include <QList>
#include <QDir>
#include <QStringBuilder>
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

void SearchPathList::setIncludePathPrefixes(bool include)
{
  mIncludePathPrefixes = include;
  updatePathList();
}

void SearchPathList::setPathPrefixList(const PathList& pathList)
{
  mPathPrefixList = pathList;
  updatePathList();
}

void SearchPathList::setPathSuffixList(const QStringList& suffixList)
{
  mPathSuffixList = suffixList;
  updatePathList();
}

void SearchPathList::prependPath(const QString& path)
{
  mPathListToPrepend.prependPath(path);
  updatePathList();
}

void SearchPathList::appendPath(const QString& path)
{
  mPathListToAppend.appendPath(path);
  updatePathList();
}

void SearchPathList::clear()
{
  mIncludePathPrefixes = false;
  mPathList.clear();
  mPathListToPrepend.clear();
  mPathListToAppend.clear();
  mPathPrefixList.clear();
  mPathSuffixList.clear();
}

void SearchPathList::updatePathList()
{
  mPathList.clear();
  mPathList.appendPathList(mPathListToPrepend);
  const auto & prefixList = mPathPrefixList;
  const auto & suffixList = mPathSuffixList;
  for(const auto & path : prefixList){
    if(mIncludePathPrefixes){
      mPathList.appendPath(path);
    }
    for(const auto & suffix : suffixList){
      mPathList.appendPath( QDir::cleanPath(path % QLatin1String("/") % suffix) );
    }
  }
  mPathList.appendPathList(mPathListToAppend);
}

}} // namespace Mdt{ namespace DeployUtils{
