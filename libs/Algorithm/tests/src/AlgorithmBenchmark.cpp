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
#include "AlgorithmBenchmark.h"
#include "Mdt/Algorithm.h"
#include "Mdt/CoreApplication.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QVector>
#include <vector>
#include <cmath>

// #include <QDebug>

namespace Algorithm = Mdt::Algorithm;

void AlgorithmBenchmark::initTestCase()
{
}

void AlgorithmBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void AlgorithmBenchmark::indexOfFirstEscapedTokenBenchmark()
{
  using Algorithm::indexOfFirstEscapedToken;

  int index;
  QString str = "ABC?D*EF%?123\\?";
  QBENCHMARK{
    index = indexOfFirstEscapedToken(str, 0, {'*','%','!','$','?'}, '\\');
  }
  QCOMPARE(index, str.size()-2);
}

void AlgorithmBenchmark::unescapeEscapedTokensBenchmark()
{
  using Algorithm::unescapeEscapedTokens;

  QString str = "ABC?DEF?123\\?";
  QString result;
  QBENCHMARK{
    result = unescapeEscapedTokens(str, {'!','&','|','*','?'}, '\\');
  }
  QCOMPARE( result , QString("ABC?DEF?123?") );
}

void AlgorithmBenchmark::indexOfFirstNonEscapedTokenBenchmark()
{
  using Algorithm::indexOfFirstNonEscapedToken;

  int index;
  QString str = "ABC\\?DEF\\?123?";
  QBENCHMARK{
    index = indexOfFirstNonEscapedToken(str, 0, {'*','%','!','$','?'}, '\\');
  }
  QCOMPARE(index, str.size()-1);
}

void AlgorithmBenchmark::replaceNonEscapedTokensBenchmark()
{
  using Algorithm::replaceNonEscapedTokens;

  QString str = "ABC\\?DEF\\?123?";
  QString result;
  QBENCHMARK{
    result = replaceNonEscapedTokens(str, {{'!',"$"},{'&',"|"},{'*',".*"},{'?',"."}}, '\\');
  }
  QCOMPARE( result , QString("ABC\\?DEF\\?123.") );
}

void AlgorithmBenchmark::moveIfQStringListBenchmark()
{
  QFETCH(QStringList, initialSourceList);
  QFETCH(QString, matchValue);
  QFETCH(QStringList, expectedSourceList);
  QFETCH(QStringList, expectedDestinationList);

  QStringList sourceList = initialSourceList;
  QStringList destinationList;

  const auto predicate = [matchValue](const QString & value){
    return value == matchValue;
  };
  QBENCHMARK{
    Mdt::Algorithm::moveIf(sourceList, destinationList, predicate);
  }
  // Check
  QCOMPARE(sourceList, expectedSourceList);
  QCOMPARE(destinationList, expectedDestinationList);
}

void AlgorithmBenchmark::moveIfQStringListBenchmark_data()
{
  createMoveIfBenchmarkData();
}

void AlgorithmBenchmark::moveIfQVectorQStringBenchmark()
{
  QFETCH(QStringList, initialSourceList);
  QFETCH(QString, matchValue);
  QFETCH(QStringList, expectedSourceList);
  QFETCH(QStringList, expectedDestinationList);

  auto sourceList = initialSourceList.toVector();
  QVector<QString> destinationList;

  const auto predicate = [matchValue](const QString & value){
    return value == matchValue;
  };
  QBENCHMARK{
    Mdt::Algorithm::moveIf(sourceList, destinationList, predicate);
  }
  // Check
  QCOMPARE(sourceList.size(), expectedSourceList.size());
  for(int i = 0; i < sourceList.size(); ++i){
    QCOMPARE(sourceList.at(i), expectedSourceList.at(i));
  }
  QCOMPARE(destinationList.size(), expectedDestinationList.size());
  for(int i = 0; i < destinationList.size(); ++i){
    QCOMPARE(destinationList.at(i), expectedDestinationList.at(i));
  }
}

void AlgorithmBenchmark::moveIfQVectorQStringBenchmark_data()
{
  createMoveIfBenchmarkData();
}

void AlgorithmBenchmark::moveIfStdVectorQStringBenchmark()
{
  QFETCH(QStringList, initialSourceList);
  QFETCH(QString, matchValue);
  QFETCH(QStringList, expectedSourceList);
  QFETCH(QStringList, expectedDestinationList);

  auto sourceList = initialSourceList.toVector().toStdVector();
  std::vector<QString> destinationList;

  const auto predicate = [matchValue](const QString & value){
    return value == matchValue;
  };
  QBENCHMARK{
    Mdt::Algorithm::moveIf(sourceList, destinationList, predicate);
  }
  // Check
  QCOMPARE((int)sourceList.size(), expectedSourceList.size());
  for(int i = 0; i < (int)sourceList.size(); ++i){
    QCOMPARE(sourceList.at(i), expectedSourceList.at(i));
  }
  QCOMPARE((int)destinationList.size(), expectedDestinationList.size());
  for(int i = 0; i < (int)destinationList.size(); ++i){
    QCOMPARE(destinationList.at(i), expectedDestinationList.at(i));
  }
}

