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
#include "Mdt/FilterExpression/LikeExpressionTerminal.h"
#include "Mdt/FilterExpression/LikeExpressionRegexTransform.h"
#include <boost/proto/matches.hpp>
#include <boost/proto/literal.hpp>
#include <boost/proto/transform/arg.hpp>

// #include <QDebug>

namespace FilterExpression = Mdt::FilterExpression;

void LikeExpressionTest::initTestCase()
{
}

void LikeExpressionTest::cleanupTestCase()
{
}

/*
 * Compile time tests
 */


/*
 * Runtime tests
 */

QString likeExpressionTestFunction(const FilterExpression::LikeExpressionTerminal<> & expr)
{
  return expr.expression();
}

void LikeExpressionTest::likeExpressionTest()
{
  using Like = FilterExpression::LikeExpressionTerminal<>;

  // Must not compile
//   QCOMPARE( likeExpressionTestFunction("A?B") , QString("A?B") );

  QCOMPARE( likeExpressionTestFunction(Like("A?B")) , QString("A?B") );

  Like A("?A?");
  QCOMPARE( boost::proto::_value()(A), QString("?A?"));
  QCOMPARE( boost::proto::value(A), QString("?A?"));
}

void LikeExpressionTest::likeExpressionRegexTransformEscapeTest()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QFETCH(QString, str);
  QFETCH(QString, expectedResult);

  LikeExpressionRegexTransform::escapeRegexMetacharacters(str);
  QCOMPARE( str, expectedResult );
}

void LikeExpressionTest::likeExpressionRegexTransformEscapeTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("expectedResult");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("AB") << "AB" << "AB";
  QTest::newRow("ABC") << "ABC" << "ABC";
  QTest::newRow("A_") << "A_" << "A_";
  QTest::newRow("A\\_") << "A\\_" << "A\\\\_";
  QTest::newRow("A%") << "A%" << "A%";
  QTest::newRow("A\\%") << "A\\%" << "A\\\\%";
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

void LikeExpressionTest::likeExpressionRegexTransformEscapeBenchmark()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QString str;
  QBENCHMARK{
    str = "*A?B{0102}??C*D\\?E";
    LikeExpressionRegexTransform::escapeRegexMetacharacters(str);
  }
  QCOMPARE(str, QString("*A?B\\{0102\\}??C*D\\?E"));
}

void LikeExpressionTest::likeExpressionRegexTransformTest()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QFETCH(QString, likeExpression);
  QFETCH(QString, expectedRegex);
  QString regex;

  regex = LikeExpressionRegexTransform::getRegexPattern(likeExpression);
  QCOMPARE(regex, expectedRegex);
}

void LikeExpressionTest::likeExpressionRegexTransformTest_data()
{
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<QString>("expectedRegex");

  /*
   * Check without any metacharacters
   */
  QTest::newRow("LIKE A") << "A" << "^A$";
  QTest::newRow("LIKE AB") << "AB" << "^AB$";
  QTest::newRow("LIKE ABC") << "ABC" << "^ABC$";
  QTest::newRow("LIKE _A") << "_A" << "^_A$";
  QTest::newRow("LIKE A%") << "A%" << "^A%$";
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
  QTest::newRow("LIKE *A") << "*A" << "^.*A$";
  QTest::newRow("LIKE \\*A") << "\\*A" << "^\\*A$";
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
  QTest::newRow("LIKE ?*") << "?*" << "^..*$";
  QTest::newRow("LIKE *?") << "*?" << "^.*.$";
  // Check also with some dots
  QTest::newRow("LIKE .?") << ".?" << "^\\..$";
  QTest::newRow("LIKE .*") << ".*" << "^\\..*$";
  QTest::newRow("LIKE .*.?") << ".*.?" << "^\\..*\\..$";
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

void LikeExpressionTest::likeExpressionRegexTransformBenchmark()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QString result;
  QString likeExpr("*A?B{0102}??C*D\\?E");
  QBENCHMARK{
    result = LikeExpressionRegexTransform::getRegexPattern(likeExpr);
  }
  QCOMPARE(result, QString("^.*A.B\\{0102\\}..C.*D\\?E$"));
}

void LikeExpressionTest::likeExpressionRegexTest()
{
  using FilterExpression::LikeExpressionRegexTransform;

  QFETCH(QString , str);
  QFETCH(QString , likeExpression);
  QFETCH(bool , expectedMatches);
  QRegularExpression regex;
  QString pattern;

  pattern = LikeExpressionRegexTransform::getRegexPattern( likeExpression );
  regex.setPattern(pattern);
  QVERIFY(regex.isValid());
  bool matches = regex.match(str).hasMatch();
  QCOMPARE( matches , expectedMatches );
}

