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
#include "LibraryInfo.h"
#include "LibraryInfoImpl.h"
#include <QFileInfo>

namespace Mdt{

Mdt::Expected<QString> LibraryInfo::getPrefixPath()
{
  auto path = getInstalledLibrariesPath();

  if(!path){
    return path;
  }

  return path;
}

Mdt::Expected<QString> LibraryInfo::getPluginsPath()
{

}

Mdt::Expected<QString> LibraryInfo::getInstalledLibrariesPath()
{
  auto path = Mdt::getLibraryFilePathFromStaticFunction(&LibraryInfo::getInstalledLibrariesPath);

  if(!path){
    return path;
  }
  QFileInfo fileInfo(*path);
  Q_ASSERT(fileInfo.exists());
  path = fileInfo.canonicalPath();

  return path;
}

} // namespace Mdt{
