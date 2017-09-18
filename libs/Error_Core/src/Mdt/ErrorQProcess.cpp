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
#include "ErrorQProcess.h"
#include "Error.h"
#include <QProcess>
#include <QObject>
#include <QMetaObject>
#include <QLatin1String>

namespace Mdt{

Error::Level _mdtErrorlevelFromQProcessError(QProcess::ProcessError processError)
{
  switch(processError){
    case QProcess::FailedToStart:
    case QProcess::Crashed:
    case QProcess::Timedout:
    case QProcess::WriteError:
    case QProcess::ReadError:
    case QProcess::UnknownError:
      return Error::Critical;
  }
  return Error::Critical;
}

Error ErrorQProcess::fromQProcess(const QProcess& process, const QString& file, int line, const QString& className, const QString& functionName)
{
  Error error;

  const QString msg = QObject::tr("Reported from QProcess: %1").arg(process.errorString());
  error.setError<QProcess::ProcessError>(process.error(), msg, _mdtErrorlevelFromQProcessError(process.error()));
  error.setSource(file, line, className, functionName);

  return error;
}

Error ErrorQProcess::fromQProcess(const QProcess& process, const QString& file, int line, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  return fromQProcess(process, file, line, QLatin1String(obj->metaObject()->className()), functionName);
}

} // namespace Mdt{
