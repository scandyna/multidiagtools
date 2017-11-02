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
#include <QRegularExpression>
#include <QStringBuilder>
#include <QLatin1String>
#include <QChar>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

LibraryName::LibraryName(const QString & fullName)
{
  if(fullName.isEmpty()){
    return;
  }
  mFullName = fullName;
  // Isolate the lib prefix
  int start = 0;
  if(fullName.startsWith(QLatin1String("lib"), Qt::CaseInsensitive)){
    mPrefix = mFullName.left(3);
    start = 3;
  }
  // Get extension and so version
  QStringRef extension;
  mVersion = parseExtension(mFullName, extension);
  mExtension = extension.toString();
  // Extract name
  if(extension.isNull()){
    mName = mFullName.mid(start);
  }else{
    const int extensionSize = fullName.size() - extension.position();
    mName = mFullName.mid( start, fullName.size() - start - extensionSize - 1 );
  }
}

bool LibraryName::hasNameDebugSuffix() const
{
  if(mName.isEmpty()){
    return false;
  }
  const auto nameSuffix = mName.at(mName.size() - 1);

  return ( (nameSuffix == 'd') || (nameSuffix == 'D') );
}

QString LibraryName::toFullNameLinux() const
{
  QString fullName = QLatin1String("lib") % mName % QLatin1String(".so");

  if(!mVersion.isNull()){
    fullName += QLatin1String(".") % mVersion.toString();
  }

  return fullName;
}

OperatingSystem LibraryName::operatingSystem(const QString & fullName)
{
  QStringRef extension;
  parseExtension(fullName, extension);
  if(extension.isNull()){
    return OperatingSystem::Unknown;
  }
  if(compareExtension(extension, "so")){
    return OperatingSystem::Linux;
  }
  if(compareExtension(extension, "dll")){
    return OperatingSystem::Windows;
  }
  return OperatingSystem::Unknown;
}

LibraryVersion LibraryName::parseExtension(const QString& fullName, QStringRef& extension)
{
  static const char *extensionRegEx = "(.so|.dll)";
  const int extentionIndex = fullName.lastIndexOf( QRegularExpression(extensionRegEx,  QRegularExpression::CaseInsensitiveOption) );
  if(extentionIndex >= 0){
    /*
     * Extract extension
     * We can have a versionned so name or simple extension
     */
    if(fullName.midRef(extentionIndex, 4) == QLatin1String(".so.")){
      extension = fullName.midRef(extentionIndex+1, 2);
      const int versionStartIndex = extentionIndex + 4;
      const int versionLen = fullName.size() - versionStartIndex;
      return LibraryVersion( fullName.midRef(versionStartIndex, versionLen) );
    }
    extension = fullName.midRef(extentionIndex+1);
  }
  return LibraryVersion();
}

bool LibraryName::compareExtension(const QStringRef & extension, const char*const s)
{
  return ( QStringRef::compare(extension, QLatin1String(s), Qt::CaseInsensitive) == 0 );
}

}} // namespace Mdt{ namespace DeployUtils{
