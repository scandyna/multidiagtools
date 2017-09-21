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
#include "BinaryDependenciesObjdumpTest.h"
#include "Mdt/DeployUtils/LibraryInfo.h"
#include "Mdt/DeployUtils/BinaryDependenciesObjdump.h"
#include "Mdt/PlainText/TestUtils.h"
#include <QChar>
#include <QByteArray>
#include <QString>
#include <algorithm>

#include <QDebug>

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void BinaryDependenciesObjdumpTest::initTestCase()
{
}

void BinaryDependenciesObjdumpTest::cleanupTestCase()
{
}

/*
 * Tests
 */


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  BinaryDependenciesObjdumpTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
