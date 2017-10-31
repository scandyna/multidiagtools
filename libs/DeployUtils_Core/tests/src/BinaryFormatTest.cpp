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
#include "BinaryFormatTest.h"
#include "Mdt/DeployUtils/BinaryFormat.h"
#include "Mdt/DeployUtils/ObjdumpWrapper.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QCoreApplication>
#include <QFileInfo>

using namespace Mdt::DeployUtils;

void BinaryFormatTest::initTestCase()
{
}

void BinaryFormatTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void BinaryFormatTest::isFileAnExecutableByExtTest()
{
  QFETCH(QString, file);
  QFETCH(bool, expectedIsAnExecutable);

  QCOMPARE( BinaryFormat::isFileAnExecutableByExtension(file), expectedIsAnExecutable );
}

void BinaryFormatTest::isFileAnExecutableByExtTest_data()
{
  QTest::addColumn<QString>("file");
  QTest::addColumn<bool>("expectedIsAnExecutable");

  const bool isAnExecutable = true;
  const bool isNotAnExecutable = false;

  QTest::newRow("") << "" << isNotAnExecutable;
  QTest::newRow("a") << "a" << isAnExecutable;
  QTest::newRow("a.exe") << "a.exe" << isAnExecutable;
  QTest::newRow("a.txt") << "a.txt" << isNotAnExecutable;
  QTest::newRow("a.so") << "a.so" << isAnExecutable;
  QTest::newRow("a.so.1") << "a.so.1" << isAnExecutable;
  QTest::newRow("a.so.1.2") << "a.so.1.2" << isAnExecutable;
  QTest::newRow("a.so.1.2.3") << "a.so.1.2.3" << isAnExecutable;
  QTest::newRow("a.dll") << "a.dll" << isAnExecutable;
  QTest::newRow("a.h") << "a.h" << isNotAnExecutable;
  QTest::newRow("a.cpp") << "a.cpp" << isNotAnExecutable;
}

void BinaryFormatTest::isFileAnExecutableByExtBenchmark()
{
  QFETCH(QString, file);
  QFETCH(bool, expectedIsAnExecutable);
  bool isAnExecutable;

  QBENCHMARK{
   isAnExecutable = BinaryFormat::isFileAnExecutableByExtension(file);
  }
  QCOMPARE(isAnExecutable, expectedIsAnExecutable);
}

void BinaryFormatTest::isFileAnExecutableByExtBenchmark_data()
{
  QTest::addColumn<QString>("file");
  QTest::addColumn<bool>("expectedIsAnExecutable");

  const bool isAnExecutable = true;
  const bool isNotAnExecutable = false;

  QTest::newRow("a") << "a" << isAnExecutable;
  QTest::newRow("a.exe") << "a.exe" << isAnExecutable;
  QTest::newRow("a.txt") << "a.txt" << isNotAnExecutable;
  QTest::newRow("a.so.1.2.3") << "a.so.1.2.3" << isAnExecutable;
  QTest::newRow("a.dll") << "a.dll" << isAnExecutable;
}

void BinaryFormatTest::runTest()
{
  ObjdumpWrapper objdump;
  if(objdump.findObjdump().isEmpty()){
    QSKIP("Could not find objdump executable");
  }

  BinaryFormat format;

  QVERIFY( format.readFormat( QCoreApplication::applicationFilePath() ) );
  QCOMPARE(format.operatingSystem(), Platform::nativeOperatingSystem());
  QCOMPARE(format.processor(), Platform::nativeProcessor());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  BinaryFormatTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
