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
#include "ToolExecutableWrapper.h"
#include "Mdt/ErrorQProcess.h"

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

ToolExecutableWrapper::ToolExecutableWrapper(QObject* parent)
 : QObject(parent)
{
}

QString ToolExecutableWrapper::readAllStandardOutputString()
{
  return QString::fromLocal8Bit( mProcess.readAllStandardOutput() );
}

QString ToolExecutableWrapper::readAllStandardErrorString()
{
  return QString::fromLocal8Bit( mProcess.readAllStandardError() );
}

bool ToolExecutableWrapper::exec(const QString& exeName, const QStringList& arguments)
{
  mProcess.start(exeName, arguments);
  if(!mProcess.waitForStarted()){
    const QString msg = tr("Failed to start command '%1 %2'.").arg(exeName, arguments.join(' '));
    auto error = mdtErrorNewTQ(QProcess::ProcessError, mProcess.error(), msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQProcessQ(mProcess, this));
    setLastError(error);
    return false;
  }
  if(!mProcess.waitForFinished()){
    const QString msg = tr("Error occured from command '%1 %2'.").arg(exeName, arguments.join(' '));
    auto error = mdtErrorNewTQ(QProcess::ProcessError, mProcess.error(), msg, Mdt::Error::Critical, this);
    error.stackError(mdtErrorFromQProcessQ(mProcess, this));
    setLastError(error);
    return false;
  }
  if(mProcess.exitStatus() != QProcess::NormalExit){
    const QString msg = tr("Process for command '%1 %2' probably crashed.").arg(exeName, arguments.join(' '));
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  if(mProcess.exitCode() != 0){
    const QString msg = tr("Process for command '%1 %2' exit with code %3 .")
                        .arg(exeName, arguments.join(' '))
                        .arg(mProcess.exitCode());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return true;
}

void ToolExecutableWrapper::setLastError(const Error& error)
{
  mLastError = error;
  mLastError.commit();
}

}} // namespace Mdt{ namespace DeployUtils{
