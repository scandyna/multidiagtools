/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "Mdt/Sql/SQLiteDatabase.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertQuery.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Mdt::Sql;

bool TestBase::initDatabaseTemporaryFile()
{
  if(!mTempFile.open()){
    qWarning() << "Could not open file " << mTempFile.fileName();
    return false;
  }
  mTempFile.close();
  mConnectionParameters.setDatabaseFile(mTempFile.fileName());

  return true;
}

bool TestBase::initDatabaseSqlite()
{
  const auto connection = SQLiteDatabase::addConnection();
  if(!connection){
    qWarning() << "Could not add a connection to as SQLite database: " << connection.error().text();
    return false;
  }
  mConnectionName = connection->name();

  if(!initDatabaseTemporaryFile()){
    return false;
  }

  SQLiteDatabase db(*connection, mConnectionParameters);
  const auto result = db.open();
  if(!result){
    qWarning() << "Could not open database, error: " << result.error().text();
    return false;
  }

  return true;
}
