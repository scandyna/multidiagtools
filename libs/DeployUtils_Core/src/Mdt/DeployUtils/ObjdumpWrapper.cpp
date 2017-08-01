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
#include "ObjdumpWrapper.h"
#include <QStringList>
#include <QProcess>
#include <QStandardPaths>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

ObjdumpWrapper::ObjdumpWrapper(QObject* parent)
 : ToolExecutableWrapper(parent)
{

}

bool ObjdumpWrapper::execFindDependencies(const QString& binaryFilePath)
{
  return execObjdump(QStringList{"-p",binaryFilePath});
}

QString ObjdumpWrapper::findObjdump()
{
  QString objdumpPath;

  objdumpPath = QStandardPaths::findExecutable("objdump");
  if(objdumpPath.isEmpty()){
    const QString msg = tr("Could not find objdump executable.");
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
  }

  return objdumpPath;
}

bool ObjdumpWrapper::execObjdump(const QStringList & arguments)
{
  /*
   * Maybe we are cross-compiling on Linux for Windows
   * In this case, try to run objdump directly
   */
  if(exec("objdump", arguments)){
    return checkStdError();
  }
  /*
   * We have to find objdump executable first
   */
  if(lastProcessError() == QProcess::FailedToStart){
    const auto objdumpPath = findObjdump();
    if(objdumpPath.isEmpty()){
      return false;
    }
    if(exec(objdumpPath, arguments)){
      return checkStdError();
    }
  }
  // Here we have a error reported by ToolExecutableWrapper

  return false;
}

bool ObjdumpWrapper::checkStdError()
{
  const auto stdErrorString = readAllStandardErrorString();

  if(!stdErrorString.isEmpty()){
    const QString msg = tr("Execution of objdump %1 reported error: %2")
                        .arg(processArguments().join(' '), stdErrorString);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
