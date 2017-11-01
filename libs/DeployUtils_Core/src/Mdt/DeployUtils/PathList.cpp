/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "PathList.h"
#include <QList>
#include <QtGlobal>
#include <QByteArray>
#include <QLibraryInfo>
#include <QDir>
#include <QChar>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

void PathList::appendPath(const QString& path)
{
  Q_ASSERT(!path.trimmed().isEmpty());

  mList.removeAll(path);
  mList.append(path);
}

void PathList::appendPathList(const PathList & pathList)
{
  for(const auto & path : pathList){
    if(!path.trimmed().isEmpty()){
      appendPath(path);
    }
  }
}

void PathList::prependPath(const QString& path)
{
  Q_ASSERT(!path.trimmed().isEmpty());

  mList.removeAll(path);
  mList.prepend(path);
}

void PathList::prependPathList(const PathList& pathList)
{
  const PathList existingPaths = *this;
  clear();
  for(const auto & path : pathList){
    if(!path.trimmed().isEmpty()){
      appendPath(path);
    }
  }
  appendPathList(existingPaths);
}

void PathList::clear()
{
  mList.clear();
}

PathList PathList::getSystemExecutablePathList()
{
  PathList pathList;

  const auto pathEnv = QString::fromLocal8Bit( qgetenv("PATH") );
#ifdef Q_OS_WIN
  const QChar separator(';');
#else
  const QChar separator(':');
#endif
  const auto rawPathList = pathEnv.split(separator);
  for(const auto & path : rawPathList){
    pathList.appendPath( QDir::cleanPath(path) );
  }

  return pathList;
}

PathList PathList::getSystemLibraryPathList()
{
  /*
   * QLibraryInfo helps to find where Qt libraries are installed (I guess)
   * For the other, we guess what system library paths could exist
   */
#ifdef Q_OS_UNIX
  PathList pathList = getSystemLibraryKnownPathListLinux();
#endif // #ifdef Q_OS_UNIX
#ifdef Q_OS_WIN
//  PathList pathList = getSystemLibraryKnownPathListWindows();
//  pathList.appendPathList( getSystemExecutablePathList() );
  PathList pathList = getSystemExecutablePathList();
#endif // #ifdef Q_OS_WIN
  pathList.appendPath( QLibraryInfo::location(QLibraryInfo::LibrariesPath) );

  return pathList;
}

PathList PathList::getSystemLibraryKnownPathListLinux()
{
  return PathList{"/usr/lib","/usr/lib/x86_64-linux-gnu","/usr/lib32","/usr/libx32","/lib","/lib32","/lib64","/libx32"};
}

PathList PathList::getSystemLibraryKnownPathListWindows()
{
  return PathList{"/windows/syswow64","/windows/system32","/windows/system"};
}

}} // namespace Mdt{ namespace DeployUtils{
