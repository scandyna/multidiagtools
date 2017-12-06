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
#include "QtToolExecutableWrapper.h"
#include "Mdt/FileSystem/SearchPathList.h"
#include <QDir>
#include <QFileInfo>

#include <QDebug>

using namespace Mdt::FileSystem;

namespace Mdt{ namespace DeployUtils{

QtToolExecutableWrapper::QtToolExecutableWrapper(QObject* parent)
 : ToolExecutableWrapper(parent)
{
}

bool QtToolExecutableWrapper::findQtBinPath(const PathList & pathPrefixList)
{
  QString directoryRoot;
  SearchPathList searchPathList;
  searchPathList.setIncludePathPrefixes(true);
  searchPathList.setPathPrefixList(pathPrefixList);
  searchPathList.setPathSuffixList({"bin","qt5/bin"});
  const auto pathList = searchPathList.pathList();

  mQtBinPath.clear();
  for(const auto & path : pathList){
    QDir dir(path);
    if(dir.exists()){
      qDebug() << " Searchin in " << path;
      const auto fiList = dir.entryInfoList(QDir::Files);
      for(const auto & fi : fiList){
        if(fi.baseName() == QLatin1String("lconvert")){
          mQtBinPath = path;
          return true;
        }
      }
    }
  }

  return false;
}

}} // namespace Mdt{ namespace DeployUtils{
