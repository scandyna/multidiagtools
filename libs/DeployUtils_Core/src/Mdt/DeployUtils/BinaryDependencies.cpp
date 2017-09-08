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
#include "SearchPathList.h"
#include "Console.h"
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

bool BinaryDependencies::findDependencies(const QString & binaryFilePath, const Mdt::DeployUtils::PathList & searchFirstPathPrefixList)
{
//   SearchPathList searchPathList;
//   // Check that given file exists
//   QFileInfo binaryFileInfo(binaryFilePath);
//   if(!binaryFileInfo.exists()){
//     const auto msg = tr("File '%1' does not exist.").arg(binaryFilePath);
//     auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
//     setLastError(error);
//     return false;
//   }
//   // Choose a implementation regarding binary file format
//   BinaryFormat bfmt;
//   if(!bfmt.readFormat(binaryFileInfo.absoluteFilePath())){
//     setLastError(bfmt.lastError());
//     return false;
//   }
//   std::unique_ptr<BinaryDependenciesImplementationInterface> impl;
//   switch(bfmt.operatindSystem()){
//     case OperatingSystem::Linux:
//       impl.reset(new BinaryDependenciesLdd);
//       break;
//     case OperatingSystem::Windows:
//       impl.reset(new BinaryDependenciesObjdump);
//       searchPathList.prependPath( binaryFileInfo.absoluteDir().absolutePath() );
//       searchPathList.setPathPrefixList(searchFirstPathPrefixList);
//       searchPathList.setPathSuffixList({"bin","qt5/bin"});
//       impl->setLibrarySearchFirstPathList(searchPathList.pathList());
//       break;
//   }
//   if(!impl){
//     const QString msg = tr("Could not find a tool to get dependencies for file '%1'.").arg(binaryFilePath);
//     auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
//     setLastError(error);
//     return false;
//   }
  QFileInfo binaryFileInfo(binaryFilePath);
  auto impl = initImpl(binaryFileInfo, searchFirstPathPrefixList);
  if(!impl){
    return false;
  }
  // Process
  Console::info(2) << " searching dependencies for " << binaryFileInfo.fileName();
  if(!impl->findDependencies(binaryFileInfo.absoluteFilePath())){
    setLastError(impl->lastError());
    return false;
  }
  // Store result
  mDependencies = impl->dependencies();

  return true;
}

bool BinaryDependencies::findDependencies(const LibraryInfoList & libraries, const PathList & searchFirstPathPrefixList)
{
  if(libraries.isEmpty()){
    return true;
  }
  QFileInfo binaryFileInfo(libraries.at(0).absoluteFilePath());
  auto impl = initImpl(binaryFileInfo, searchFirstPathPrefixList);
  if(!impl){
    return false;
  }
  // Process
  if(!impl->findDependencies(libraries)){
    setLastError(impl->lastError());
    return false;
  }
  // Store result
  mDependencies = impl->dependencies();

  return true;
}

std::unique_ptr<BinaryDependenciesImplementationInterface> BinaryDependencies::initImpl(const QFileInfo & binaryFileInfo, const PathList & searchFirstPathPrefixList)
{
  std::unique_ptr<BinaryDependenciesImplementationInterface> impl;
  SearchPathList searchPathList;

  // Check that given file exists
  if(!binaryFileInfo.exists()){
    const auto msg = tr("File '%1' does not exist.").arg(binaryFileInfo.absoluteFilePath());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return impl;
  }
  // Choose a implementation regarding binary file format
  BinaryFormat bfmt;
  if(!bfmt.readFormat(binaryFileInfo.absoluteFilePath())){
    setLastError(bfmt.lastError());
    return impl;
  }
  switch(bfmt.operatindSystem()){
    case OperatingSystem::Unknown:
      break;
    case OperatingSystem::Linux:
      impl.reset(new BinaryDependenciesLdd);
      break;
    case OperatingSystem::Windows:
      impl.reset(new BinaryDependenciesObjdump);
      searchPathList.prependPath( binaryFileInfo.absoluteDir().absolutePath() );
      searchPathList.setPathPrefixList(searchFirstPathPrefixList);
      searchPathList.setPathSuffixList({"bin","qt5/bin"});
      impl->setLibrarySearchFirstPathList(searchPathList.pathList());
      break;
  }
  if(!impl){
    const QString msg = tr("Could not find a tool to get dependencies for file '%1'.").arg(binaryFileInfo.absoluteFilePath());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return impl;
  }

  return impl;
}

void BinaryDependencies::setLastError(const Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace DeployUtils{
