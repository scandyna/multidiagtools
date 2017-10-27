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
#include "RPathInfoList.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QDir>
#include <algorithm>

namespace Mdt{ namespace DeployUtils{

void RPathInfoList::setRelativePath(const QString & path)
{
  Q_ASSERT( !path.contains(QLatin1String("$ORIGIN")) );
  Q_ASSERT( !path.contains(':') );

  clear();
  mRPath.emplace_back( RPathInfo( buildRPathInfo(path) ) );
}

void RPathInfoList::appendPath(const QString & path)
{
  Q_ASSERT( !path.contains(QLatin1String("$ORIGIN")) );
  Q_ASSERT( !path.contains(':') );

  const auto rpath = buildRPathInfo(path);
  if(containsPath(rpath)){
    return;
  }
  mRPath.emplace_back(rpath);
}

void RPathInfoList::prpendPath(const QString& path)
{
  Q_ASSERT( !path.contains(QLatin1String("$ORIGIN")) );
  Q_ASSERT( !path.contains(':') );

  const auto rpath = buildRPathInfo(path);
  if(containsPath(rpath)){
    return;
  }
  mRPath.emplace( mRPath.cbegin(), rpath );
}

void RPathInfoList::prependRelativePath(const QString & path)
{
  Q_ASSERT( !path.contains(QLatin1String("$ORIGIN")) );
  Q_ASSERT( !path.contains(':') );

  const auto rpath = buildRPathInfo(path);
  Q_ASSERT(rpath.isRelative());
  if(containsPath(rpath)){
    return;
  }
  mRPath.emplace( mRPath.cbegin(), rpath );
}

void RPathInfoList::clear()
{
  mRPath.clear();
}

QString RPathInfoList::toStringLinux() const
{
  QString rpath;

  if(isEmpty()){
    return rpath;
  }
  rpath = mRPath[0].toStringLinux();
  for(unsigned int i = 1; i < mRPath.size(); ++i){
    rpath += QLatin1String(":") % mRPath[i].toStringLinux();
  }

  return rpath;
}

RPathInfo RPathInfoList::buildRPathInfo(const QString & path)
{
  const auto cleanPath = QDir::cleanPath(path);

  if(cleanPath.startsWith('/')){
    return RPathInfo(cleanPath, false);
  }else{
    if(cleanPath == QLatin1String(".")){
      return RPathInfo("", true);
    }
    return RPathInfo(cleanPath, true);
  }
}

RPathInfo RPathInfoList::buildRPathInfoFromRawPath(const QString& path)
{
  const auto cleanPath = QDir::cleanPath(path);

  if(cleanPath == QLatin1String("$ORIGIN")){
    return RPathInfo("", true);
  }
  if(cleanPath.startsWith(QLatin1String("$ORIGIN/"))){
    QString cleanPathCopy = cleanPath;
    cleanPathCopy.remove("$ORIGIN/");
    return RPathInfo(cleanPathCopy, true);
  }
  return RPathInfo(cleanPath, false);
}

bool RPathInfoList::containsPath(const RPathInfo & rpath)
{
  const auto pred = [&rpath](const RPathInfo & rpi){
    return rpath.path() == rpi.path();
  };
  return ( std::find_if(mRPath.cbegin(), mRPath.cend(), pred) != mRPath.cend() );
}

RPathInfoList RPathInfoList::fromRawRPath(const QString & rpathString)
{
  RPathInfoList rpathInfo;
  const auto rpath = rpathString.trimmed().split(':', QString::SkipEmptyParts);

  for(const auto & path : rpath){
    rpathInfo.mRPath.emplace_back( buildRPathInfoFromRawPath(path) );
  }

  return rpathInfo;
}

}} // namespace Mdt{ namespace DeployUtils{
