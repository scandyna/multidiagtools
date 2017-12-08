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
#include "CreateFile.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QLatin1String>
#include <QStringBuilder>
#include <QDebug>

namespace Mdt{ namespace TestLib{

bool createFile(const QString& filePath)
{
  const QFileInfo fi(filePath);
  auto dir = fi.absoluteDir();

  if(!dir.mkpath(dir.absolutePath())){
    qWarning() << "Unable to create directory " << dir.absolutePath();
    return false;
  }
  QFile file(fi.absoluteFilePath());
  if(!file.open(QFile::WriteOnly)){
    qWarning() << "Unable to create file " << file.fileName() << ": " << file.errorString();
    return false;
  }
  file.close();

  return true;
}

bool createFileInDirectory(const QString& directoryPath, const QString& fileName)
{
  return createFile( QDir::cleanPath(directoryPath % QLatin1String("/") % fileName) );
}

}} // namespace Mdt{ namespace TestLib{
