/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClBase.h"
#include <QSqlError>

mdtClBase::mdtClBase(QSqlDatabase db) 
{
  pvDatabase = db;
}

mdtClBase::~mdtClBase() 
{
}

QSqlDatabase mdtClBase::database() 
{
  return pvDatabase;
}

mdtError mdtClBase::lastError() 
{
  return pvLastError;
}

bool mdtClBase::beginTransaction() 
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn transaction (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtClBase::rollbackTransaction() 
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn rollback (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtClBase::commitTransaction() 
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn commit (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

