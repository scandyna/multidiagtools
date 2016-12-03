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
#include "FilterExpressionTest.h"
#include "Mdt/Application.h"
#include "Mdt/FilterExpression/LikeExpression.h"
#include "Mdt/FilterExpression/LikeExpressionRegexTransform.h"
// #include <QString>
// #include <QVariant>
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>
// #include <type_traits>
// #include <typeinfo>

#include <QDebug>

namespace FilterExpression = Mdt::FilterExpression;

void FilterExpressionTest::initTestCase()
{
}

void FilterExpressionTest::cleanupTestCase()
{
}

/*
 * Compile time tests
 */


/*
 * Runtime tests
 */

QString likeExpressionTestFunction(const FilterExpression::LikeExpression & expr)
{
  return expr.expression();
}

void FilterExpressionTest::likeExpressionTest()
{
  using Like = FilterExpression::LikeExpression;

  // Must not compile
//   QCOMPARE( likeExpressionTestFunction("A?B") , QString("A?B") );

  QCOMPARE( likeExpressionTestFunction(Like("A?B")) , QString("A?B") );
}

void FilterExpressionTest::likeExpressionRegexTransformEscapeTest()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QFETCH(QString, str);
  QFETCH(QString, expectedResult);

  LikeExpressionRegexTransform::escapeRegexMetacharacters(str);
  QCOMPARE( str, expectedResult );
}

void FilterExpressionTest::likeExpressionRegexTransformEscapeTest_data()
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
   * Wildcards are transformed to regex metacharacters (later) if they are not escaped.
   * If already escaped, they must be keeped untouched.
   */
  // Check various combinaisons with token ?
  QTest::newRow("A") << "A" << "A";
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

void FilterExpressionTest::likeExpressionRegexTransformEscapeBenchmark()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QString str;
  QBENCHMARK{
    str = "*A?B{0102}??C*D\\?E";
    LikeExpressionRegexTransform::escapeRegexMetacharacters(str);
  }
  QCOMPARE(str, QString("*A?B\\{0102\\}??C*D\\?E"));
}

void FilterExpressionTest::likeExpressionRegexTransformTest()
{
//   using FilterExpression::LikeExpression;
  using FilterExpression::LikeExpressionRegexTransform;
  using Like = FilterExpression::LikeExpression;

  QFETCH(QString, likeExpression);
  QFETCH(QString, expectedRegex);
  QString regex;

  // Must not compile
//   QCOMPARE( LikeExpressionRegexTransform::getRegexPattern("A") , QString("A") );

  regex = LikeExpressionRegexTransform::getRegexPattern(Like(likeExpression));
  QCOMPARE(regex, expectedRegex);
}

void FilterExpressionTest::likeExpressionRegexTransformTest_data()
{
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<QString>("expectedRegex");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("LIKE A") << "A" << "^A$";
  QTest::newRow("LIKE AB") << "AB" << "^AB$";
  QTest::newRow("LIKE ABC") << "ABC" << "^ABC$";
  /*
   * Wildcards are transformed to regex metacharacters if they are not escaped.
   * If already escaped, they must be keeped untouched.
   */
  QTest::newRow("LIKE ?") << "?" << "^.$";
  QTest::newRow("LIKE \\?") << "\\?" << "^\\?$";
  QTest::newRow("LIKE ??") << "??" << "^..$";
  QTest::newRow("LIKE ?\\?") << "?\\?" << "^.\\?$";
  QTest::newRow("LIKE ?A") << "?A" << "^.A$";
  QTest::newRow("LIKE \\?A") << "\\?A" << "^\\?A$";
  QTest::newRow("LIKE A?") << "A?" << "^A.$";
  QTest::newRow("LIKE A\\?") << "A\\?" << "^A\\?$";
  QTest::newRow("LIKE _A") << "_A" << "^.A$";
  QTest::newRow("LIKE *A") << "*A" << "^.*A$";
  QTest::newRow("LIKE \\*A") << "\\*A" << "^\\*A$";
  QTest::newRow("LIKE A%") << "A%" << "^A.*$";
  QTest::newRow("LIKE *A?") << "*A?" << "^.*A.$";
  QTest::newRow("LIKE *A*") << "*A*" << "^.*A.*$";
  // Check alternate escaped and not escaped wildcards
  QTest::newRow("LIKE ?\\??") << "?\\??" << "^.\\?.$";
  QTest::newRow("LIKE ?\\???") << "?\\???" << "^.\\?..$";
  QTest::newRow("LIKE ?\\??\\?") << "?\\??\\?" << "^.\\?.\\?$";
  QTest::newRow("LIKE *\\**") << "*\\**" << "^.*\\*.*$";
  QTest::newRow("LIKE *\\***") << "*\\***" << "^.*\\*.*.*$";
  QTest::newRow("LIKE *\\**\\*") << "*\\**\\*" << "^.*\\*.*\\*$";
  // Check also strings which conatins every wildcard, in every order
  QTest::newRow("LIKE ?_%*") << "?_%*" << "^...*.*$";
  QTest::newRow("LIKE *?_%") << "*?_%" << "^.*...*$";
  QTest::newRow("LIKE %*?_") << "%*?_" << "^.*.*..$";
  QTest::newRow("LIKE _%*?") << "_%*?" << "^..*.*.$";
  // Check also with some dots
  QTest::newRow("LIKE .*") << ".*" << "^\\..*$";
  QTest::newRow("LIKE .%") << ".%" << "^\\..*$";
  QTest::newRow("LIKE .%.*") << ".%.*" << "^\\..*\\..*$";
  /*
   * All regexp metacharacters (that are not wildcards) must simply be escaped
   * (note that \ is a metacharacter)
   */
  // Check with expression containing regexp metacharacters
  QTest::newRow("LIKE {A}") << "{A}" << "^\\{A\\}$";
  QTest::newRow("LIKE {*A}") << "{*A}" << "^\\{.*A\\}$";
  QTest::newRow("LIKE [A]") << "[A]" << "^\\[A\\]$";
  QTest::newRow("LIKE (A)") << "(A)" << "^\\(A\\)$";
  QTest::newRow("LIKE 0x1.5") << "0x1.5" << "^0x1\\.5$";
  QTest::newRow("LIKE A+B") << "A+B" << "^A\\+B$";
  QTest::newRow("LIKE A|B") << "A|B" << "^A\\|B$";
  QTest::newRow("LIKE \\") << "\\" << "^\\\\$";
  QTest::newRow("LIKE \\*") << "\\*" << "^\\*$";
  // Check expressions with '\' followed by a regexp metacharacter
  QTest::newRow("LIKE A\\|B") << "A\\|B" << "^A\\\\\\|B$";  // Escape '\' and '|'
  // Check expressions with regexp abbreviations
  QTest::newRow("LIKE \\d") << "\\d" << "^\\\\d$";  // Similar to LIKE '\d'
  QTest::newRow("LIKE \\s") << "\\s" << "^\\\\s$";
  QTest::newRow("LIKE \\w") << "\\w" << "^\\\\w$";
  QTest::newRow("LIKE \\D") << "\\D" << "^\\\\D$";
  QTest::newRow("LIKE \\S") << "\\S" << "^\\\\S$";
  QTest::newRow("LIKE \\W") << "\\W" << "^\\\\W$";
  QTest::newRow("LIKE \\N") << "\\N" << "^\\\\N$";
  QTest::newRow("LIKE \\b") << "\\b" << "^\\\\b$";
}

