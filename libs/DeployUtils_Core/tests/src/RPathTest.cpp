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
#include "RPathTest.h"
#include "Mdt/DeployUtils/RPathInfo.h"
#include "Mdt/DeployUtils/RPathInfoList.h"

using namespace Mdt::DeployUtils;

void RPathTest::initTestCase()
{
}

void RPathTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RPathTest::rpathInfoTest()
{
  RPathInfo rpi1("../lib", true);
  QCOMPARE(rpi1.path(), QString("../lib"));
  QVERIFY(rpi1.isRelative());
  QVERIFY(!rpi1.isEmpty());
  QVERIFY(!rpi1.isOrigin());

  RPathInfo rpi2("/usr/lib", false);
  QCOMPARE(rpi2.path(), QString("/usr/lib"));
  QVERIFY(!rpi2.isRelative());
  QVERIFY(!rpi2.isEmpty());
  QVERIFY(!rpi2.isOrigin());

  RPathInfo rpi3("", false);
  QVERIFY(rpi3.path().isEmpty());
  QVERIFY(!rpi3.isRelative());
  QVERIFY(rpi3.isEmpty());
  QVERIFY(!rpi3.isOrigin());

  RPathInfo rpi4("", true);
  QVERIFY(rpi4.path().isEmpty());
  QVERIFY(rpi4.isRelative());
  QVERIFY(!rpi4.isEmpty());
  QVERIFY(rpi4.isOrigin());
}

void RPathTest::rpathInfoToStringLinuxTest()
{
  RPathInfo rpi1("../lib", true);
  QCOMPARE(rpi1.toStringLinux(), QString("$ORIGIN/../lib"));

  RPathInfo rpi2("", true);
  QCOMPARE(rpi2.toStringLinux(), QString("$ORIGIN"));
}

void RPathTest::rpathInfoListTest()
{
  /*
   * Initial state
   */
  RPathInfoList rpath;
  QVERIFY(rpath.isEmpty());
  QCOMPARE(rpath.count(), 0);
  QVERIFY(rpath.toStringLinux().isEmpty());
  /*
   * Set a relative path
   */
  rpath.setRelativePath("lib");
  QCOMPARE(rpath.count(), 1);
  QVERIFY(!rpath.isEmpty());
  QCOMPARE(rpath.at(0).path(), QString("lib"));
  QVERIFY(rpath.at(0).isRelative());
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN/lib"));
  /*
   * Prepend a relative path
   */
  rpath.prependRelativePath("bin");
  QCOMPARE(rpath.count(), 2);
  QCOMPARE(rpath.at(0).path(), QString("bin"));
  QVERIFY(rpath.at(0).isRelative());
  QCOMPARE(rpath.at(1).path(), QString("lib"));
  QVERIFY(rpath.at(1).isRelative());
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN/bin:$ORIGIN/lib"));
  // Check that prepending the same path again does not add it
  rpath.prependRelativePath("lib");
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN/bin:$ORIGIN/lib"));
  rpath.prependRelativePath("lib/");
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN/bin:$ORIGIN/lib"));
  /*
   * Set a relative path
   */
  rpath.setRelativePath("lib");
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN/lib"));
  /*
   * Clear
   */
  rpath.clear();
  QVERIFY(rpath.isEmpty());
  QVERIFY(rpath.toStringLinux().isEmpty());
  /*
   * Append a relative path
   */
  rpath.appendPath("lib");
  QCOMPARE(rpath.count(), 1);
  QVERIFY(rpath.at(0).isRelative());
  QCOMPARE(rpath.at(0).path(), QString("lib"));
  // Check that appending the same path again does not add it
  rpath.appendPath("lib");
  QCOMPARE(rpath.count(), 1);
  rpath.appendPath("lib/");
  QCOMPARE(rpath.count(), 1);
  /*
   * Append a absolute path
   */
  rpath.appendPath("/usr/lib");
  QCOMPARE(rpath.count(), 2);
  QVERIFY(rpath.at(0).isRelative());
  QCOMPARE(rpath.at(0).path(), QString("lib"));
  QVERIFY(!rpath.at(1).isRelative());
  QCOMPARE(rpath.at(1).path(), QString("/usr/lib"));
  /*
   * Prepend a relative path
   */
  rpath.prpendPath("bin");
  QCOMPARE(rpath.count(), 3);
  QVERIFY(rpath.at(0).isRelative());
  QCOMPARE(rpath.at(0).path(), QString("bin"));
  QVERIFY(rpath.at(1).isRelative());
  QCOMPARE(rpath.at(1).path(), QString("lib"));
  QVERIFY(!rpath.at(2).isRelative());
  QCOMPARE(rpath.at(2).path(), QString("/usr/lib"));
  /*
   * Prepend a absolute path
   */
  rpath.prpendPath("/usr/bin");
  QCOMPARE(rpath.count(), 4);
  QVERIFY(!rpath.at(0).isRelative());
  QCOMPARE(rpath.at(0).path(), QString("/usr/bin"));
  QVERIFY(rpath.at(1).isRelative());
  QCOMPARE(rpath.at(1).path(), QString("bin"));
  QVERIFY(rpath.at(2).isRelative());
  QCOMPARE(rpath.at(2).path(), QString("lib"));
  QVERIFY(!rpath.at(3).isRelative());
  QCOMPARE(rpath.at(3).path(), QString("/usr/lib"));
  /*
   * Clear
   */
  rpath.clear();
  QVERIFY(rpath.isEmpty());
  QVERIFY(rpath.toStringLinux().isEmpty());
  /*
   * Append origin
   */
  rpath.appendPath(".");
  QCOMPARE(rpath.count(), 1);
  QVERIFY(rpath.at(0).isRelative());
  QVERIFY(rpath.at(0).isOrigin());
  QVERIFY(rpath.at(0).path().isEmpty());
  QCOMPARE(rpath.toStringLinux(), QString("$ORIGIN"));
}

void RPathTest::rpathInfoListFromRawRPathTest()
{
  RPathInfoList rpi;

  rpi = RPathInfoList::fromRawRPath("");
  QVERIFY(rpi.isEmpty());

  rpi = RPathInfoList::fromRawRPath("/usr/lib");
  QCOMPARE(rpi.count(), 1);
  QVERIFY(!rpi.at(0).isRelative());
  QCOMPARE(rpi.at(0).path(), QString("/usr/lib"));

  rpi = RPathInfoList::fromRawRPath("/usr/lib\n");
  QCOMPARE(rpi.count(), 1);
  QVERIFY(!rpi.at(0).isRelative());
  QCOMPARE(rpi.at(0).path(), QString("/usr/lib"));

  rpi = RPathInfoList::fromRawRPath("/usr/lib:\n");
  QCOMPARE(rpi.count(), 1);
  QVERIFY(!rpi.at(0).isRelative());
  QCOMPARE(rpi.at(0).path(), QString("/usr/lib"));

  rpi = RPathInfoList::fromRawRPath("/usr/lib:$ORIGIN");
  QCOMPARE(rpi.count(), 2);
  QVERIFY(!rpi.at(0).isRelative());
  QCOMPARE(rpi.at(0).path(), QString("/usr/lib"));
  QVERIFY(rpi.at(1).isRelative());
  QVERIFY(rpi.at(1).isOrigin());
  QVERIFY(rpi.at(1).path().isEmpty());

  rpi = RPathInfoList::fromRawRPath("$ORIGIN/lib");
  QCOMPARE(rpi.count(), 1);
  QVERIFY(rpi.at(0).isRelative());
  QCOMPARE(rpi.at(0).path(), QString("lib"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RPathTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
