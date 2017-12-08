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
#include "CreateFileTest.h"
#include "Mdt/TestLib/CreateFile.h"
#include <QTemporaryDir>
#include <QDir>
#include <QFile>

using namespace Mdt::TestLib;

void CreateFileTest::initTestCase()
{
}

void CreateFileTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void CreateFileTest::createFileTest()
{
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const auto filePath = QDir::cleanPath(dir.path() + "/subA/subB/file.txt");
  QVERIFY(!QFile::exists(filePath));
  QVERIFY(createFile(filePath));
  QVERIFY(QFile::exists(filePath));
}

void CreateFileTest::createFileInDirectoryTest()
{
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QString fileName = "file.txt";
  const auto dirPath = QDir::cleanPath( dir.path() + "/subC" );
  const auto filePath = QDir::cleanPath( dirPath + "/" + fileName );
  QVERIFY(!QFile::exists(filePath));
  QVERIFY(createFileInDirectory(dirPath, fileName));
  QVERIFY(QFile::exists(filePath));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CreateFileTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
