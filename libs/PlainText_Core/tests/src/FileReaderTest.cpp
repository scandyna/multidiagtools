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
#include "FileReaderTest.h"
#include "Mdt/PlainText/FileReader.h"

using namespace Mdt::PlainText;

void FileReaderTest::initTestCase()
{
}

void FileReaderTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FileReaderTest::readFileTest()
{
  QFETCH(QString, fileData);
  QFETCH(QByteArray, encoding);
  QFETCH(int, maxSize);
  QFETCH(bool, expectedOk);

  QTemporaryFile file;
  if(QTextCodec::codecForName(encoding) == nullptr){
    QVERIFY(writeTemporaryTextFile(file, fileData, "UTF-8"));
  }else{
    QVERIFY(writeTemporaryTextFile(file, fileData, encoding));
  }

  FileReader reader;
  QCOMPARE(reader.readFile(file.fileName(), encoding, maxSize), expectedOk);
  if(expectedOk == true){
    QCOMPARE(reader.stringData(), fileData);
  }
}

void FileReaderTest::readFileTest_data()
{
  QTest::addColumn<QString>("fileData");
  QTest::addColumn<QByteArray>("encoding");
  QTest::addColumn<int>("maxSize");
  QTest::addColumn<bool>("expectedOk");

  const bool Ok = true;
  const bool Nok = false;
  const QByteArray Utf8 = "UTF-8";

  QTest::newRow("") << "" << Utf8 << 1024 << Ok;
  QTest::newRow("A,1024") << "A" << Utf8 << 1024 << Ok;
  QTest::newRow("A,1") << "A" << Utf8 << 1 << Ok;
  QTest::newRow("AB,2") << "AB" << Utf8 << 2 << Ok;
  QTest::newRow("AB,1") << "AB" << Utf8 << 1 << Nok;
  QTest::newRow("NonExistingCodec") << "A" << QByteArray("SomeNonExistingCodec") << 1024 << Nok;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FileReaderTest test;

  return QTest::qExec(&test, argc, argv);
}
