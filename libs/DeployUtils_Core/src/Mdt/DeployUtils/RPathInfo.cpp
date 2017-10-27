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
#include "RPathInfo.h"
#include <QLatin1String>
#include <QStringBuilder>
#include <QDir>

namespace Mdt{ namespace DeployUtils{

QString RPathInfo::toStringLinux() const
{
  QString path;

  if(isRelative()){
    if(mPath.isEmpty()){
      path = QLatin1String("$ORIGIN");
    }else{
      path = QLatin1String("$ORIGIN") % QDir::cleanPath( QLatin1String("/") % mPath );
    }
  }else{
    path = QDir::cleanPath( mPath );
  }

  return path;
}

}} // namespace Mdt{ namespace DeployUtils{
