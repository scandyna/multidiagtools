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
#include "LibraryInfoList.h"
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace DeployUtils{

LibraryInfoList::LibraryInfoList(std::initializer_list<LibraryInfo> list)
{
  std::unique_copy( list.begin(), list.end(), std::back_inserter(mList) );
}

void LibraryInfoList::addLibrary(const LibraryInfo & library)
{
  if(mList.contains(library)){
    return;
  }
  mList.append(library);
}

void LibraryInfoList::addLibraries(const LibraryInfoList & libraries)
{
  for(const auto & library : libraries){
    addLibrary(library);
  }
}

void LibraryInfoList::push_back(const LibraryInfo & library)
{
  addLibrary(library);
}

bool LibraryInfoList::containsLibraryAbsoluteFilePath(const QString & filePath) const
{
  const auto pred = [filePath](const LibraryInfo & fi){
    return ( QString::compare(fi.absoluteFilePath(), filePath, Qt::CaseSensitive) == 0 );
  };
  return ( std::find_if(mList.cbegin(), mList.cend(), pred) != mList.cend() );
}

void LibraryInfoList::clear()
{
  mList.clear();
}

void LibraryInfoList::reserve(int size)
{
  mList.reserve(size);
}

}} // namespace Mdt{ namespace DeployUtils{
