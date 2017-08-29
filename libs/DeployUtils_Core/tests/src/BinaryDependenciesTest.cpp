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
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>

// #include <QDebug>

#ifndef PREFIX_PATH
 #error "PREFIX_PATH missing"
#endif

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

void BinaryDependenciesTest::runTest()
{
  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  PathList searchFirstPrefixPaths = PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
  BinaryDependencies deps;

  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath(), searchFirstPrefixPaths));
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