void AlgorithmBenchmark::moveIfStdVectorQStringBenchmark_data()
{
  createMoveIfBenchmarkData();
}

void AlgorithmBenchmark::countStringBenchmark()
{
  QFETCH(QStringList, stringList);
  QFETCH(QString, str);
  QFETCH(Qt::CaseSensitivity, caseSensitivity);
  QFETCH(int, expectedCount);

  int cnt;
  QBENCHMARK{
    cnt = Algorithm::count(stringList, str, caseSensitivity);
  }
  QCOMPARE(cnt, expectedCount);
}

void AlgorithmBenchmark::countStringBenchmark_data()
{
  QTest::addColumn<QStringList>("stringList");
  QTest::addColumn<QString>("str");
  QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");
  QTest::addColumn<int>("expectedCount");

  QTest::newRow("{a,b,c,A,B,C}|C|ci") << QStringList{"a","b","c","A","B","C"}
                                      << "C" << Qt::CaseInsensitive << 2;
}

void AlgorithmBenchmark::hasDuplicatesBenchmark()
{
  QFETCH(QStringList, stringList);
  QFETCH(Qt::CaseSensitivity, caseSensitivity);
  QFETCH(bool, expectedHasDuplicates);

  bool hasDuplicates;
  QBENCHMARK{
    hasDuplicates = Algorithm::hasDuplicates(stringList, caseSensitivity);
  }
  QCOMPARE(hasDuplicates, expectedHasDuplicates);
}

void AlgorithmBenchmark::hasDuplicatesBenchmark_data()
{
  QTest::addColumn<QStringList>("stringList");
  QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");
  QTest::addColumn<bool>("expectedHasDuplicates");

  const bool hasDuplicates = true;
//   const bool noDuplicates = false;

  QTest::newRow("{a,b,c,A,B,C}|ci") << QStringList{"a","b","c","A","B","C"}
                                    << Qt::CaseInsensitive << hasDuplicates;
}

void AlgorithmBenchmark::findLastConsecutiveIntegralValueBenchmark()
{
  using namespace Mdt::Algorithm;

  QFETCH(QVector<int>, inVector);
  QFETCH(int, expectedIndex);

  QVector<int>::const_iterator it;

  QBENCHMARK{
    it = findLastConsecutiveIntegralValue(inVector.cbegin(), inVector.cend());
  }
  QVERIFY( it == std::next(inVector.cbegin(), expectedIndex) );
}

void AlgorithmBenchmark::findLastConsecutiveIntegralValueBenchmark_data()
{
  QTest::addColumn< QVector<int> >("inVector");
  QTest::addColumn<int>("expectedIndex");

  QTest::newRow("1,2") << QVector<int>{1,2} << 1;
  QTest::newRow("1,2,3,4,5,8") << QVector<int>{1,2,3,4,5,8} << 4;
}

/*
 * Helpers
 */

void AlgorithmBenchmark::createMoveIfBenchmarkData()
{
  QTest::addColumn<QStringList>("initialSourceList");
  QTest::addColumn<QString>("matchValue");
  QTest::addColumn<QStringList>("expectedSourceList");
  QTest::addColumn<QStringList>("expectedDestinationList");

  QStringList initialSourceList;
  QString matchValue;
  QStringList expectedSourceList;
  QStringList expectedDestinationList;

  QTest::newRow("") << QStringList{} << ""
                    << QStringList{} << QStringList{};

  prepareMoveIfBenchmarkData(initialSourceList, matchValue, expectedSourceList, expectedDestinationList, 10);
  QTest::newRow("10") << initialSourceList << matchValue << expectedSourceList << expectedDestinationList;

  prepareMoveIfBenchmarkData(initialSourceList, matchValue, expectedSourceList, expectedDestinationList, 100);
  QTest::newRow("100") << initialSourceList << matchValue << expectedSourceList << expectedDestinationList;

  prepareMoveIfBenchmarkData(initialSourceList, matchValue, expectedSourceList, expectedDestinationList, 1000);
  QTest::newRow("1000") << initialSourceList << matchValue << expectedSourceList << expectedDestinationList;
}

void AlgorithmBenchmark::prepareMoveIfBenchmarkData(QStringList& initialSourceList, QString& matchValue, QStringList& expectedSourceList, QStringList& expectedDestinationList, int n)
{
  Q_ASSERT(n > 0);

  initialSourceList = generateStringList(n);
  matchValue = initialSourceList.last();
  expectedSourceList = initialSourceList;
  expectedSourceList.removeAll(matchValue);
  expectedDestinationList = initialSourceList.filter(matchValue);
}

QStringList AlgorithmBenchmark::generateStringList(int size)
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
  AlgorithmBenchmark test;

//   app.debugEnvironment();

  return QTest::qExec(&test, argc, argv);
}
