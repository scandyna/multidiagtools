/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ConsoleBackend.h"
#include "Mdt/Error.h"
#include <QDebug>
#include <QDebugStateSaver>
#include <QLatin1String>
#include <QStringBuilder>

namespace Mdt{ namespace ErrorLogger {

ConsoleBackend::~ConsoleBackend()
{
}

void ConsoleBackend::logError(const Error & error)
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
  msg = getErrorStackString(error) + getErrorString(error);
  dbg << msg;
}

QString ConsoleBackend::getErrorString(const Error & error) const
{
  QString str;
  QString informativeText;

  str = tr("Error occured in ") % error.functionName() % QStringLiteral("\n") \
      % QStringLiteral("-> ") % tr("Text: ") % error.text() % QStringLiteral("\n");
  informativeText = error.informativeText();
  if(!informativeText.isEmpty()){
    str += QStringLiteral("-> ") % tr("Informative text: ") % error.informativeText() % QStringLiteral("\n");
  }
  str += QStringLiteral("-> ") % tr("File: ") % error.fileName() + QStringLiteral("\n") \
       % QStringLiteral("-> ") % tr("Line: ") % QString::number(error.fileLine());

  return str;
}

QString ConsoleBackend::getErrorStackString(const Error & error) const
{
  QString str;
  const auto errorStack = error.getErrorStack();

  for(const auto & e : errorStack){
    /// \todo check if allready commited
    str += getErrorString(e) % QStringLiteral("\n");
  }

  return str;
}

}}  // namespace Mdt{ namespace ErrorLogger {
