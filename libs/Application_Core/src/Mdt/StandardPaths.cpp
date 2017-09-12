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
#include "StandardPaths.h"
#include <QStandardPaths>
#include <QStringList>
#include <QLatin1String>
#include <QStringBuilder>
#include <QDir>

// #include <QDebug>

namespace Mdt{

QString StandardPaths::logDirectoryPath()
{
  return QDir::cleanPath( QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) % QLatin1String("/log") );
}

QString StandardPaths::cacheDirectoryPath()
{
  return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

// QString StandardPaths::mdtDataDirectoryPath()
// {
//   const auto locations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
//   
//   qDebug() << "locations: " << locations;
//   
//   if(locations.isEmpty()){
//     return QString();
//   }
//   return locations.at(0);
// }


} // namespace Mdt{
