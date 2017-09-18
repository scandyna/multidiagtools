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
#include "StandardPathsTest.h"
#include "Mdt/StandardPaths.h"
#include <QDir>

#include <QDebug>

using namespace Mdt;

void StandardPathsTest::initTestCase()
{
}

void StandardPathsTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StandardPathsTest::logDirectoryTest()
{
  /*
   * The returned directory probably not exists,
   *  it will be created by Mdt::CoreApplication
   *  We just check that a absolute path is returned
   */
  QVERIFY(!StandardPaths::logDirectoryPath().isEmpty());
  QDir dir(StandardPaths::logDirectoryPath());
  QVERIFY(dir.isAbsolute());
}

void StandardPathsTest::cacheDirectoryTest()
{
  /*
   * The returned directory probably not exists,
   *  it will be created by Mdt::CoreApplication
   *  We just check that a absolute path is returned
   */
  QVERIFY(!StandardPaths::cacheDirectoryPath().isEmpty());
  QDir dir(StandardPaths::cacheDirectoryPath());
  QVERIFY(dir.isAbsolute());
}

// void StandardPathsTest::mdtDataDirectoryTest()
// {
// 
//   qDebug() << StandardPaths::mdtDataDirectoryPath();
//   
//   QFAIL("Not complete");
// }

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StandardPathsTest test;

//   if(!app.init()){
//     return 1;
//   }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
