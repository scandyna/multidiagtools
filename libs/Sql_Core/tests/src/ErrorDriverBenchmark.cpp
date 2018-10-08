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
#include "ErrorDriverBenchmark.h"
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
 * Benchmarks
 */

void ErrorDriverBenchmark::instantiateAndGetErrorBenchmark()
{
  QSqlError sqlError = createSqlError(Mdt::ErrorCode::UniqueConstraintError);
  Mdt::ErrorCode result;

  QBENCHMARK{
    ErrorDriver driver("TEST");
    Q_ASSERT(driver.isValid());
    result = driver.errorCode(sqlError);
  }

  QCOMPARE(result, ErrorCode::UniqueConstraintError);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  ErrorDriverBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
