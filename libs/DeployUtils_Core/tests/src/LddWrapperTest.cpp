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
#include "LddWrapperTest.h"
#include "Mdt/DeployUtils/LddWrapper.h"
#include <QCoreApplication>
#include <QtGlobal>

using namespace Mdt::DeployUtils;

void LddWrapperTest::initTestCase()
{
}

void LddWrapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LddWrapperTest::runLddTest()
{
#ifndef Q_OS_UNIX
  QSKIP("This test can only work on unixes.");
#endif // #ifndef Q_OS_UNIX

  LddWrapper ldd;
  QVERIFY(ldd.execFindDependencies( QCoreApplication::applicationFilePath() ));
  QVERIFY(!ldd.readAllStandardOutputString().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  LddWrapperTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
