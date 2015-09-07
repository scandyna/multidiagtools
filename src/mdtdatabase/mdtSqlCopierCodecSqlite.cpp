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
#include "mdtSqlCopierCodecSqlite.h"
#include "mdtSqlSchemaTable.h"
#include <QSqlError>

#include <QDebug>

bool mdtSqlCopierCodecSqlite::openTarget()
{
  auto cs = pvSettings;

  qDebug() << "CNN: " << cs.connectionName() << " , db: " << cs.filePath();

  // Get a database regarding connection name
  if(pvDatabase.connectionName() != cs.connectionName()){
    pvDatabase = QSqlDatabase::database(cs.connectionName(), false);
  }
  // Set database name (file path)
  if(pvDatabase.databaseName() != cs.filePath()){
    if(pvDatabase.isOpen()){
      pvDatabase.close();
    }
    pvDatabase.setDatabaseName(cs.filePath());
  }
  // Open database
  if(!pvDatabase.isOpen()){
    if(!pvDatabase.open()){
      QSqlError error = pvDatabase.lastError();
      pvLastError.setError(tr("Unable to open database") + " '" + cs.filePath() + "'", mdtError::Error);
      pvLastError.setSystemError(error.number(), error.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtSqlCopierCodecSqlite");
      pvLastError.commit();
      return false;
    }
  }
  // Check that requested table exists
  if(!pvDatabase.tables().contains(cs.tableName())){
    pvLastError.setError(tr("Table") + " '" + cs.tableName() + "'" + tr("does not exist in database") + " '" + cs.filePath() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlCopierCodecSqlite");
    pvLastError.commit();
    return false;
  }
  // Set fields
  QStringList fieldNames;
  mdtSqlSchemaTable ts;
  if(!ts.setupFromTable(cs.tableName(), pvDatabase)){
    pvLastError = ts.lastError();
    return false;
  }
  for(const auto & field : ts.fieldList()){
    fieldNames.append(field.name());
  }
  setFieldNameList(fieldNames);

  return true;
}
