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
#include "LibraryTest.h"
#include "Mdt/DeployUtils/LibraryVersion.h"
#include "Mdt/DeployUtils/LibraryName.h"
#include "Mdt/DeployUtils/Library.h"

using namespace Mdt::DeployUtils;

void LibraryTest::initTestCase()
{
}

void LibraryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LibraryTest::libraryVersionTest()
{
  /*
   * Default constructed
   */
  LibraryVersion v0;
  QCOMPARE(v0.versionMajor(), (qint8)-1);
  QCOMPARE(v0.versionMinor(), (qint8)-1);
  QCOMPARE(v0.versionPatch(), (qint8)-1);
  QVERIFY(v0.isNull());
  QVERIFY(v0.toString().isEmpty());
  /*
   * Some non null versions
   */
  LibraryVersion v1(1);
  QCOMPARE(v1.versionMajor(), (qint8)1);
  QCOMPARE(v1.versionMinor(), (qint8)-1);
  QCOMPARE(v1.versionPatch(), (qint8)-1);
  QVERIFY(!v1.isNull());
  QCOMPARE(v1.toString(), QString("1"));
  LibraryVersion v12(1,2);
  QCOMPARE(v12.versionMajor(), (qint8)1);
  QCOMPARE(v12.versionMinor(), (qint8)2);
  QCOMPARE(v12.versionPatch(), (qint8)-1);
  QVERIFY(!v12.isNull());
  QCOMPARE(v12.toString(), QString("1.2"));
  LibraryVersion v123(1,2,3);
  QCOMPARE(v123.versionMajor(), (qint8)1);
  QCOMPARE(v123.versionMinor(), (qint8)2);
  QCOMPARE(v123.versionPatch(), (qint8)3);
  QVERIFY(!v123.isNull());
  QCOMPARE(v123.toString(), QString("1.2.3"));
}

void LibraryTest::libraryVersionSetStrTest()
{
  QFETCH(QString, versionString);
  QFETCH(QString, expectedVersionString);
  QFETCH(bool, expectedIsNull);

  LibraryVersion v(versionString);
  QCOMPARE(v.isNull(), expectedIsNull);
  QCOMPARE(v.toString(), expectedVersionString);
}

void LibraryTest::libraryVersionSetStrTest_data()
{
  QTest::addColumn<QString>("versionString");
  QTest::addColumn<QString>("expectedVersionString");
  QTest::addColumn<bool>("expectedIsNull");

  const bool Null = true;
  const bool NotNull = false;

  QTest::newRow("") << "" << "" << Null;
  QTest::newRow("1") << "1" << "1" << NotNull;
  QTest::newRow("1.2") << "1.2" << "1.2" << NotNull;
  QTest::newRow("1.2.3") << "1.2.3" << "1.2.3" << NotNull;
  QTest::newRow("0.0.0") << "0.0.0" << "0.0.0" << NotNull;
  QTest::newRow("a") << "a" << "" << Null;
  QTest::newRow("a.b") << "a.b" << "" << Null;
  QTest::newRow("a.b.c") << "a.b.c" << "" << Null;
}

void LibraryTest::libraryVersionBenchmark()
{
  QFETCH(QString, versionString);
  QFETCH(QString, expectedVersionString);

  QString outVersionString;
  QBENCHMARK{
    LibraryVersion v(versionString);
    outVersionString = v.toString();
  }
  QCOMPARE(outVersionString, expectedVersionString);
}

void LibraryTest::libraryVersionBenchmark_data()
{
  QTest::addColumn<QString>("versionString");
  QTest::addColumn<QString>("expectedVersionString");

  QTest::newRow("1") << "1" << "1";
  QTest::newRow("1.2") << "1.2" << "1.2";
  QTest::newRow("1.2.3") << "1.2.3" << "1.2.3";
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LibraryTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
