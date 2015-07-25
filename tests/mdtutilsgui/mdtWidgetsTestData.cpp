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
#include "mdtWidgetsTestData.h"
#include <QTest>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QDebug>

mdtWidgetsTestData::mdtWidgetsTestData()
 : pvDatabase(QSqlDatabase::addDatabase("QSQLITE"))
{
  QTemporaryFile dbFile;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  pvDbFileInfo.setFile(dbFile.fileName() + ".db");
  pvDatabase.setDatabaseName(pvDbFileInfo.absoluteFilePath());
  if(!pvDatabase.open()){
    debugSqlError(pvDatabase.lastError());
  }
  QVERIFY(pvDatabase.isOpen());
}

void mdtWidgetsTestData::clear()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

bool mdtWidgetsTestData::createClientTable()
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "CREATE TABLE 'Client_tbl' (";
  sql += "'Id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'Name' VARCHAR(50) , ";
  sql += "'Remarks' VARCHAR(50) );";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

bool mdtWidgetsTestData::populateClientTable()
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Andy', 'REM 1')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Bety', 'REM 3')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Andy', 'REM 10')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Andy', 'REM 2')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Bety', 'REM 1')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Andy', 'REM 3')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Andy', 'REM 15')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Bety', 'REM 10')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Bety', 'REM 2')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  sql = "INSERT INTO 'Client_tbl' ('name', 'remarks') VALUES('Bety', 'REM 15')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

void mdtWidgetsTestData::debugSqlError(const QSqlQuery &query)
{
  qDebug() << "Error on query '" << query.lastQuery() << "': " << query.lastError();
}

void mdtWidgetsTestData::debugSqlError(const QSqlError &error)
{
  qDebug() << error;
}

void mdtWidgetsTestData::createDatabaseSchema()
{

}
