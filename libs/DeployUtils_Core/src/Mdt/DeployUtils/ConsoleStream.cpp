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
#include "ConsoleStream.h"
#include "Mdt/Error.h"
#include <QDebug>

namespace Mdt{ namespace DeployUtils{

ConsoleStream::ConsoleStream(QtMsgType msgType, int minLevel, int level)
{
  if(minLevel <= level){
    mDbg.reset(new QDebug(msgType));
    mDbg->noquote();
    mDbg->nospace();
  }
}

ConsoleStream& ConsoleStream::operator<<(const char* str)
{
  if(mDbg){
    *mDbg << str;
  }
  return *this;
}

ConsoleStream & ConsoleStream::operator<<(const QString & str)
{
//   std::cout << " operator<<(const QString & str)" << std::endl;
  if(mDbg){
    *mDbg << str;
  }
  return *this;
}

ConsoleStream& ConsoleStream::operator<<(const Error & error)
{
  if(mDbg){
    *mDbg << error.text();
  }
  return *this;
}

}} // namespace Mdt{ namespace DeployUtils{
