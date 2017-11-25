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
#include "LconvertWrapperTest.h"
#include "Mdt/DeployUtils/LconvertWrapper.h"
#include <QString>

#ifndef PREFIX_PATH
 #error "PREFIX_PATH missing"
#endif

using namespace Mdt::DeployUtils;

void LconvertWrapperTest::initTestCase()
{
}

void LconvertWrapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LconvertWrapperTest::runLconvertTest()
{
  LconvertWrapper lconvert;

  lconvert.findQtBinPath( getPathPrefixList() );
  // Run lconvert -h to see if executable is found and runs
  QVERIFY(lconvert.execLconvert({"-h"}));
}

/*
 * Helpers
 */

PathList LconvertWrapperTest::getPathPrefixList()
{
  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  return PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  LconvertWrapperTest test;

//   app.debugEnvironment();

  return QTest::qExec(&test, argc, argv);
}
