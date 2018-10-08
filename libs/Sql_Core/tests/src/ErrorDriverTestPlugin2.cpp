/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "ErrorDriverTestPlugin2.h"

namespace Mdt{ namespace Sql{

Mdt::ErrorCode ErrorDriverTestDriver2::errorCode(const QSqlError & sqlError) const
{
  if(!sqlError.isValid()){
    return Mdt::ErrorCode::UnknownError;
  }
  const int code = sqlError.nativeErrorCode().toInt();
  if( (code >= 0) && (code <= 3) ){
    return static_cast<Mdt::ErrorCode>(code);
  }
  return Mdt::ErrorCode::UnknownError;
}

Mdt::Error::Level ErrorDriverTestDriver2::errorLevel(const QSqlError&) const
{
  return Mdt::Error::Critical;
}

AbstractErrorDriver *ErrorDriverTestDriverPlugin2::create()
{
  return new ErrorDriverTestDriver2;
}

}} // namespace Mdt{ namespace Sql{
