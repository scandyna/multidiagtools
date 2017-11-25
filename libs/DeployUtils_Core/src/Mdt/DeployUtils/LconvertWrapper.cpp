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
#include "LconvertWrapper.h"
#include "Mdt/Error.h"
#include <QDir>
#include <QStringBuilder>
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

LconvertWrapper::LconvertWrapper(QObject* parent)
 : QtToolExecutableWrapper(parent)
{
}

bool LconvertWrapper::execLconvert(const QStringList & arguments)
{
  QString lconvert;

  if(qtBinPath().isEmpty()){
    lconvert = "lconvert";
  }else{
    lconvert = QDir::cleanPath( qtBinPath() % QLatin1String("/lconvert") );
  }
  if(!exec(lconvert, arguments)){
    return false;
  }

  return checkStdError();
}

bool LconvertWrapper::executeJoinQmFiles(const QStringList& inFilePathList, const QString& outFilePath)
{
  Q_ASSERT(!inFilePathList.isEmpty());
  Q_ASSERT(!outFilePath.isEmpty());

  QStringList arguments;

  for(const auto & inFilePath : inFilePathList){
    Q_ASSERT(!inFilePath.isEmpty());
    arguments.append("-i");
    arguments.append(inFilePath);
  }
  arguments.append("-o");
  arguments.append(outFilePath);

  return execLconvert(arguments);
}

bool LconvertWrapper::checkStdError()
{
  const auto stdErrorString = readAllStandardErrorString();

  if(!stdErrorString.isEmpty()){
    const QString msg = tr("Execution of lconvert %1 reported error: %2")
                        .arg(processArguments().join(' '), stdErrorString);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
