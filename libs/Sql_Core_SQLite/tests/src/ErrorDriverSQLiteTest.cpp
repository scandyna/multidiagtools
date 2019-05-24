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
#include "ErrorDriverSQLiteTest.h"
#include "Mdt/Sql/ErrorDriver.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/DeleteQuery.h"
#include "sqlite3.h"
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace Mdt::Sql;
using Mdt::ErrorCode;

void ErrorDriverSQLiteTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createPersonTable());
}

void ErrorDriverSQLiteTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

QSqlError createSqlError(int sqliteErrorCode, QSqlError::ErrorType errorType = QSqlError::UnknownError)
{
  QString codeStr = QString::number(sqliteErrorCode);

  return QSqlError("Driver text", "DB text", errorType, codeStr);
}

/*
 * Tests
 */

void ErrorDriverSQLiteTest::loadAndUseTest()
{
  QSqlError sqlError = createSqlError(SQLITE_CONSTRAINT_UNIQUE);

  ErrorDriver driver(database());
  QVERIFY(driver.isValid());
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::UniqueConstraintError);
  QCOMPARE(driver.errorLevel(sqlError), Mdt::Error::Critical);
}

void ErrorDriverSQLiteTest::catchUniqueConstraintTest()
{
  QVERIFY(clearPersonTable());

  QVERIFY(addPerson(1, "A"));
  QVERIFY(!addPerson(2, "A"));
  QCOMPARE(lasteError().error<ErrorCode>(), ErrorCode::UniqueConstraintError);
}

/*
 * Member helpers
 */

bool ErrorDriverSQLiteTest::createPersonTable()
{
  const QString sql = "CREATE TABLE Person ("\
                      " id INTEGER PRIMARY KEY,"\
                      " name UNIQUE NOT NULL"\
                      ")";
  QSqlQuery query(database());
  if(!query.exec(sql)){
    qWarning() << "Creating Person table failed: " << query.lastError().text();
    return false;
  }

  return true;
}

bool ErrorDriverSQLiteTest::addPerson(int id, const QString& name)
{
  InsertQuery query(connection());

  query.setTableName("Person");
  query.addValue(FieldName("id"), id);
  query.addValue(FieldName("name"), name);
  if(!query.exec()){
    setLastError(query.lastError());
    return false;
  }

  return true;
}

bool ErrorDriverSQLiteTest::clearPersonTable()
{
  DeleteQuery query(connection());

  query.setTableName("Person");
  if(!query.exec()){
    setLastError(query.lastError());
    return false;
  }

  return true;
}

void ErrorDriverSQLiteTest::setLastError(const Mdt::Error & error)
{
  Q_ASSERT(!error.isNull());

  mLastError = error;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ErrorDriverSQLiteTest test;

  return QTest::qExec(&test, argc, argv);
}
