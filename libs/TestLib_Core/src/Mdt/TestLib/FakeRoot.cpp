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
#include "FakeRoot.h"
#include <QDir>
#include <QLatin1String>
#include <QStringBuilder>
#include <QDebug>

namespace Mdt{ namespace TestLib{

QString pathWithFakeRoot(const QTemporaryDir& root, const QString& path)
{
  return QDir::cleanPath( root.path() % QLatin1String("/") % path );
}

void prependFakeRootToPath(const QTemporaryDir& root, QString& path)
{
  path = pathWithFakeRoot(root, path);
}

void prependFakeRootToPathList(const QTemporaryDir& root, QStringList& pathList)
{
  for(auto it = pathList.begin(); it != pathList.end(); ++it){
    *it = pathWithFakeRoot(root, *it);
  }
}

bool createPathInFakeRoot(const QTemporaryDir& root, const QString& path)
{
  QDir dir;
  if( !dir.mkpath( pathWithFakeRoot(root, path) ) ){
    qWarning() << "Could not create path " << pathWithFakeRoot(root, path);
    return false;
  }
  return true;
}

bool createPathListInFakeRoot(const QTemporaryDir& root, const QStringList& pathList)
{
  for(const auto & path : pathList){
    if(!createPathInFakeRoot(root, path)){
      return false;
    }
  }
  return true;
}

}} // namespace Mdt{ namespace TestLib{
