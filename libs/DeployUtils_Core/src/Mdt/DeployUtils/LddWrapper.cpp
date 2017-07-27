/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "LddWrapper.h"

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

LddWrapper::LddWrapper(QObject* parent)
 : ToolExecutableWrapper(parent)
{
}

bool LddWrapper::execFindDependencies(const QString & binaryFilePath)
{
  qDebug() << "LDD: exec on " << binaryFilePath;

  if(!exec("ldd", {"--abcd",binaryFilePath})){
    return false;
  }
  const auto stdErrorString = readAllStandardErrorString();
  if(!stdErrorString.isEmpty()){
    const QString msg = tr("Execution of ldd %1 reported error: %2").arg(binaryFilePath, stdErrorString);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
