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
#include "LibraryInfoImpl_Windows.h"
#include "LibraryInfoImpl.h"
#include "Mdt/Error.h"
#include <windows.h>

#include <QDebug>

namespace Mdt{

Mdt::Expected<QString> LibraryInfoImplWindows::getLibraryFilePath(const void * const address)
{
  Q_ASSERT(address != nullptr);

  Mdt::Expected<QString> path;

  HMODULE module;
  const auto result = GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(address),
            &module);
  if(result == 0){
    const auto msg = tr("Call to GetModuleHandleExA() failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "LibraryInfoImpl");
    path = error;
    return path;
  }

  constexpr auto fileNameLength = 400;
  char fileName[fileNameLength];
  fileName[0] = '\0';
  GetModuleFileNameA(module, fileName, fileNameLength);
  path = QString::fromLocal8Bit(fileName);

  return path;
}

Mdt::Expected<QString> getLibraryFilePath(const void * const address)
{
  return LibraryInfoImplWindows::getLibraryFilePath(address);
}

} // namespace Mdt{