void LikeExpressionTest::likeExpressionRegexTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("likeExpression");
  QTest::addColumn<bool>("expectedMatches");

  // Check with data without any wildcard or metacharacter
  QTest::newRow("A LIKE A") << "A" << "A" << true;
  QTest::newRow("B LIKE A") << "B" << "A" << false;
  QTest::newRow("AB LIKE A") << "AB" << "A" << false;
  QTest::newRow("BA LIKE A") << "BA" << "A" << false;
  QTest::newRow("ABC LIKE A") << "ABC" << "A" << false;
  QTest::newRow("ABC LIKE B") << "ABC" << "B" << false;
  QTest::newRow("A LIKE ?A") << "A" << "?A" << false;
  QTest::newRow("AB LIKE ?A") << "AB" << "?A" << false;
  QTest::newRow("BA LIKE ?A") << "BA" << "?A" << true;
  QTest::newRow("A LIKE A?") << "A" << "A?" << false;
  QTest::newRow("AB LIKE A?") << "AB" << "A?" << true;
  QTest::newRow("ABC LIKE A?") << "ABC" << "A?" << false;
  QTest::newRow("A LIKE *A") << "A" << "*A" << true;
  QTest::newRow("AB LIKE *A") << "AB" << "*A" << false;
  QTest::newRow("BA LIKE *A") << "BA" << "*A" << true;
  QTest::newRow("ABC LIKE *A") << "ABC" << "*A" << false;
  QTest::newRow("A LIKE A*") << "A" << "A*" << true;
  QTest::newRow("AB LIKE A*") << "AB" << "A*" << true;
  QTest::newRow("BA LIKE A*") << "BA" << "A*" << false;
  QTest::newRow("ABC LIKE A*") << "ABC" << "A*" << true;
  QTest::newRow("A LIKE *A*") << "A" << "*A*" << true;
  QTest::newRow("BA LIKE *A*") << "BA" << "*A*" << true;
  QTest::newRow("AB LIKE *A*") << "AB" << "*A*" << true;
  QTest::newRow("ABC LIKE *A*") << "ABC" << "*A*" << true;
  QTest::newRow("B LIKE *A*") << "B" << "*A*" << false;
  // Check matching ? and *
  QTest::newRow("A LIKE ?") << "A" << "?" << true;
  QTest::newRow("? LIKE ?") << "?" << "?" << true;
  QTest::newRow("?? LIKE ?") << "??" << "?" << false;
  QTest::newRow("A LIKE \\?") << "A" << "\\?" << false;
  QTest::newRow("? LIKE \\?") << "?" << "\\?" << true;
  QTest::newRow("A LIKE \\*") << "A" << "\\*" << false;
  QTest::newRow("* LIKE \\*") << "*" << "\\*" << true;
  // Check matching % and _
  QTest::newRow("A LIKE %") << "A" << "%" << false;
  QTest::newRow("% LIKE %") << "%" << "%" << true;
  QTest::newRow("A LIKE _") << "A" << "_" << false;
  QTest::newRow("_ LIKE _") << "_" << "_" << true;
  // Check matching regexp keywords
  QTest::newRow("A LIKE {") << "A" << "{" << false;
  QTest::newRow("{ LIKE {") << "{" << "{" << true;
  QTest::newRow("d LIKE d") << "d" << "d" << true;
  QTest::newRow("\\d LIKE d") << "\\d" << "d" << false;
  QTest::newRow("d LIKE \\d") << "d" << "\\d" << false;
  QTest::newRow("\\d LIKE \\d") << "\\d" << "\\d" << true;
  QTest::newRow("A LIKE Q") << "A" << "Q" << false;
  QTest::newRow("Q LIKE Q") << "Q" << "Q" << true;
  QTest::newRow("A LIKE \\Q") << "A" << "\\Q" << false;
  QTest::newRow("Q LIKE \\Q") << "Q" << "\\Q" << false;
  QTest::newRow("\\Q LIKE \\Q") << "\\Q" << "\\Q" << true;
  QTest::newRow("A LIKE E") << "A" << "E" << false;
  QTest::newRow("E LIKE E") << "E" << "E" << true;
  QTest::newRow("A LIKE \\E") << "A" << "\\E" << false;
  QTest::newRow("E LIKE \\E") << "E" << "\\E" << false;
  QTest::newRow("\\E LIKE \\E") << "\\E" << "\\E" << true;
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
