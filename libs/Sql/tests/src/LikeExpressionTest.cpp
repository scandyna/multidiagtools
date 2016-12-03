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
#include "LikeExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Expression/LikeExpressionSqlTransform.h"
#include "Mdt/FilterExpression/LikeExpression.h"

namespace Sql = Mdt::Sql;

void LikeExpressionTest::initTestCase()
{
  /// \todo Remove if unused
  // Get database instance
//   mDatabase = QSqlDatabase::addDatabase("QSQLITE");
//   if(!mDatabase.isValid()){
//     QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
//   }
}

void LikeExpressionTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LikeExpressionTest::sqlTransformEscapeTest()
{
  using Sql::Expression::LikeExpressionSqlTransform;

  QFETCH(QString, str);
  QFETCH(QString, expectedResult);

  LikeExpressionSqlTransform::escapeMetacharacters(str);
  QCOMPARE( str, expectedResult );
}

void LikeExpressionTest::sqlTransformEscapeTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("expectedResult");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("AB") << "AB" << "AB";
  QTest::newRow("ABC") << "ABC" << "ABC";
  /*
   * Wildcards are transformed to LIKE metacharacters (later) if they are not escaped.
   * If already escaped, they must be keeped untouched.
   */
  // Check various combinaisons with token ?
  QTest::newRow("?") << "?" << "?";
  QTest::newRow("\\?") << "\\?" << "\\?";
  QTest::newRow("?A") << "?A" << "?A";
  QTest::newRow("\\?A") << "\\?A" << "\\?A";
  QTest::newRow("A?") << "A?" << "A?";
  QTest::newRow("A\\?") << "A\\?" << "A\\?";
  QTest::newRow("A?B") << "A?B" << "A?B";
  QTest::newRow("A\\?B") << "A\\?B" << "A\\?B";
  QTest::newRow("??") << "??" << "??";
  QTest::newRow("\\??") << "\\??" << "\\??";
  QTest::newRow("?\\?") << "?\\?" << "?\\?";
  QTest::newRow("\\?\\?") << "\\?\\?" << "\\?\\?";
  // Checks with other tokens
  QTest::newRow("A*") << "A*" << "A*";
  QTest::newRow("A\\*") << "A\\*" << "A\\*";
  QTest::newRow("A_") << "A_" << "A_";
  QTest::newRow("A\\_") << "A\\_" << "A\\_";
  QTest::newRow("A%") << "A%" << "A%";
  QTest::newRow("A\\%") << "A\\%" << "A\\%";
  /*
   * All regexp metacharacters (that are not wildcards) must simply be escaped
   * (note that \ is a metacharacter)
   */
  /// \todo ADAPT
  // Check escaping '\'
  QTest::newRow("\\") << "\\" << "\\\\";
  QTest::newRow("\\\\") << "\\\\" << "\\\\\\\\";
  QTest::newRow("\\A") << "\\A" << "\\\\A";
  QTest::newRow("\\AB") << "\\AB" << "\\\\AB";
  QTest::newRow("A\\") << "A\\" << "A\\\\";
  QTest::newRow("A\\B") << "A\\B" << "A\\\\B";
  QTest::newRow("A\\B\\") << "A\\B\\" << "A\\\\B\\\\";
  QTest::newRow("A\\B\\C") << "A\\B\\C" << "A\\\\B\\\\C";
  QTest::newRow("A\\B\\C\\") << "A\\B\\C\\" << "A\\\\B\\\\C\\\\";
  // Check escaping regex metacharacters
  QTest::newRow("{") << "{" << "\\{";
  QTest::newRow("A{") << "A{" << "A\\{";
  QTest::newRow("A\\{") << "A\\{" << "A\\\\\\{";  // Escape '\' and '{'
  QTest::newRow("A}") << "A}" << "A\\}";
  QTest::newRow("A\\}") << "A\\}" << "A\\\\\\}";  // Escape '\' and '}'
  QTest::newRow("A[") << "A[" << "A\\[";
  QTest::newRow("A\\[") << "A\\[" << "A\\\\\\[";  // Escape '\' and '['
  QTest::newRow("A]") << "A]" << "A\\]";
  QTest::newRow("A\\]") << "A\\]" << "A\\\\\\]";  // Escape '\' and ']'
  QTest::newRow("A(") << "A(" << "A\\(";
  QTest::newRow("A\\(") << "A\\(" << "A\\\\\\(";  // Escape '\' and '('
  QTest::newRow("A)") << "A)" << "A\\)";
  QTest::newRow("A\\)") << "A\\)" << "A\\\\\\)";  // Escape '\' and ')'
  QTest::newRow("^") << "^" << "\\^";
  QTest::newRow("$") << "$" << "\\$";
  QTest::newRow(".") << "." << "\\.";
  QTest::newRow("+") << "+" << "\\+";
  QTest::newRow("|") << "|" << "\\|";
  QTest::newRow("\\|") << "\\|" << "\\\\\\|";     // Escape '\' and '|'
  /*
   * Check with some expressions that could have some sense
   */
  QTest::newRow("A?B") << "A?B" << "A?B";
  QTest::newRow("A??B") << "A??B" << "A??B";
  QTest::newRow("*A") << "*A" << "*A";
  QTest::newRow("A*") << "A*" << "A*";
  QTest::newRow("*A*") << "*A*" << "*A*";

}

void LikeExpressionTest::sqlTransformEscapeBenchmark()
{
  QFAIL("Not implemented");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LikeExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
