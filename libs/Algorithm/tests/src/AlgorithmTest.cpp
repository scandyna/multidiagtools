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
#include "AlgorithmTest.h"
#include "Mdt/Algorithm.h"
#include "Mdt/Application.h"

#include <QDebug>

namespace Algorithm = Mdt::Algorithm;

void AlgorithmTest::initTestCase()
{
}

void AlgorithmTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void AlgorithmTest::removeFirstLastCharIfTest()
{
  using namespace Mdt::Algorithm;

  QCOMPARE(removeFirstLastCharIf("",'A'), QString(""));
  QCOMPARE(removeFirstLastCharIf("A",'A'), QString(""));
  QCOMPARE(removeFirstLastCharIf("AA",'A'), QString(""));
  QCOMPARE(removeFirstLastCharIf("AB",'A'), QString("B"));
  QCOMPARE(removeFirstLastCharIf("ABA",'A'), QString("B"));
  QCOMPARE(removeFirstLastCharIf("AABAA",'A'), QString("ABA"));
  QCOMPARE(removeFirstLastCharIf("B",'A'), QString("B"));
}

void AlgorithmTest::indexOfFirstEscapedTokenTest()
{
  using Algorithm::indexOfFirstEscapedToken;

  QFETCH(QString, str);
  QFETCH(int, from);
  QFETCH(int, expectedIndex);

  // Data driven part
  QCOMPARE( indexOfFirstEscapedToken(str, from, {'?','*'}, '\\') , expectedIndex );
  /*
   * Make shure that tokens and escape are not hardcoded
   */
  QCOMPARE( indexOfFirstEscapedToken("A", 0, {'_','%'}, '!') , -1 );
  QCOMPARE( indexOfFirstEscapedToken("_", 0, {'_','%'}, '!') , -1 );
  QCOMPARE( indexOfFirstEscapedToken("!_", 0, {'_','%'}, '!') , 0 );
}

void AlgorithmTest::indexOfFirstEscapedTokenTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<int>("from");
  QTest::addColumn<int>("expectedIndex");

  // Strings that never matches
  QTest::newRow("A|0") << "A" << 0 << -1;
  QTest::newRow("AB|0") << "AB" << 0 << -1;
  QTest::newRow("AB|1") << "AB" << 1 << -1;
  QTest::newRow("ABC|0") << "ABC" << 0 << -1;
  QTest::newRow("ABC|1") << "ABC" << 1 << -1;
  QTest::newRow("ABC|2") << "ABC" << 2 << -1;
  // Check with single non escaped tokens in str
  QTest::newRow("?|0") << "?" << 0 << -1;
  QTest::newRow("*|0") << "*" << 0 << -1;
  // Check with single escaped tokens in str
  QTest::newRow("\\?|0") << "\\?" << 0 << 0;
  QTest::newRow("\\*|0") << "\\*" << 0 << 0;
  // Check alternation of escaped and non escaped
  QTest::newRow("?|0") << "?" << 0 << -1;
  QTest::newRow("?\\?|0") << "?\\?" << 0 << 1;
  QTest::newRow("?\\?|1") << "?\\?" << 1 << 1;
  QTest::newRow("?\\?|2") << "?\\?" << 2 << -1;
  QTest::newRow("?\\??|0") << "?\\??" << 0 << 1;
  QTest::newRow("?\\??|1") << "?\\??" << 1 << 1;
  QTest::newRow("?\\??|2") << "?\\??" << 2 << -1;
  QTest::newRow("?\\??|3") << "?\\??" << 3 << -1;
  // Mix non matching and matching in str
  QTest::newRow("A?|0") << "A?" << 0 << -1;
  QTest::newRow("A?|1") << "A?" << 1 << -1;
  QTest::newRow("A?B?|0") << "A?B?" << 0 << -1;
  QTest::newRow("A?B?|1") << "A?B?" << 1 << -1;
  QTest::newRow("A?B?|2") << "A?B?" << 2 << -1;
  QTest::newRow("A?B?|3") << "A?B?" << 3 << -1;
  QTest::newRow("A?B\\?|0") << "A?B\\?" << 0 << 3;
  QTest::newRow("A?B\\?|1") << "A?B\\?" << 1 << 3;
  QTest::newRow("A?B\\?|2") << "A?B\\?" << 2 << 3;
  QTest::newRow("A?B\\?|3") << "A?B\\?" << 3 << 3;
  QTest::newRow("A?B\\?|4") << "A?B\\?" << 4 << -1;
}

void AlgorithmTest::indexOfFirstEscapedTokenBenchmark()
{
  using Algorithm::indexOfFirstEscapedToken;

  int index;
  QString str = "ABC?D*EF%?123\\?";
  QBENCHMARK{
    index = indexOfFirstEscapedToken(str, 0, {'*','%','!','$','?'}, '\\');
  }
  QCOMPARE(index, str.size()-2);
}

