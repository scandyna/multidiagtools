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
#include "FindTranslation.h"
#include "SearchPathList.h"
#include "Console.h"
#include <QLatin1String>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
// #include <algorithm>
// #include <iterator>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

TranslationInfoList findQtTranslations(const LibraryInfo& qtLibrary, const PathList& pathPrefixList)
{

}

QString findQtTranslationsRoot(const PathList& pathPrefixList)
{
  return findDirectoryRoot("translations", {"qt5",".."}, pathPrefixList);
}

QString findMdtTranslationsRoot(const PathList& pathPrefixList)
{
  return findDirectoryRoot("translations", {".."}, pathPrefixList);
}

QString findDirectoryRoot(const QString& directory, const QStringList& possibleSubdirectories, const PathList& pathPrefixList)
{
  QString directoryRoot;
  SearchPathList searchPathList;
  searchPathList.setIncludePathPrefixes(true);
  if(pathPrefixList.isEmpty()){
    searchPathList.setPathPrefixList( PathList::getSystemLibraryPathList() );
  }else{
    searchPathList.setPathPrefixList(pathPrefixList);
  }
  searchPathList.setPathSuffixList(possibleSubdirectories);
  const auto pathList = searchPathList.pathList();

  for(const auto & path : pathList){
    qDebug() << "Searchin in " << path;
    QDir dir( QDir::cleanPath(path + QLatin1String("/") + directory) );
    if(dir.exists()){
      directoryRoot = dir.absolutePath();
      qDebug() << " - found: " << directoryRoot;
      return directoryRoot;
    }
  }

  return directoryRoot;
}

}} // namespace Mdt{ namespace DeployUtils{
