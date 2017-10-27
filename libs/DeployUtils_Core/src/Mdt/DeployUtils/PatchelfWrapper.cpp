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
#include "PatchelfWrapper.h"

namespace Mdt{ namespace DeployUtils{

PatchelfWrapper::PatchelfWrapper(QObject* parent)
 : ToolExecutableWrapper(parent)
{
}

bool PatchelfWrapper::execReadRPath(const QString & binaryFilePath)
{
  return execPatchelf(QStringList{"--print-rpath",binaryFilePath});
}

bool PatchelfWrapper::execWriteRPath(const QString & rpath, const QString& binaryFilePath)
{
  return execPatchelf(QStringList{"--set-rpath",rpath,binaryFilePath});
}

bool PatchelfWrapper::execPatchelf(const QStringList & arguments)
{
  return exec("patchelf", arguments);
}

}} // namespace Mdt{ namespace DeployUtils{
