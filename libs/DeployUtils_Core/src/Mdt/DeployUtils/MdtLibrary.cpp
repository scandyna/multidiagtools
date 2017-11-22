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
#include "MdtLibrary.h"
#include "LibraryName.h"
#include <QLatin1String>
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace DeployUtils{

LibraryInfoList MdtLibrary::getMdtLibraries(const LibraryInfoList& libraries)
{
  LibraryInfoList mdtLibraries;

  std::copy_if(libraries.cbegin(), libraries.cend(), std::back_inserter(mdtLibraries), isMdtLibrary);

  return mdtLibraries;
}

QString MdtLibrary::baseName(const LibraryInfo & library)
{
  const auto name = library.libraryName().name();
  if(name.startsWith(QLatin1String("Mdt0"), Qt::CaseInsensitive)){
    return name.right(name.size() - 4);
  }
  return name;
}

QStringList MdtLibrary::getBaseNames(const LibraryInfoList& libraries)
{
  QStringList baseNames;

  for(const auto & library : libraries){
    baseNames.append( baseName(library) );
  }

  return baseNames;
}

bool MdtLibrary::isMdtLibrary(const LibraryInfo& libraryInfo)
{
  return libraryInfo.libraryName().name().startsWith(QLatin1String("Mdt0"), Qt::CaseInsensitive);
}

}} // namespace Mdt{ namespace DeployUtils{
