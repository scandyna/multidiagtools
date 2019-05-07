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
#include "AlgorithmTest.h"
#include "Mdt/Algorithm.h"
#include "Mdt/CoreApplication.h"
#include <QString>
#include <QStringList>
#include <QLatin1String>
#include <QChar>
#include <QVector>
#include <QList>
#include <vector>
#include <iterator>
#include <cmath>

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

void AlgorithmTest::moveIfTest()
{
  QFETCH(QStringList, initialSourceList);
  QFETCH(QString, matchValue);
  QFETCH(QStringList, expectedSourceList);
  QFETCH(QStringList, expectedDestinationList);

  QStringList sourceList = initialSourceList;
  QStringList destinationList;

  // Process
  const auto predicate = [matchValue](const QString & value){
    return value == matchValue;
  };
  Mdt::Algorithm::moveIf(sourceList, destinationList, predicate);
  // Check
  QCOMPARE(sourceList, expectedSourceList);
  QCOMPARE(destinationList, expectedDestinationList);
}

void AlgorithmTest::moveIfTest_data()
{
  QTest::addColumn<QStringList>("initialSourceList");
  QTest::addColumn<QString>("matchValue");
  QTest::addColumn<QStringList>("expectedSourceList");
  QTest::addColumn<QStringList>("expectedDestinationList");

  QTest::newRow("") << QStringList{} << ""
                    << QStringList{} << QStringList{};

  QTest::newRow("A|") << QStringList{"A"} << ""
                      << QStringList{"A"} << QStringList{};

  QTest::newRow("A|A") << QStringList{"A"} << "A"
                       << QStringList{} << QStringList{"A"};

  QTest::newRow("A,B|A") << QStringList{"A","B"} << "A"
                         << QStringList{"B"} << QStringList{"A"};


}

void AlgorithmTest::countStringTest()
{
  QFETCH(QStringList, stringList);
  QFETCH(QString, str);
  QFETCH(Qt::CaseSensitivity, caseSensitivity);
  QFETCH(int, expectedCount);

  QCOMPARE(Algorithm::count(stringList, str, caseSensitivity), expectedCount);
}

void AlgorithmTest::countStringTest_data()
{
  QTest::addColumn<QStringList>("stringList");
  QTest::addColumn<QString>("str");
  QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");
  QTest::addColumn<int>("expectedCount");

  QTest::newRow("{}|A|cs") << QStringList{}
                           << "A" << Qt::CaseSensitive << 0;

  QTest::newRow("{A}|A|cs") << QStringList{"A"}
                            << "A" << Qt::CaseSensitive << 1;

  QTest::newRow("{A}|a|cs") << QStringList{"A"}
                            << "a" << Qt::CaseSensitive << 0;

  QTest::newRow("{A}|a|ci") << QStringList{"A"}
                            << "a" << Qt::CaseInsensitive << 1;

  QTest::newRow("{a,b,c,A,B,C}|b|cs") << QStringList{"a","b","c","A","B","C"}
                                      << "b" << Qt::CaseSensitive << 1;

  QTest::newRow("{a,b,c,A,B,C}|b|ci") << QStringList{"a","b","c","A","B","C"}
                                      << "b" << Qt::CaseInsensitive << 2;

}

void AlgorithmTest::hasDuplicatesTest()
{
  QFETCH(QStringList, stringList);
  QFETCH(Qt::CaseSensitivity, caseSensitivity);
  QFETCH(bool, expectedHasDuplicates);

  QCOMPARE(Algorithm::hasDuplicates(stringList, caseSensitivity), expectedHasDuplicates);
}

void AlgorithmTest::hasDuplicatesTest_data()
{
  QTest::addColumn<QStringList>("stringList");
  QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");
  QTest::addColumn<bool>("expectedHasDuplicates");

  const bool hasDuplicates = true;
  const bool noDuplicates = false;

  QTest::newRow("") << QStringList{}
                    << Qt::CaseSensitive << noDuplicates;

  QTest::newRow("A") << QStringList{"A"}
                    << Qt::CaseSensitive << noDuplicates;

  QTest::newRow("Aa|cs") << QStringList{"A","a"}
                         << Qt::CaseSensitive << noDuplicates;

  QTest::newRow("Aa|ci") << QStringList{"A","a"}
                         << Qt::CaseInsensitive << hasDuplicates;


  QTest::newRow("aBA|cs") << QStringList{"a","B","A"}
                          << Qt::CaseSensitive << noDuplicates;

  QTest::newRow("aBA|ci") << QStringList{"a","B","A"}
                          << Qt::CaseInsensitive<< hasDuplicates;

}

