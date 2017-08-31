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
#include "BinaryDependenciesImplementationInterface.h"

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesImplementationInterface::BinaryDependenciesImplementationInterface(QObject* parent)
 : QObject(parent)
{
}

void BinaryDependenciesImplementationInterface::setLibrarySearchFirstPathList(const PathList& pathList)
{
  mLibrarySearchFirstPathList = pathList;
}

bool BinaryDependenciesImplementationInterface::findDependencies(const LibraryInfoList & libraries)
{
  Q_ASSERT(!libraries.isEmpty());

  LibraryInfoList allDependencies;
  for(const auto & library : libraries){
    Q_ASSERT(!library.absoluteFilePath().isEmpty());
    if(!findDependencies(library.absoluteFilePath())){
      return false;
    }
    allDependencies.addLibraries(mDependencies);
  }
  mDependencies = allDependencies;
  return true;
}

void BinaryDependenciesImplementationInterface::setDependencies(const LibraryInfoList & dependencies)
{
  mDependencies = dependencies;
}

void BinaryDependenciesImplementationInterface::setLastError(const Error& error)
{
  mLastError = error;
  mLastError.commit();
}

}} // namespace Mdt{ namespace DeployUtils{
