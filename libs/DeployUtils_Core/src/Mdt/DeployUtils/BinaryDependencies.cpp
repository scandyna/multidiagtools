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
#include "BinaryDependencies.h"
#include "BinaryDependenciesImplementationInterface.h"
#include "BinaryDependenciesLdd.h"
#include "BinaryDependenciesObjdump.h"
#include "BinaryFormat.h"
#include <QFileInfo>
#include <QDir>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(QObject* parent)
 : QObject(parent)
{
}

BinaryDependencies::~BinaryDependencies()
{
}

void BinaryDependencies::setLibrarySearchFirstPathList(const PathList& pathList)
{
  mLibrarySearchFirstPathList = pathList;
}

void BinaryDependencies::setLibrarySearchFirstPathSuffixList(const QStringList & suffixList)
{
  mLibrarySearchFirstPathSuffixList = suffixList;
}

PathList BinaryDependencies::getLibrarySearchFirstPathList(BinaryFileDirectoryInclude binaryFileDirectoryInclude) const
{
  PathList paths;

  if( (binaryFileDirectoryInclude == IncludeBinaryFileDirectory) && (!mBinaryFileDirectoryPath.isEmpty()) ){
    paths.appendPath(mBinaryFileDirectoryPath);
  }
  if(!mLibrarySearchFirstPathList.isEmpty()){
    const auto & librarySearchFirstPathList = mLibrarySearchFirstPathList;
    const auto & suffixList = mLibrarySearchFirstPathSuffixList;
    for(const auto & path : librarySearchFirstPathList){
      paths.appendPath(path);
      for(const auto & suffix : suffixList){
        paths.appendPath( QDir::cleanPath(path + "/" + suffix) );
      }
    }
  }

  return paths;
}

bool BinaryDependencies::setBinaryFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  if(!fi.exists()){
    const auto msg = tr("File '%1' does not exist.").arg(filePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  mBinaryFilePath = filePath;
  mBinaryFileDirectoryPath = fi.absoluteDir().absolutePath();

  return true;
}

bool BinaryDependencies::findDependencies()
{
  // Choose a implementation regarding binary file format
  BinaryFormat bfmt;
  if(!bfmt.readFormat(mBinaryFilePath)){
    setLastError(bfmt.lastError());
    return false;
  }
  BinaryFileDirectoryInclude binaryFileDirectoryInclude;
  std::unique_ptr<BinaryDependenciesImplementationInterface> impl;
  switch(bfmt.operatindSystem()){
    case OperatingSystem::Linux:
      binaryFileDirectoryInclude = ExcludeBinaryFileDirectory;
      impl.reset(new BinaryDependenciesLdd);
      break;
    case OperatingSystem::Windows:
      binaryFileDirectoryInclude = IncludeBinaryFileDirectory;
      impl.reset(new BinaryDependenciesObjdump);
      break;
  }
  if(!impl){
    const QString msg = tr("Could not find a tool to get dependencies for file '%1'.").arg(mBinaryFilePath);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  // Setup implementation and run
  impl->setLibrarySearchFirstPathList( getLibrarySearchFirstPathList(binaryFileDirectoryInclude) );
  if(!impl->findDependencies(mBinaryFilePath)){
    setLastError(impl->lastError());
    return false;
  }
  // Store result
  mDependencies = impl->dependencies();

  return true;
}

bool BinaryDependencies::findDependencies(const QString& binaryFilePath)
{
  if(!setBinaryFile(binaryFilePath)){
    return false;
  }
  return findDependencies();
}

void BinaryDependencies::setLastError(const Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace DeployUtils{
