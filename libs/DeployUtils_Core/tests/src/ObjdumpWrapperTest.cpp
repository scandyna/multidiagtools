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
#include "ObjdumpWrapperTest.h"
#include "Mdt/DeployUtils/ObjdumpWrapper.h"
#include <QCoreApplication>
#include <QtGlobal>

// #include <QDebug>

using namespace Mdt::DeployUtils;

void ObjdumpWrapperTest::initTestCase()
{
}

void ObjdumpWrapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ObjdumpWrapperTest::runObjdumpTest()
{
#if not defined Q_OS_UNIX && not defined Q_OS_WIN
  QSKIP("This test can only work on Unixes and Windows.");
#endif

  ObjdumpWrapper objdump;
  if(objdump.findObjdump().isEmpty()){
    QSKIP("Could not find objdump executable");
  }
  QVERIFY(objdump.execFindDependencies( QCoreApplication::applicationFilePath() ));
  QVERIFY(!objdump.readAllStandardOutputString().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ObjdumpWrapperTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
