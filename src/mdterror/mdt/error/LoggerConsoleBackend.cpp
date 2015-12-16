/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "LoggerConsoleBackend.h"
#include "mdtErrorV2.h"
#include <QDebug>
#include <QDebugStateSaver>
#include <QLatin1String>

namespace mdt{ namespace error {

LoggerConsoleBackend::~LoggerConsoleBackend()
{
}

void LoggerConsoleBackend::logError(const mdtErrorV2& error)
{
  QDebug dbg(QtDebugMsg);
  QDebugStateSaver stateSaver(dbg);
  QString msg;

  // Setup debugger
  dbg.resetFormat();
  dbg.noquote();
  dbg.nospace();
  /*
   * We build the string and then print it out
   * (prevents splitting of informations of the same error)
   * Note: because potentially all errors are logged,
   *       we not have to print the error stack here.
   */
  msg = tr("Error occured in ") + getErrorString(error);
  dbg << msg;
}

QString LoggerConsoleBackend::getErrorString(const mdtErrorV2 & error) const
{
  QString str;
  QString informativeText;

  str = error.functionName() + QLatin1String("\n") \
      + QLatin1String("-> ") + tr("Text: ") + error.text() + QLatin1String("\n");
  informativeText = error.informativeText();
  if(!informativeText.isEmpty()){
    str += QLatin1String("-> ") + tr("Informative text: ") + error.informativeText() + QLatin1String("\n");
  }
  str += QLatin1String("-> ") + tr("File: ") + error.fileName() + QLatin1String("\n") \
       + QLatin1String("-> ") + tr("Line: ") + QString::number(error.fileLine());

  return str;
}

}}  // namespace mdt{ namespace error {
