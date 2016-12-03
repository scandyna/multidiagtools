/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "LikeExpressionSqliteTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Expression/LikeExpressionSqlTransform.h"
#include "Mdt/FilterExpression/LikeExpression.h"

namespace Sql = Mdt::Sql;

void LikeExpressionSqliteTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void LikeExpressionSqliteTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LikeExpressionSqliteTest::sqlTransformTest()
{
  using Sql::Expression::LikeExpressionSqlTransform;
  using Like = Mdt::FilterExpression::LikeExpression;

  QFETCH(QString, likeExpression);
  QFETCH(QString, expectedSql);
  QString sql;
  auto db = mDatabase;

  sql = LikeExpressionSqlTransform::getSql(Like(likeExpression), db);
  QCOMPARE(sql, expectedSql);
}

void LikeExpressionSqliteTest::sqlTransformTest_data()
{
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<QString>("expectedSql");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("LIKE A") << "A" << "LIKE 'A' ESCAPE '\\'";
  QTest::newRow("LIKE AB") << "AB" << "LIKE 'AB' ESCAPE '\\'";
  QTest::newRow("LIKE ABC") << "ABC" << "LIKE 'ABC' ESCAPE '\\'";
  /*
   * Wildcards are transformed to SQL LIKE metacharacters if they are not escaped.
   * If they are escaped:
   *  - '_' or '%' must be escaped
   *  - '?' or '*' must not be scaped (are regular characters in SQL LIKE clause)
   */
  // Check simple transforms
  QTest::newRow("LIKE ?") << "?" << "LIKE '_'";
  QTest::newRow("LIKE _") << "_" << "LIKE '_'";
  QTest::newRow("LIKE *") << "*" << "LIKE '%'";
  QTest::newRow("LIKE %") << "%" << "LIKE '%'";
  // Check simple transforms with escaped wildcards
  QTest::newRow("LIKE \\?") << "\\?" << "LIKE '?'";
  QTest::newRow("LIKE \\_") << "\\_" << "LIKE '\\_' ESCAPE '\\'";
  QTest::newRow("LIKE \\*") << "\\*" << "LIKE '*'";
  QTest::newRow("LIKE \\%") << "\\%" << "LIKE '\\%' ESCAPE '\\'";
  // Check alternate escaped and not escaped wildcards with *
  QTest::newRow("LIKE *\\**") << "*\\**" << "LIKE '%*%'";
  QTest::newRow("LIKE *\\***") << "*\\***" << "LIKE '%*%%'";
  QTest::newRow("LIKE *\\**\\*") << "*\\**\\*" << "LIKE '%*%*'";
  // Check alternate escaped and not escaped wildcards with %
  QTest::newRow("LIKE %\\%%") << "%\\%%" << "LIKE '%\\%%' ESCAPE '\\'";
  QTest::newRow("LIKE %\\%%%") << "%\\%%%" << "LIKE '%\\%%%' ESCAPE '\\'";
  QTest::newRow("LIKE %\\%%\\%") << "%\\%%\\%" << "LIKE '%\\%%\\%' ESCAPE '\\'";
  // Check with '
  QTest::newRow("LIKE '") << "'" << "LIKE ''''";
  // Check with '\'
  QTest::newRow("LIKE \\") << "\\" << "LIKE '\\'";
  QTest::newRow("LIKE %\\") << "%\\" << "LIKE '%\\'";
  
  
  QTest::newRow("LIKE ?") << "?" << "LIKE '_'";
  QTest::newRow("LIKE \\?") << "\\?" << "LIKE '?'";
  QTest::newRow("LIKE ??") << "??" << "LIKE '__'";
  QTest::newRow("LIKE ?\\?") << "?\\?" << "LIKE '_?'";
  QTest::newRow("LIKE ?A") << "?A" << "LIKE '_A'";
  QTest::newRow("LIKE \\?A") << "\\?A" << "LIKE '?A'";
  QTest::newRow("LIKE A?") << "A?" << "LIKE 'A_'";
  QTest::newRow("LIKE A\\?") << "A\\?" << "LIKE 'A?'";
  QTest::newRow("LIKE _A") << "_A" << "LIKE '_A'";
  QTest::newRow("LIKE *A") << "*A" << "^.*A$";
  QTest::newRow("LIKE \\*A") << "\\*A" << "^\\*A$";
  QTest::newRow("LIKE A%") << "A%" << "^A.*$";
  QTest::newRow("LIKE *A?") << "*A?" << "^.*A.$";
  QTest::newRow("LIKE *A*") << "*A*" << "^.*A.*$";
  // Check alternate escaped and not escaped wildcards
  QTest::newRow("LIKE ?\\??") << "?\\??" << "^.\\?.$";
  QTest::newRow("LIKE ?\\???") << "?\\???" << "^.\\?..$";
  QTest::newRow("LIKE ?\\??\\?") << "?\\??\\?" << "^.\\?.\\?$";
  // Check also strings which conatins every wildcard, in every order
  QTest::newRow("LIKE ?_%*") << "?_%*" << "^...*.*$";
  QTest::newRow("LIKE *?_%") << "*?_%" << "^.*...*$";
  QTest::newRow("LIKE %*?_") << "%*?_" << "^.*.*..$";
  QTest::newRow("LIKE _%*?") << "_%*?" << "^..*.*.$";
  // Check also with some dots
  QTest::newRow("LIKE .*") << ".*" << "^\\..*$";
  QTest::newRow("LIKE .%") << ".%" << "^\\..*$";
  QTest::newRow("LIKE .%.*") << ".%.*" << "^\\..*\\..*$";

}

void LikeExpressionSqliteTest::sqlTransformBenchmark()
{
  QFAIL("Not implemented");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LikeExpressionSqliteTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
