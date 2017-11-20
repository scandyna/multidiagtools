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
#include "MdtLibraryTest.h"
#include "Mdt/DeployUtils/MdtLibrary.h"

using namespace Mdt::DeployUtils;

void MdtLibraryTest::initTestCase()
{
}

void MdtLibraryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void MdtLibraryTest::getMdtLibrariesTest()
{
  QFETCH(QStringList, librariesNames);
  QFETCH(QStringList, expectedMdtLibrariesNames);

  const auto libraries = buildLibraryInfoList("/tmp", librariesNames);
  const auto expectedMdtLibraries = buildLibraryInfoList("/tmp", expectedMdtLibrariesNames);
  const auto mdtLibraries = MdtLibrary::getMdtLibraries(libraries);
  QVERIFY(compareLibraryInfoList(mdtLibraries, expectedMdtLibraries));
}

void MdtLibraryTest::getMdtLibrariesTest_data()
{
  QTest::addColumn<QStringList>("librariesNames");
  QTest::addColumn<QStringList>("expectedMdtLibrariesNames");

  QTest::newRow("0") << QStringList{}
                    << QStringList{};

  QTest::newRow("1") << QStringList{"libMdt0Algorithm.so"}
                     << QStringList{"libMdt0Algorithm.so"};

  QTest::newRow("2") << QStringList{"libQt5Core.so","libMdt0Algorithm.so"}
                     << QStringList{"libMdt0Algorithm.so"};

  QTest::newRow("3") << QStringList{"A.so","libMdt0Algorithm.so"}
                     << QStringList{"libMdt0Algorithm.so"};

}

void MdtLibraryTest::baseNameTest()
{
  QFETCH(QString, libraryInputName);
  QFETCH(QString, expectedBaseName);

  const auto library = buildLibraryInfo("/tmp", libraryInputName);
  QCOMPARE(MdtLibrary::baseName(library), expectedBaseName);
}

void MdtLibraryTest::baseNameTest_data()
{
  QTest::addColumn<QString>("libraryInputName");
  QTest::addColumn<QString>("expectedBaseName");

  QTest::newRow("1") << "libMdt0Algorithm.so"
                     << "Algorithm";

  QTest::newRow("2") << "Mdt0Algorithm.so"
                     << "Algorithm";

  QTest::newRow("3") << "Algorithm.so"
                     << "Algorithm";

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  MdtLibraryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
