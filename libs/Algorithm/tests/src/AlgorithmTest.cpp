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