void FilterExpressionTest::likeExpressionRegexTransformBenchmark()
{
  using FilterExpression::LikeExpression;
  using FilterExpression::LikeExpressionRegexTransform;

  QString result;
  LikeExpression likeExpr("*A?B{0102}??C*D\\?E");
  QBENCHMARK{
    result = LikeExpressionRegexTransform::getRegexPattern(likeExpr);
  }
  QCOMPARE(result, QString("^.*A.B\\{0102\\}..C.*D\\?E$"));
}

void FilterExpressionTest::likeExpressionRegexTest()
{
  using FilterExpression::LikeExpression;
  using FilterExpression::LikeExpressionRegexTransform;

  QFETCH(QString , str);
  QFETCH(QString , likeExpression);
  QFETCH(bool , expectedMatches);
  QRegularExpression regex;
  QString pattern;

  pattern = LikeExpressionRegexTransform::getRegexPattern( LikeExpression(likeExpression) );
  regex.setPattern(pattern);
  QVERIFY(regex.isValid());
  bool matches = regex.match(str).hasMatch();
  QCOMPARE( matches , expectedMatches );
}

void FilterExpressionTest::likeExpressionRegexTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<bool>("expectedMatches");

  QTest::newRow("A LIKE A |y") << "A" << "A" << true;
  QTest::newRow("A LIKE B |n") << "A" << "B" << false;
  QTest::newRow("ABC LIKE A |n") << "ABC" << "A" << false;
  QTest::newRow("ABC LIKE B |n") << "ABC" << "B" << false;
  QTest::newRow("ABC LIKE ?B |n") << "ABC" << "?B" << false;
  QTest::newRow("ABC LIKE B? |n") << "ABC" << "B?" << false;
  QTest::newRow("ABC LIKE ?B? |y") << "ABC" << "?B?" << true;
  QTest::newRow("ABC LIKE _B |n") << "ABC" << "_B" << false;
  QTest::newRow("ABC LIKE B_ |n") << "ABC" << "B_" << false;
  QTest::newRow("ABC LIKE _B_ |y") << "ABC" << "_B_" << true;
  QTest::newRow("ABC LIKE *B |n") << "ABC" << "*B" << false;
  QTest::newRow("ABC LIKE B* |n") << "ABC" << "B*" << false;
  QTest::newRow("ABC LIKE *B* |y") << "ABC" << "*B*" << true;
  QTest::newRow("ABC LIKE %B |n") << "ABC" << "%B" << false;
  QTest::newRow("ABC LIKE B% |n") << "ABC" << "B%" << false;
  QTest::newRow("ABC LIKE %B% |y") << "ABC" << "%B%" << true;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FilterExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
