/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "FakeRootTest.h"
#include "Mdt/TestLib/FakeRoot.h"
#include <QString>
#include <QStringList>
#include <QTemporaryDir>

#include <QDebug>

using namespace Mdt::TestLib;

void FakeRootTest::initTestCase()
{
}

void FakeRootTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FakeRootTest::pathWithFakeRootTest()
{
  QFETCH(QString, path);
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QCOMPARE( pathWithFakeRoot(root, path), QDir::cleanPath( root.path() + "/" + path ) );
}

void FakeRootTest::pathWithFakeRootTest_data()
{
  QTest::addColumn<QString>("path");

  QTest::newRow("usr") << "usr";
  QTest::newRow("/usr") << "/usr";
}

void FakeRootTest::prependFakeRootToPathTest()
{
  QFETCH(QString, path);
  QTemporaryDir root;
  QVERIFY(root.isValid());
  QString fullPath = path;
  prependFakeRootToPath(root, fullPath);
  QCOMPARE(fullPath, pathWithFakeRoot(root, path));
}

void FakeRootTest::prependFakeRootToPathTest_data()
{
  QTest::addColumn<QString>("path");

  QTest::newRow("usr") << "usr";
  QTest::newRow("/usr") << "/usr";
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FakeRootTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
