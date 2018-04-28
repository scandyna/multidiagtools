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

  void unescapeEscapedTokensTest();
  void unescapeEscapedTokensTest_data();

  void indexOfFirstNonEscapedTokenTest();
  void indexOfFirstNonEscapedTokenTest_data();

  void replaceNonEscapedTokensTest();
  void replaceNonEscapedTokensTest_data();

  void moveIfTest();
  void moveIfTest_data();

  void countStringTest();
  void countStringTest_data();

  void hasDuplicatesTest();
  void hasDuplicatesTest_data();

  void forEachAdjacentPairIntTest();
  void forEachAdjacentPairIntTest_data();
  void forEachAdjacentPairStringTest();
  void forEachAdjacentPairStringTest_data();

 private:

  static QStringList generateStringList(int size);
};

#endif // #ifndef MDT_ALGORITHM_TEST_H
