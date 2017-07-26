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
#include "LibraryName.h"
#include <QLatin1String>
#include <QStringRef>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QLatin1String>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

LibraryName::LibraryName(const QString & name)
{
  if(name.isEmpty()){
    return;
  }
  // Isolate the lib prefix
  int start = 0;
  if(name.startsWith(QLatin1String("lib"), Qt::CaseInsensitive)){
    start = 3;
  }
  // Isolate extension, and maybe version, from the rest
  static const char *extensionRegEx = "(.so|.dll)";
  int extentionIndex = name.lastIndexOf( QRegularExpression(extensionRegEx,  QRegularExpression::CaseInsensitiveOption) );
  int nameLen = name.size() - start;
  if(extentionIndex >= 0){
    nameLen -= name.size() - extentionIndex;
  }
  mName = name.mid(start, nameLen);
  // Get version if available
  if( (extentionIndex >= 0 ) && (name.midRef(extentionIndex, 4) == QLatin1String(".so.")) ){
    const int versionStartIndex = extentionIndex + 4;
    const int versionLen = name.size() - versionStartIndex;
    const auto versionString = name.midRef(versionStartIndex, versionLen);
    mVersion = LibraryVersion(versionString);
  }
}

QString LibraryName::toFullNameLinux() const
{
  QString fullName = QLatin1String("lib") % mName % QLatin1String(".so");

  if(!mVersion.isNull()){
    fullName += QLatin1String(".") % mVersion.toString();
  }

  return fullName;
}


}} // namespace Mdt{ namespace DeployUtils{
