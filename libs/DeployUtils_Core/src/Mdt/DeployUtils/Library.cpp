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
#include "Library.h"
#include "LibraryName.h"
#include <QtGlobal>
#include <QDir>
#include <QFileInfo>
#include <algorithm>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

Library::Library(QObject* parent)
 : QObject(parent)
{
}

bool Library::findLibrary(const QString & name, const PathList pathList, Library::SearchInSystemPaths searchInSystemPaths)
{
  Q_ASSERT(!name.isEmpty());
  Q_ASSERT( (searchInSystemPaths != ExcludeSystemPaths) || (!pathList.isEmpty()) );

  LibraryName libraryName(name);
  Q_ASSERT(!libraryName.isNull());
  PathList searchPathList = pathList;

  if(searchInSystemPaths == IncludeSystemPaths){
    addSystemPaths(searchPathList);
  }
  qDebug() << "Searchin library " << name << " ...";
  // Lambda to check if a file matches request library
  const auto matchFile = [libraryName, this](const QFileInfo & fileInfo){
    LibraryName currentFile(fileInfo.fileName());
    if(currentFile.name() == libraryName.name()){
      qDebug() << " --> FOUND ! file: " << fileInfo.absoluteFilePath();
      mLibraryInfo.setAbsoluteFilePath(fileInfo.absoluteFilePath());
      return true;
    }
    return false;
  };
  // Lambda that search the library in given path
  const auto searchInDirectory = [libraryName, matchFile](const QString & path){
    qDebug() << " Try in " << path;
    const QDir currentDirectoy(path);
    const auto fileInfoList = currentDirectoy.entryInfoList();
    const auto it = std::find_if(fileInfoList.cbegin(), fileInfoList.cend(), matchFile);
    return (it != fileInfoList.cend());
  };
  // Search in all paths
  const auto it = std::find_if(searchPathList.cbegin(), searchPathList.cend(), searchInDirectory);
  if(it == searchPathList.cend()){
    const auto searchedPathString = searchPathList.toStringList().join(" , ");
    const auto msg = tr("Could not find library '%1' in one of the following paths: %2")
                     .arg(libraryName.toFullNameLinux(), searchedPathString);
    mLastError = mdtErrorNewQ(msg, Error::Critical, this);
    return false;
  }

  return true;
}

void Library::addSystemPaths(PathList & pathList)
{
#ifdef Q_OS_UNIX
  pathList.appendPath("/usr/lib");
  pathList.appendPath("/usr/lib/x86_64-linux-gnu");
  pathList.appendPath("/usr/lib32");
  pathList.appendPath("/usr/libx32");
  pathList.appendPath("/lib");
  pathList.appendPath("/lib32");
  pathList.appendPath("/lib64");
  pathList.appendPath("/libx32");
#endif // #ifdef Q_OS_UNIX
}

}} // namespace Mdt{ namespace DeployUtils{
