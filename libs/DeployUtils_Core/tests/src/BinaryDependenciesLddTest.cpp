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
#include "Mdt/DeployUtils/BinaryDependenciesLdd.h"
#include "Mdt/PlainText/TestUtils.h"
#include <QChar>
#include <QByteArray>
#include <algorithm>

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
  QFETCH(StringRecordList, expectedDependencies);

  BinaryDependenciesLdd bd;
  bd.fillAndSetDependencies(dependencies);
  QCOMPARE( toStringRecordList(bd.dependencies()), expectedDependencies );
}

void BinaryDependenciesLddTest::fillAndSetDependenciesTest_data()
{
  QTest::addColumn<StringRecordList>("dependencies");
  QTest::addColumn<StringRecordList>("expectedDependencies");

  const QString prefixPath = "/usr/lib/";

  QTest::addColumn<StringRecordList>("dependencies");
  QTest::addColumn<LibraryInfoList>("expectedDependencies");

  QTest::newRow("0")
    << StringRecordList{}
    << StringRecordList{};

  QTest::newRow("1")
    << createRecordList( QStringList{"libQt5Core.so"}, prefixPath )
    << createRecordList( QStringList{"libQt5Core.so"}, prefixPath );

  QTest::newRow("2")
    << createRecordList( QStringList{"linux-vdso.so.1","libQt5Core.so"}, prefixPath )
    << createRecordList( QStringList{"libQt5Core.so"}, prefixPath );

  QTest::newRow("NotFound")
    << StringRecordList{{"libA.so","/usr/lib/libA.so"},{"libB.so","not found"}}
    << StringRecordList{{"libA.so","/usr/lib/libA.so"}};
}

void BinaryDependenciesLddTest::fillAndSetDependenciesBenchmark()
{
  QFETCH(StringRecordList, dependencies);
  QFETCH(StringRecordList, expectedDependencies);

  BinaryDependenciesLdd bd;
  QBENCHMARK_ONCE{
    bd.fillAndSetDependencies(dependencies);
  }
  QCOMPARE( toStringRecordList(bd.dependencies()), expectedDependencies );
}

void BinaryDependenciesLddTest::fillAndSetDependenciesBenchmark_data()
{
  QTest::addColumn<StringRecordList>("dependencies");
  QTest::addColumn<StringRecordList>("expectedDependencies");

  const QString prefixPath = "/usr/lib/";

  QTest::newRow("10")
    << generateLibraryRecordList(10, prefixPath)
    << generateLibraryRecordList(10, prefixPath);

  QTest::newRow("20")
    << generateLibraryRecordList(20, prefixPath)
    << generateLibraryRecordList(20, prefixPath);

  QTest::newRow("30")
    << generateLibraryRecordList(30, prefixPath)
    << generateLibraryRecordList(30, prefixPath);
}

StringRecordList BinaryDependenciesLddTest::toStringRecordList(const LibraryInfoList& libraryInfoList)
{
  StringRecordList recordList;

  for(const auto & libraryInfo : libraryInfoList){
    recordList.appendRecord( StringRecord{libraryInfo.libraryName().toFullNameLinux(), libraryInfo.absoluteFilePath()} );
  }

  return recordList;
}

StringRecordList BinaryDependenciesLddTest::createRecordList(const QStringList& libNameList, const QString& pathPrefix)
{
  StringRecordList recList;

  for(const auto & libName : libNameList){
    recList.appendRecord( StringRecord{libName, pathPrefix+libName} );
  }

  return recList;
}

StringRecordList BinaryDependenciesLddTest::generateLibraryRecordList(int n, const QString & pathPrefix)
{
  QStringList libraryNameList;

  /*
   * Letters used to generate library names.
   * The one that are in the exclude list must not be present here
   * (else the benchmark will fail).
   */
  static const char letters[] = "abdefghijklnopqrstuvwxyABCDEFGHIJKLNOPQRSTUVWXYZ";
  libraryNameList.reserve(n);
  for(int i = 0; i < n; ++i){
    const char c = letters[ i % ( sizeof(letters)-1 ) ];
    libraryNameList.append( "lib" + QString( QChar(c) ) + ".so" );
  }
 
  return createRecordList(libraryNameList, pathPrefix);
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
