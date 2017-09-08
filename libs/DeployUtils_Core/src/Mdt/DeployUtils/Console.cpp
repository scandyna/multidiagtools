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
#include "Console.h"
#include "ConsoleStream.h"

namespace Mdt{ namespace DeployUtils{

int Console::Level = 1;

void Console::setLevel(int level)
{
  Q_ASSERT(level >= 0);

  Console::Level = level;
}

ConsoleStream Console::info(int minlevel)
{
  Q_ASSERT(minlevel >= 0);

  return ConsoleStream(QtInfoMsg, minlevel, Console::level());
}

ConsoleStream Console::error()
{
  return ConsoleStream(QtWarningMsg, 0, Console::level());
}

}} // namespace Mdt{ namespace DeployUtils{
