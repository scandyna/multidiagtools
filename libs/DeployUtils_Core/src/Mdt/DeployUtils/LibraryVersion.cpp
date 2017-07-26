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
#include "LibraryVersion.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QStringRef>
#include <QVector>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

LibraryVersion::LibraryVersion(qint8 versionMajor, qint8 versionMinor, qint8 versionPatch)
 : mVersionMajor(versionMajor),
   mVersionMinor(versionMinor),
   mVersionPatch(versionPatch)
{
  Q_ASSERT(mVersionMajor >= 0);
  Q_ASSERT( (mVersionPatch < 0) || (mVersionMinor >= 0) );
}

LibraryVersion::LibraryVersion(const QStringRef & version)
{
//   const auto stringList = version.splitRef('.', QString::SkipEmptyParts);
  const auto stringList = version.split('.', QString::SkipEmptyParts);
  bool vMajorOk = true;
  bool vMinorOk = true;
  bool vPatchOk = true;

  switch(stringList.size()){
    case 1:
      mVersionMajor = stringList.at(0).toShort(&vMajorOk);
      break;
    case 2:
      mVersionMajor = stringList.at(0).toShort(&vMajorOk);
      mVersionMinor = stringList.at(1).toShort(&vMinorOk);
      break;
    case 3:
      mVersionMajor = stringList.at(0).toShort(&vMajorOk);
      mVersionMinor = stringList.at(1).toShort(&vMinorOk);
      mVersionPatch = stringList.at(2).toShort(&vPatchOk);
      break;
    default:
      break;
  }
  if(!vMajorOk || !vMinorOk || !vPatchOk){
    mVersionMajor = -1;
    mVersionMinor = -1;
    mVersionPatch = -1;
  }
}

QString LibraryVersion::toString() const
{
  QString version;

  if(isNull()){
    return version;
  }
  if(mVersionPatch >= 0){
    version = QString::number(mVersionMajor) % QLatin1String(".") % QString::number(mVersionMinor) % QLatin1String(".") % QString::number(mVersionPatch);
  }else if(mVersionMinor >= 0){
    version = QString::number(mVersionMajor) % QLatin1String(".") % QString::number(mVersionMinor);
  }else{
    version = QString::number(mVersionMajor);
  }

  return version;
}

}} // namespace Mdt{ namespace DeployUtils{
