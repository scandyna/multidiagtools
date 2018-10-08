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
#include "SQLiteErrorDriverPluginTest.h"
#include "Mdt/ErrorCode.h"
#include "Mdt/Sql/ErrorDriver.h"
#include "Mdt/Sql/SQLiteErrorDriver.h"
#include "sqlite3.h"
#include <QtTest/QTest>
#include <QCoreApplication>
#include <QSqlError>

#include <QDebug>

using namespace Mdt::Sql;
using Mdt::ErrorCode;

void SQLiteErrorDriverPluginTest::initTestCase()
{
}

void SQLiteErrorDriverPluginTest::cleanupTestCase()
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

void SQLiteErrorDriverPluginTest::isExtendedErrorCodeTest()
{
  QVERIFY(!SQLiteErrorDriver::isExtendedErrorCode(SQLITE_ERROR));
  QVERIFY(SQLiteErrorDriver::isExtendedErrorCode(SQLITE_CORRUPT_VTAB));
}

void SQLiteErrorDriverPluginTest::fromPrimaryErrorCodeTest()
{
  QCOMPARE(SQLiteErrorDriver::fromPrimaryErrorCode(SQLITE_CONSTRAINT), ErrorCode::ConstraintError);
}

void SQLiteErrorDriverPluginTest::fromExtendedErrorCodeTest()
{
  QCOMPARE(SQLiteErrorDriver::fromExtendedErrorCode(SQLITE_CONSTRAINT_UNIQUE), ErrorCode::UniqueConstraintError);
}

void SQLiteErrorDriverPluginTest::errorCodeTest()
{
  SQLiteErrorDriver driver;
  QSqlError sqlError;

  sqlError = createSqlError(SQLITE_CONSTRAINT);
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::ConstraintError);

  sqlError = createSqlError(SQLITE_CONSTRAINT_UNIQUE);
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::UniqueConstraintError);
}

void SQLiteErrorDriverPluginTest::errorLevelTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  SQLiteErrorDriverPluginTest test;

  return QTest::qExec(&test, argc, argv);
}