void AlgorithmTest::unescapeEscapedTokensTest()
{
  using Algorithm::unescapeEscapedTokens;

  QFETCH(QString, str);
  QFETCH(QString, expectedStr);

  // Data driven part
  QCOMPARE( unescapeEscapedTokens(str, {'?','*'} , '\\') , expectedStr );
  /*
   * Make shure that tokens and escape are not hardcoded
   */
  QCOMPARE( unescapeEscapedTokens("A$B!$", {'$','&'} , '!') , QString("A$B$") );
}

void AlgorithmTest::unescapeEscapedTokensTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("expectedStr");

  // Strings with no token
  QTest::newRow("") << "" << "";
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("AB") << "AB" << "AB";
  // Check with single non escaped tokens in str
  QTest::newRow("?") << "?" << "?";
  QTest::newRow("*") << "*" << "*";
  // Check with single escaped tokens in str
  QTest::newRow("\\?") << "\\?" << "?";
  QTest::newRow("\\*") << "\\*" << "*";
  // Check alternation of escaped and non escaped
  QTest::newRow("?\\?") << "?\\?" << "??";
  QTest::newRow("?\\??") << "?\\??" << "???";
  QTest::newRow("*\\*") << "*\\*" << "**";
  QTest::newRow("*\\**") << "*\\**" << "***";
  // Mix non tokens and tokens in str
  QTest::newRow("A?") << "A?" << "A?";
  QTest::newRow("A\\?") << "A\\?" << "A?";
  QTest::newRow("A?B?") << "A?B?" << "A?B?";
  QTest::newRow("A\\?B\\?") << "A\\?B\\?" << "A?B?";
  QTest::newRow("A*B*") << "A*B*" << "A*B*";
  QTest::newRow("A\\*B\\*") << "A\\*B\\*" << "A*B*";
  // Check with non token escaped
  QTest::newRow("\\!") << "\\!" << "\\!";
  QTest::newRow("\\!\\?") << "\\!\\?" << "\\!?";
  QTest::newRow("\\!A\\?") << "\\!A\\?" << "\\!A?";
}

void AlgorithmTest::unescapeEscapedTokensBenchmark()
{
  using Algorithm::unescapeEscapedTokens;

  QString str = "ABC?DEF?123\\?";
  QString result;
  QBENCHMARK{
    result = unescapeEscapedTokens(str, {'!','&','|','*','?'}, '\\');
  }
  QCOMPARE( result , QString("ABC?DEF?123?") );
}

void AlgorithmTest::indexOfFirstNonEscapedTokenTest()
{
  using Algorithm::indexOfFirstNonEscapedToken;

  QFETCH(QString, str);
  QFETCH(int, from);
  QFETCH(int, expectedIndex);

  // Data driven part
  QCOMPARE( indexOfFirstNonEscapedToken(str, from, {'?','*'}, '\\') , expectedIndex );
  /*
   * Make shure that tokens and escape are not hardcoded
   */
  QCOMPARE( indexOfFirstNonEscapedToken("A", 0, {'_','%'}, '!') , -1 );
  QCOMPARE( indexOfFirstNonEscapedToken("_", 0, {'_','%'}, '!') , 0 );
  QCOMPARE( indexOfFirstNonEscapedToken("!_", 0, {'_','%'}, '!') , -1 );
  QCOMPARE( indexOfFirstNonEscapedToken("%", 0, {'_','%'}, '!') , 0 );
  QCOMPARE( indexOfFirstNonEscapedToken("!%", 0, {'_','%'}, '!') , -1 );
  /// \todo Check matching escape !
  QCOMPARE( indexOfFirstNonEscapedToken("A", 0, {'\\'}, '\\') , -1 );
  QCOMPARE( indexOfFirstNonEscapedToken("\\", 0, {'\\'}, '\\') , 0 );
  ///QCOMPARE( indexOfFirstNonEscapedToken("\\\\", 0, {'\\'}, '\\') , -1 );
}

