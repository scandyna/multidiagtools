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
#include "FileBackendFormatEngine.h"
#include <QString>
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>
#include <QDateTime>

namespace Mdt{ namespace ErrorLogger {

QString FileBackendFormatEngine::formatError(const Error & error) const
{
  /// \todo Add also support for error stack
  QString str;

  // Prepare line beginning
  const QString lineBegin = QDateTime::currentDateTime().toString(QLatin1String("yyyy.MM.dd hh:mm:ss")) % QLatin1Char(' ') \
                            % errorLevelText(error.level()) % QLatin1String(": ");
  // Build message
  str = lineBegin % QLatin1String("In ") % error.functionName() % QLatin1Char('\n') \
      % lineBegin + QLatin1String("-> ") % tr("Text: ") % error.text() % QLatin1Char('\n');
  const auto informativeText = error.informativeText();
  if(!informativeText.isEmpty()){
    str += lineBegin % QLatin1String("-> ") % error.informativeText() % QLatin1Char('\n');
  }
  str += lineBegin % QLatin1String("-> ") % tr("File: ") % error.fileName() % QLatin1Char('\n') \
       % lineBegin % QLatin1String("-> ") % tr("Line: ") % QString::number(error.fileLine()); /// % QLatin1Char('\n');

  return str;
}

QString FileBackendFormatEngine::errorLevelText(Mdt::Error::Level level) const
{
  switch(level){
    case Error::NoError:
      return QLatin1String("[NoError]");
    case Error::Critical:
      return QLatin1String("[Critical]");
    case Error::Warning:
      return QLatin1String("[Warning]");
    case Error::Info:
      return QLatin1String("[Info]");
  }
  return QString();
}

}} // namespace Mdt{ namespace ErrorLogger {
