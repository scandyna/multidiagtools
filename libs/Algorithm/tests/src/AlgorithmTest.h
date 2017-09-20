/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ALGORITHM_TEST_H
#define MDT_ALGORITHM_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <QStringList>

class AlgorithmTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void removeFirstLastCharIfTest();

  void indexOfFirstEscapedTokenTest();
  void indexOfFirstEscapedTokenTest_data();
  void indexOfFirstEscapedTokenBenchmark();

  void unescapeEscapedTokensTest();
  void unescapeEscapedTokensTest_data();
  void unescapeEscapedTokensBenchmark();

  void indexOfFirstNonEscapedTokenTest();
  void indexOfFirstNonEscapedTokenTest_data();
  void indexOfFirstNonEscapedTokenBenchmark();

  void replaceNonEscapedTokensTest();
  void replaceNonEscapedTokensTest_data();
  void replaceNonEscapedTokensBenchmark();

  void moveIfTest();
  void moveIfTest_data();
  void moveIfQStringListBenchmark();
  void moveIfQStringListBenchmark_data();
  void moveIfQVectorQStringBenchmark();
  void moveIfQVectorQStringBenchmark_data();
  void moveIfStdVectorQStringBenchmark();
  void moveIfStdVectorQStringBenchmark_data();

 private:

  void createMoveIfBenchmarkData();
  static void prepareMoveIfBenchmarkData(QStringList & initialSourceList, QString & matchValue,
                                         QStringList & expectedSourceList, QStringList & expectedDestinationList, int n);
  static QStringList generateStringList(int size);
};

#endif // #ifndef MDT_ALGORITHM_TEST_H
