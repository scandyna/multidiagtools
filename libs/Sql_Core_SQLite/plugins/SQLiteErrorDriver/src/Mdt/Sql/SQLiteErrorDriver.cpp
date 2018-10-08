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
#include "SQLiteErrorDriver.h"
#include "sqlite3.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QLatin1String>

#include <QDebug>

namespace Mdt{ namespace Sql{

Mdt::ErrorCode SQLiteErrorDriver::errorCode(const QSqlError& sqlError) const
{
  bool ok = false;
  int sqliteErrorCode = sqlError.nativeErrorCode().toInt(&ok);

  if(!ok){
    return Mdt::ErrorCode::UnknownError;
  }
  if(isExtendedErrorCode(sqliteErrorCode)){
    return fromExtendedErrorCode(sqliteErrorCode);
  }
  return fromPrimaryErrorCode(sqliteErrorCode);
}

Mdt::Error::Level SQLiteErrorDriver::errorLevel(const QSqlError& sqlError) const
{
  return Mdt::Error::Critical;
}

bool SQLiteErrorDriver::isExtendedErrorCode(int errorCode)
{
  return errorCode > 255;
}

Mdt::ErrorCode SQLiteErrorDriver::fromPrimaryErrorCode(int errorCode)
{
  using Mdt::ErrorCode;

  switch(errorCode){
    case SQLITE_CONSTRAINT:
      return ErrorCode::ConstraintError;
  }

  return ErrorCode::UnknownError;
}

Mdt::ErrorCode SQLiteErrorDriver::fromExtendedErrorCode(int errorCode)
{
  using Mdt::ErrorCode;

  switch(errorCode){
    case SQLITE_CONSTRAINT_UNIQUE:
      return ErrorCode::UniqueConstraintError;
  }

  return ErrorCode::UnknownError;
}

}} // namespace Mdt{ namespace Sql{
