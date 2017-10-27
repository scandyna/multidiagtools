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
#include "RPath.h"
#include "PatchelfWrapper.h"
#include "BinaryFormat.h"
#include "Console.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

RPath::RPath(QObject* parent)
 : QObject(parent)
{
}

bool RPath::readRPath(const QString & binaryFilePath)
{
  PatchelfWrapper patchelf;

  if(!patchelf.execReadRPath(binaryFilePath)){
    setLastError(patchelf.lastError());
    return false;
  }
  mRPath = RPathInfoList::fromRawRPath( patchelf.readAllStandardOutputString() );

  return true;
}

bool RPath::setRPathToOrigin(const QString& binaryFilePath)
{
  PatchelfWrapper patchelf;

  if(!patchelf.execWriteRPath("$ORIGIN", binaryFilePath)){
    setLastError(patchelf.lastError());
    return false;
  }

  return true;
}

bool RPath::setRPath(const RPathInfoList& rpath, const QString& binaryFilePath)
{
  PatchelfWrapper patchelf;

  if(!patchelf.execWriteRPath(rpath.toStringLinux(), binaryFilePath)){
    setLastError(patchelf.lastError());
    return false;
  }

  return true;
}

bool RPath::prependPath(const QString & path, const QString & binaryFilePath)
{
  if(!readRPath(binaryFilePath)){
    return false;
  }
  mRPath.prpendPath(path);
  if(!setRPath(mRPath, binaryFilePath)){
    return false;
  }
  return true;
}

bool RPath::prependPathForBinaries(const QString & path, const QString& directoryPath)
{
  QDir dir(directoryPath);

  if(!dir.exists()){
    const QString msg = tr("Directory '%1' does not exist, or is a file.").arg(directoryPath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  const auto fileInfoList = dir.entryInfoList(QDir::Files);
  for(const auto fileInfo : fileInfoList){
    if(BinaryFormat::isFileAnExecutableByExtension(fileInfo)){
      Console::info(2) << " prepend path '" << path << "' to '" << fileInfo.fileName();
      if(!prependPath(path, fileInfo.absoluteFilePath())){
        return false;
      }
    }
  }

  return true;
}

void RPath::setLastError(const Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace DeployUtils{
