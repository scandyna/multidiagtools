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
#include "Mdt/DeployUtils/BinaryDependenciesImplementationInterface.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QCoreApplication>
#include <QStringList>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>

// #include <QDebug>

#ifdef Q_OS_WIN
 #include "Mdt/DeployUtils/ObjdumpWrapper.h"
#endif

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

class BinaryDependenciesImplementationInterfaceTester : public BinaryDependenciesImplementationInterface
{
 public:

  bool findDependencies(const QString& binaryFilePath) override;
};

bool BinaryDependenciesImplementationInterfaceTester::findDependencies(const QString& binaryFilePath)
{
  QFileInfo binaryFileInfo(binaryFilePath);
  LibraryInfo li;
  li.setLibraryPlatformName(binaryFileInfo.baseName() + "_dep.so");
  li.setAbsoluteFilePath(binaryFileInfo.absoluteFilePath() + "_dep.so");
  LibraryInfoList dependencies;
  dependencies.addLibrary(li);
  setDependencies(dependencies);

  return true;
}

/*
 * Tests
 */

void BinaryDependenciesTest::implementationInterfaceTest()
{
  BinaryDependenciesImplementationInterfaceTester bdt;
  BinaryDependenciesImplementationInterface *bdi = &bdt;
  /*
   * Fin dependencies for 1 binary
   */
  QVERIFY(bdi->findDependencies("exe1"));
  QCOMPARE(bdi->dependencies().count(), 1);
  QCOMPARE(bdi->dependencies().at(0).libraryName().name(), QString("exe1_dep"));
  /*
   * Check running a second time
   */
  QVERIFY(bdi->findDependencies("exe2"));
  QCOMPARE(bdi->dependencies().count(), 1);
  QCOMPARE(bdi->dependencies().at(0).libraryName().name(), QString("exe2_dep"));
  /*
   * Check finding dependencies for a list of libraries
   */
  LibraryInfoList libraries;
  LibraryInfo lib;
  lib.setLibraryPlatformName("a.so");
  lib.setAbsoluteFilePath("/opt/lib/a.so");
  libraries.addLibrary(lib);
  lib.setLibraryPlatformName("b.so");
  lib.setAbsoluteFilePath("/opt/lib/b.so");
  libraries.addLibrary(lib);
  QVERIFY(bdi->findDependencies(libraries));
  QCOMPARE(bdi->dependencies().count(), 2);
  QCOMPARE(bdi->dependencies().at(0).libraryName().name(), QString("a_dep"));
  QCOMPARE(bdi->dependencies().at(1).libraryName().name(), QString("b_dep"));
}

void BinaryDependenciesTest::runTest()
{
#ifdef Q_OS_WIN
  ObjdumpWrapper objdump;
  if(objdump.findObjdump().isEmpty()){
    QSKIP("Could not find objdump executable");
  }
#endif

  const auto prefix = QString::fromLocal8Bit(PREFIX_PATH);
  PathList searchFirstPrefixPaths = PathList::fromStringList( prefix.split(';', QString::SkipEmptyParts) );
  BinaryDependencies deps;
  LibraryInfoList dependencies;

  /*
   * Find dependencies for a single binary
   */
  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath(), searchFirstPrefixPaths));
  dependencies = deps.dependencies();
  QVERIFY(!dependencies.isEmpty());
  /*
   * Find dependencie for a list of binaries.
   * No binary passed must exists in the dependencies
   */
  QStringList binaries;
  binaries << QCoreApplication::applicationFilePath() << dependencies.at(0).absoluteFilePath();
  QVERIFY(deps.findDependencies( binaries, searchFirstPrefixPaths));
  dependencies = deps.dependencies();
  QVERIFY(!dependencies.isEmpty());
  QStringList dependenciesStrList;
  for(const auto & li : dependencies){
    dependenciesStrList.append(li.absoluteFilePath());
  }
  QVERIFY(!dependenciesStrList.contains(binaries.at(0)));
  QVERIFY(!dependenciesStrList.contains(binaries.at(1)));
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
