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

#include <QVector>

#include <QStringRef>

#include <QRegularExpression>

#include <QDebug>

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
//   qDebug() << "start: " << start << ", nameLen: " << nameLen;
  mName = name.mid(start, nameLen);
//   qDebug() << "Name: " << mName;
  // Get version if available
  if( (extentionIndex >= 0 ) && (name.midRef(extentionIndex, 4) == QLatin1String(".so.")) ){
    qDebug() << "Have maybe a version";
    const int versionStartIndex = extentionIndex + 4;
    const int versionLen = name.size() - versionStartIndex;
    qDebug() << "start: " << versionStartIndex << " , len: " << versionLen;
    const auto versionString = name.midRef(versionStartIndex, versionLen);
    qDebug() << "V: " << versionString;
    mVersion = LibraryVersion(versionString);
  }
  
//   qDebug() << "extentionIndex: " << extentionIndex;
//   int N = name.size();
//   int n = N - start - (N - extentionIndex);
//   qDebug() << "N: " << N << " start: " << start << " ext: " << extentionIndex << "n: " << n;
//   qDebug() << "name: " << name.mid(start, n);
//   static const char *nameAndExtensionRegEx = "^.*(.so|.dll)";
//   const auto nameAndExtensionParts = name.splitRef(QRegularExpression(nameAndExtensionRegEx,  QRegularExpression::CaseInsensitiveOption), QString::SkipEmptyParts);
//   
//   qDebug() << "nameAndExtensionParts: " << nameAndExtensionParts;
}

QString LibraryName::toFullNameLinux() const
{

}


}} // namespace Mdt{ namespace DeployUtils{
