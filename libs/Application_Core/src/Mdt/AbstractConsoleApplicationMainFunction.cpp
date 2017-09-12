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
#include "AbstractConsoleApplicationMainFunction.h"
#include <QCoreApplication>
#include <QTimer>

namespace Mdt{

AbstractConsoleApplicationMainFunction::AbstractConsoleApplicationMainFunction(QObject* parent)
 : QObject(parent)
{
  // If we let 0ms, we have a lots of memory leaks about dbus reported by ASAN
  QTimer::singleShot(10, this, &AbstractConsoleApplicationMainFunction::run);
}

QStringList AbstractConsoleApplicationMainFunction::getArguments()
{
  Q_ASSERT(QCoreApplication::instance() != nullptr);

  return QCoreApplication::arguments();
}

void AbstractConsoleApplicationMainFunction::aboutToQuit()
{
}

void AbstractConsoleApplicationMainFunction::run()
{
  Q_ASSERT(QCoreApplication::instance() != nullptr);

  int retCode = runMain();
  QCoreApplication::exit(retCode);
}

} // namespace Mdt{
