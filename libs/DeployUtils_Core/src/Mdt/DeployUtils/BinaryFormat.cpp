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
#include "BinaryFormat.h"
#include "ObjdumpWrapper.h"
#include "ObjdumpBinaryFormatParser.h"
#include "LibraryName.h"
#include <QFileInfo>
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

BinaryFormat::BinaryFormat(QObject* parent)
 : QObject(parent)
{
}

bool BinaryFormat::readFormat(const QString& binaryFilePath)
{
  ObjdumpWrapper objdump;
  if(!objdump.execReadFormat(binaryFilePath)){
    setLastError(objdump.lastError());
    return false;
  }

  ObjdumpBinaryFormatParser parser;
  if(!parser.parse(objdump.readAllStandardOutputString())){
    const QString msg = tr("Could not parse the binary format of file '%1'")
                        .arg(binaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  mOperatingSystem = parser.operatindSystem();
  mProcessor = parser.processor();

  return true;
}

bool BinaryFormat::isFileAnExecutableByExtension(const QFileInfo& fileInfo)
{
  if(fileInfo.fileName().isEmpty()){
    return false;
  }
  const auto extension = fileInfo.suffix();
  // Windows
  if( compareExtension(extension, "exe") || compareExtension(extension, "dll") ){
    return true;
  }
  // Linux
  if(extension.isEmpty()){
    return true;
  }
  LibraryName libName(fileInfo.fileName()); // LibraryName handles versionned so names
  if( compareExtension(libName.extension(), "so") ){
    return true;
  }

  return false;
}

bool BinaryFormat::compareExtension(const QString& extention, const char*const match)
{
  return ( QString::compare(extention, QLatin1String(match), Qt::CaseInsensitive) == 0 );
}

void BinaryFormat::setLastError(const Error & error)
{
  mLastError = error;
  mLastError.commit();
}


}} // namespace Mdt{ namespace DeployUtils{
