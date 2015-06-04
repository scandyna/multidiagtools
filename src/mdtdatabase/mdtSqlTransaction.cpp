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
#include "mdtSqlTransaction.h"
#include <QSqlError>
#include <QObject>

mdtSqlTransaction::mdtSqlTransaction(QSqlDatabase db)
 : pvDatabase(db),
   pvIsStarted(false)
{
}

mdtSqlTransaction::~mdtSqlTransaction()
{
  if(pvIsStarted){
    rollback();
  }
}

bool mdtSqlTransaction::begin()
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(QObject::tr("Cannot beginn transaction (database: '") + pvDatabase.databaseName() + QObject::tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTransaction");
    pvLastError.commit();
    return false;
  }
  pvIsStarted = true;
  return true;
}

bool mdtSqlTransaction::commit()
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(QObject::tr("Cannot commit transaction (database: '") + pvDatabase.databaseName() + QObject::tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTransaction");
    pvLastError.commit();
    return false;
  }
  pvIsStarted = false;
  return true;
}

bool mdtSqlTransaction::rollback()
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(QObject::tr("Cannot rollback transaction (database: '") + pvDatabase.databaseName() + QObject::tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTransaction");
    pvLastError.commit();
    return false;
  }
  pvIsStarted = false;
  return true;
}