void AlgorithmTest::forEachAdjacentPairIntTest()
{
  using namespace Mdt::Algorithm;

  QFETCH(QVector<int>, inVector);
  QFETCH(QList<int>, expectedResult);

  QList<int> result;

  const auto f = [&result](int a, int b){
    result.append(a+b);
  };

  forEachAdjacentPair(inVector.cbegin(), inVector.cend(), f);
  QCOMPARE(result, expectedResult);
}

void AlgorithmTest::forEachAdjacentPairIntTest_data()
{
  QTest::addColumn< QVector<int> >("inVector");
  QTest::addColumn< QList<int> >("expectedResult");

  QTest::newRow("") << QVector<int>{} << QList<int>{};
  QTest::newRow("1") << QVector<int>{1} << QList<int>{};
  QTest::newRow("1,2") << QVector<int>{1,2} << QList<int>{3};
  QTest::newRow("1,2,3") << QVector<int>{1,2,3} << QList<int>{3,5};
}

void AlgorithmTest::forEachAdjacentPairStringTest()
{
  using namespace Mdt::Algorithm;

  QFETCH(QStringList, stringList);
  QFETCH(QStringList, expectedResult);

  QStringList result;

  const auto f = [&result](const QString & a, const QString & b){
    result.append(a+b);
  };

  forEachAdjacentPair(stringList.cbegin(), stringList.cend(), f);
  QCOMPARE(result, expectedResult);
}

void AlgorithmTest::forEachAdjacentPairStringTest_data()
{
  QTest::addColumn<QStringList>("stringList");
  QTest::addColumn<QStringList>("expectedResult");

  QTest::newRow("") << QStringList{} << QStringList{};
  QTest::newRow("A") << QStringList{"A"} << QStringList{};
  QTest::newRow("A,B") << QStringList{"A","B"} << QStringList{"AB"};
  QTest::newRow("A,B,C") << QStringList{"A","B","C"} << QStringList{"AB","BC"};
}

void AlgorithmTest::findLastConsecutiveIntegralValueTest()
{
  using namespace Mdt::Algorithm;

  QFETCH(QVector<int>, inVector);
  QFETCH(int, expectedIndex);

  const auto it = findLastConsecutiveIntegralValue(inVector.cbegin(), inVector.cend());
  QVERIFY( it == std::next(inVector.cbegin(), expectedIndex) );
}

void AlgorithmTest::findLastConsecutiveIntegralValueTest_data()
{
  QTest::addColumn< QVector<int> >("inVector");
  QTest::addColumn<int>("expectedIndex");

  QTest::newRow("") << QVector<int>{} << 0;
  QTest::newRow("1") << QVector<int>{1} << 0;
  QTest::newRow("1,2") << QVector<int>{1,2} << 1;
  QTest::newRow("1,3") << QVector<int>{1,3} << 0;
  QTest::newRow("1,2,3") << QVector<int>{1,2,3} << 2;
  QTest::newRow("1,2,4") << QVector<int>{1,2,4} << 1;
  QTest::newRow("1,3,5") << QVector<int>{1,3,5} << 0;
  QTest::newRow("1,3,4,5") << QVector<int>{1,3,4,5} << 0;
  QTest::newRow("1,2,4,5,7,8") << QVector<int>{1,2,4,5,7,8} << 1;
}


QStringList AlgorithmTest::generateStringList(int size)
{
  QStringList list;

  list.reserve(size);
  for(int i = 0; i < size; ++i){
    list << QString( QChar('A' + i) );
  }

  return list;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AlgorithmTest test;

//   app.debugEnvironment();

  return QTest::qExec(&test, argc, argv);
}
