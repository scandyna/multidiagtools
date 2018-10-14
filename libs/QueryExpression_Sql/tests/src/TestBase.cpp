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
#include "TestBase.h"
#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include "Mdt/Sql/SQLiteDatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using Mdt::Sql::SQLiteConnectionParameters;
using Mdt::Sql::SQLiteDatabase;

bool TestBase::initDatabaseSqlite()
{
  SQLiteConnectionParameters parameters;

  // Create a database
  if(!mTempFile.open()){
    qWarning() << "Could not open file " << mTempFile.fileName();
    return false;
  }
  mTempFile.close();
  parameters.setDatabaseFile(mTempFile.fileName());
  mConnectionParameters = parameters.toConnectionParameters();

  SQLiteDatabase sqliteDb;
  if( !sqliteDb.openExisting(mConnectionParameters.databaseName()) ){
    qWarning() << "Could not open database, error: " << sqliteDb.lastError().text();
    return false;
  }
  mDatabase = sqliteDb.database();

  return true;
}