void AlgorithmTest::indexOfFirstNonEscapedTokenTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<int>("from");
  QTest::addColumn<int>("expectedIndex");

  // Strings that never matches
  QTest::newRow("A|0") << "A" << 0 << -1;
  QTest::newRow("AB|0") << "AB" << 0 << -1;
  QTest::newRow("AB|1") << "AB" << 1 << -1;
  QTest::newRow("ABC|0") << "ABC" << 0 << -1;
  QTest::newRow("ABC|1") << "ABC" << 1 << -1;
  QTest::newRow("ABC|2") << "ABC" << 2 << -1;
  // Check with single non escaped tokens in str
  QTest::newRow("?|0") << "?" << 0 << 0;
  QTest::newRow("*|0") << "*" << 0 << 0;
  // Check with single escaped tokens in str
  QTest::newRow("\\?|0") << "\\?" << 0 << -1;
  QTest::newRow("\\*|0") << "\\*" << 0 << -1;
  // Check alternation of escaped and non escaped
  QTest::newRow("?|0") << "?" << 0 << 0;
  QTest::newRow("?\\?|0") << "?\\?" << 0 << 0;
  QTest::newRow("?\\?|1") << "?\\?" << 1 << -1;
  QTest::newRow("?\\?|2") << "?\\?" << 2 << -1;
  QTest::newRow("?\\??|0") << "?\\??" << 0 << 0;
  QTest::newRow("?\\??|1") << "?\\??" << 1 << 3;
  QTest::newRow("?\\??|2") << "?\\??" << 2 << 3;
  QTest::newRow("?\\??|3") << "?\\??" << 3 << 3;
  // Mix non matching and matching in str
  QTest::newRow("A?|0") << "A?" << 0 << 1;
  QTest::newRow("A?|1") << "A?" << 1 << 1;
  QTest::newRow("A?B?|0") << "A?B?" << 0 << 1;
  QTest::newRow("A?B?|1") << "A?B?" << 1 << 1;
  QTest::newRow("A?B?|2") << "A?B?" << 2 << 3;
  QTest::newRow("A?B?|3") << "A?B?" << 3 << 3;
  QTest::newRow("A?B\\?|0") << "A?B\\?" << 0 << 1;
  QTest::newRow("A?B\\?|1") << "A?B\\?" << 1 << 1;
  QTest::newRow("A?B\\?|2") << "A?B\\?" << 2 << -1;
  QTest::newRow("A?B\\?|3") << "A?B\\?" << 3 << -1;
  QTest::newRow("A?B\\?|4") << "A?B\\?" << 4 << -1;
}

void AlgorithmTest::indexOfFirstNonEscapedTokenBenchmark()
{
  using Algorithm::indexOfFirstNonEscapedToken;

  int index;
  QString str = "ABC\\?DEF\\?123?";
  QBENCHMARK{
    index = indexOfFirstNonEscapedToken(str, 0, {'*','%','!','$','?'}, '\\');
  }
  QCOMPARE(index, str.size()-1);
}

void AlgorithmTest::replaceNonEscapedTokensTest()
{
  using Algorithm::replaceNonEscapedTokens;

  QFETCH(QString, str);
  QFETCH(QString, expectedStr);

  // Data driven part
  QCOMPARE( replaceNonEscapedTokens(str, {{'?',"."},{'*',".*"}} , '\\') , expectedStr );
  /*
   * Make shure that tokens and escape are not hardcoded
   */
  QCOMPARE( replaceNonEscapedTokens("A$B!$", {{'$',"&"}} , '!') , QString("A&B!$") );
}

void AlgorithmTest::replaceNonEscapedTokensTest_data()
{
  QTest::addColumn<QString>("str");
  QTest::addColumn<QString>("expectedStr");

  // Strings with no token
  QTest::newRow("") << "" << "";
  QTest::newRow("A") << "A" << "A";
  QTest::newRow("AB") << "AB" << "AB";
  // Check with single non escaped tokens in str
  QTest::newRow("?") << "?" << ".";
  QTest::newRow("*") << "*" << ".*";
  // Check with single escaped tokens in str
  QTest::newRow("\\?") << "\\?" << "\\?";
  QTest::newRow("\\*") << "\\*" << "\\*";
  // Check alternation of escaped and non escaped
  QTest::newRow("?\\?") << "?\\?" << ".\\?";
  QTest::newRow("?\\??") << "?\\??" << ".\\?.";
  QTest::newRow("*\\*") << "*\\*" << ".*\\*";
  QTest::newRow("*\\**") << "*\\**" << ".*\\*.*";
  // Mix non tokens and tokens in str
  QTest::newRow("A?") << "A?" << "A.";
  QTest::newRow("A?B?") << "A?B?" << "A.B.";
  QTest::newRow("A*B*") << "A*B*" << "A.*B.*";
}

void AlgorithmTest::replaceNonEscapedTokensBenchmark()
{
  using Algorithm::replaceNonEscapedTokens;

  QString str = "ABC\\?DEF\\?123?";
  QString result;
  QBENCHMARK{
    result = replaceNonEscapedTokens(str, {{'!',"$"},{'&',"|"},{'*',".*"},{'?',"."}}, '\\');
  }
  QCOMPARE( result , QString("ABC\\?DEF\\?123.") );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  AlgorithmTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
