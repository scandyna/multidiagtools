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
#include "BinaryDependenciesTest.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QCoreApplication>

// #include <QDebug>

using namespace Mdt::DeployUtils;

void BinaryDependenciesTest::initTestCase()
{
}

void BinaryDependenciesTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void BinaryDependenciesTest::searchPathListTest()
{

  QFAIL("Not complete");
}

void BinaryDependenciesTest::runTest()
{
  BinaryDependencies deps;

//   QVERIFY(deps.isValid());
  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath() ));
  QVERIFY(!deps.dependencies().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BinaryDependenciesTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
