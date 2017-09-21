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
#include "LibraryName.h"
#include <QFileInfo>
#include <iterator>
#include <algorithm>

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesImplementationInterface::BinaryDependenciesImplementationInterface(QObject* parent)
 : QObject(parent)
{
}

void BinaryDependenciesImplementationInterface::setLibrarySearchFirstPathList(const PathList& pathList)
{
  mLibrarySearchFirstPathList = pathList;
}

bool BinaryDependenciesImplementationInterface::findDependencies(const QStringList& binariesFilePaths)
{
  Q_ASSERT(!binariesFilePaths.isEmpty());

  LibraryInfoList allDependencies;
  for(const auto & filePath : binariesFilePaths){
    Q_ASSERT(!filePath.isEmpty());
    if(!findDependencies(filePath)){
      return false;
    }
    allDependencies.addLibraries(mDependencies);
  }
  setDependencies(allDependencies, binariesFilePaths);

  return true;
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
  setDependencies(allDependencies, libraries);

  return true;
}

void BinaryDependenciesImplementationInterface::setDependencies(const LibraryInfoList & dependencies)
{
  mDependencies = dependencies;
}

void BinaryDependenciesImplementationInterface::setDependencies(const LibraryInfoList & dependencies, const QStringList & librariesToExclude)
{
  const auto pred = [librariesToExclude](const LibraryInfo & li){
    return !listContainsLibrary(librariesToExclude, li);
  };
  mDependencies.clear();
  std::copy_if( dependencies.cbegin(), dependencies.cend(), std::back_inserter(mDependencies), pred );
}

void BinaryDependenciesImplementationInterface::setDependencies(const LibraryInfoList & dependencies, const LibraryInfoList & librariesToExclude)
{
  const auto pred = [librariesToExclude](const LibraryInfo & li){
    return !listContainsLibrary(librariesToExclude, li);
  };
  mDependencies.clear();
  std::copy_if( dependencies.cbegin(), dependencies.cend(), std::back_inserter(mDependencies), pred );
}

void BinaryDependenciesImplementationInterface::setLastError(const Error& error)
{
  mLastError = error;
  mLastError.commit();
}

bool BinaryDependenciesImplementationInterface::listContainsLibrary(const QStringList& list, const LibraryInfo& li)
{
  const auto pred = [li](const QString & filePath){
    const auto libName = LibraryName( QFileInfo(filePath).fileName() );
    return ( QString::compare( libName.name(), li.libraryName().name(), Qt::CaseSensitive ) == 0 );
  };
  return ( std::find_if(list.cbegin(), list.cend(), pred) != list.cend() );
}

bool BinaryDependenciesImplementationInterface::listContainsLibrary(const LibraryInfoList& list, const LibraryInfo& li)
{
  const auto pred = [li](const LibraryInfo & lia){
    return ( QString::compare( lia.libraryName().name(), li.libraryName().name(), Qt::CaseSensitive ) == 0 );
  };
  return ( std::find_if(list.cbegin(), list.cend(), pred) != list.cend() );
}

}} // namespace Mdt{ namespace DeployUtils{
