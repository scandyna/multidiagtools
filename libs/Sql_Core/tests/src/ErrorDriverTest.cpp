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
#include "ErrorDriverTest.h"
#include "Mdt/Sql/ErrorDriver.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include <QSqlError>

using Mdt::Sql::ErrorDriver;
using Mdt::ErrorCode;

/*
 * Helpers
 */

QSqlError createSqlError(Mdt::ErrorCode errorCode)
{
  int code = static_cast<int>(errorCode);
  QString codeStr = QString::number(code);

  return QSqlError("Driver text", "DB text", QSqlError::ConnectionError, codeStr);
}

/*
 * Tests
 */

void ErrorDriverTest::nullDriverTest()
{
  QSqlError sqlError = createSqlError(Mdt::ErrorCode::UniqueConstraintError);

  ErrorDriver driver1("");
  QVERIFY(!driver1.isValid());
  QCOMPARE(driver1.errorCode(sqlError), ErrorCode::UnknownError);
  QCOMPARE(driver1.errorLevel(sqlError), Mdt::Error::Critical);

  QSqlDatabase db;
  QVERIFY(!db.isValid());
  ErrorDriver driver2(db);
  QVERIFY(!driver2.isValid());
  QCOMPARE(driver2.errorCode(sqlError), ErrorCode::UnknownError);
  QCOMPARE(driver2.errorLevel(sqlError), Mdt::Error::Critical);

  ErrorDriver driver3(nullptr);
  QVERIFY(!driver3.isValid());
  QCOMPARE(driver3.errorCode(sqlError), ErrorCode::UnknownError);
  QCOMPARE(driver3.errorLevel(sqlError), Mdt::Error::Critical);
}

void ErrorDriverTest::unhandledDriverTest()
{
  QSqlError sqlError = createSqlError(Mdt::ErrorCode::UniqueConstraintError);

  ErrorDriver driver("UnhandledDbDriver");
  QVERIFY(!driver.isValid());
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::UnknownError);
  QCOMPARE(driver.errorLevel(sqlError), Mdt::Error::Critical);
}

void ErrorDriverTest::testDriverTest()
{
  QSqlError sqlError = createSqlError(Mdt::ErrorCode::UniqueConstraintError);

  ErrorDriver driver("TEST");
  QVERIFY(driver.isValid());
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::UniqueConstraintError);
  QCOMPARE(driver.errorLevel(sqlError), Mdt::Error::Critical);
}

void ErrorDriverTest::swapDriverTest()
{
  QSqlError sqlError = createSqlError(Mdt::ErrorCode::UniqueConstraintError);

  ErrorDriver driver("TEST");
  QVERIFY(driver.isValid());
  QCOMPARE(driver.errorCode(sqlError), ErrorCode::UniqueConstraintError);

  ErrorDriver driver2("TEST2");
  QVERIFY(driver2.isValid());
  QCOMPARE(driver2.errorCode(sqlError), ErrorCode::UniqueConstraintError);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ErrorDriverTest test;

  return QTest::qExec(&test, argc, argv);
}
