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
#include "BinaryDependenciesLddTest.h"
#include "Mdt/DeployUtils/LibraryInfo.h"
#include "Mdt/DeployUtils/LibraryInfoList.h"
#include "Mdt/DeployUtils/BinaryDependenciesLdd.h"
#include "Mdt/PlainText/StringRecordList.h"
#include "Mdt/PlainText/TestUtils.h"

using namespace Mdt::DeployUtils;
using namespace Mdt::PlainText;

void BinaryDependenciesLddTest::initTestCase()
{
}

void BinaryDependenciesLddTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void BinaryDependenciesLddTest::fillAndSetDependenciesTest()
{
  QFETCH(StringRecordList, dependencies);
  QFETCH(LibraryInfoList, expectedDependencies);

  BinaryDependenciesLdd bd;
  bd.fillAndSetDependencies(dependencies);
  QCOMPARE(bd.dependencies().count(), expectedDependencies.count());
  for(int i = 0; i < bd.dependencies().count(); ++i){
    QVERIFY(bd.dependencies().at(i) == expectedDependencies.at(i));
  }
  
  QFAIL("Not complete");
}

void BinaryDependenciesLddTest::fillAndSetDependenciesTest_data()
{
  QTest::addColumn<StringRecordList>("dependencies");
  QTest::addColumn<LibraryInfoList>("expectedDependencies");

  LibraryInfo liQt5Core;
  liQt5Core.setLibraryPlatformName("libQt5Core.so.5");
  LibraryInfo liLinuxVdso;
  liLinuxVdso.setLibraryPlatformName("linux-vdso.so.1");
  LibraryInfoList liList;

  ///liList.c
  QTest::newRow("")
    << StringRecordList{}
    << LibraryInfoList{};
// 
//   QTest::newRow("")
//     << StringRecordList{{"libQt5Core.so.5"}}
//     << StringRecordList{{"libQt5Core.so.5"}};
// 
//   QTest::newRow("")
//     << StringRecordList{{"linux-vdso.so.1"},{"libQt5Core.so.5"}}
//     << StringRecordList{{"libQt5Core.so.5"}};


}


void BinaryDependenciesLddTest::fillAndSetDependenciesBenchmark()
{
  QFAIL("Not complete");
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BinaryDependenciesLddTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
