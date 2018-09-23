/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "LibraryInfoImpl_Unix.h"
#include "LibraryInfoImpl.h"
#include "Mdt/Error.h"
#include <dlfcn.h>

namespace Mdt{

Mdt::Expected<QString> LibraryInfoImplUnix::getLibraryFilePath(const void * const address)
{
  Q_ASSERT(address != nullptr);

  Mdt::Expected<QString> path;

  Dl_info dlInfo;
  dlInfo.dli_fname = nullptr;
  const auto result = dladdr(address, &dlInfo);
  if(result == 0){
    const auto msg = tr("Call to dladdr() failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "LibraryInfoImpl");
    path = error;
    return path;
  }
  if(dlInfo.dli_fname == nullptr){
    const auto msg = tr("dladdr() did not provide the library file name.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "LibraryInfoImpl");
    path = error;
    return path;
  }
  path = QString::fromLocal8Bit(dlInfo.dli_fname);

  return path;
}


Mdt::Expected<QString> getLibraryFilePath(const void * const address)
{
  return LibraryInfoImplUnix::getLibraryFilePath(address);
}

} // namespace Mdt{
