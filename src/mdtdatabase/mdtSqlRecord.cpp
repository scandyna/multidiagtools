/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlRecord.h"
#include <QSqlError>
#include <QSqlField>

mdtSqlRecord::mdtSqlRecord() 
{
}

mdtSqlRecord::~mdtSqlRecord() 
{
}

bool mdtSqlRecord::addField(const QString & fieldName, const QString & tableName, const QSqlDatabase & db) 
{
  QSqlRecord dbRecord;
  QSqlField fld;

  // Get record object to fetch attributes
  dbRecord = db.record(tableName);
  if(dbRecord.isEmpty()){
    QSqlError sqlError = db.lastError();
    pvLastError.setError("Cannot get field information. Field: " + fieldName + ", table: " + tableName, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
    pvLastError.commit();
    return false;
  }
  // Get field and add it
  fld = dbRecord.field(fieldName);
  if(!fld.isValid()){
    pvLastError.setError("Field '" + fieldName + "' was not found in table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
    pvLastError.commit();
    return false;
  }
  append(fld);

  return true;
}

mdtError mdtSqlRecord::lastError() const
{
  return pvLastError;
}

mdtError & mdtSqlRecord::lastErrorW() 
{
  return pvLastError;
}

