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
#include "FileMultiPassIteratorTest.h"
#include "Mdt/PlainText/FileMultiPassIterator.h"
#include <QTemporaryFile>
#include <QTextCodec>

using namespace Mdt::PlainText;

void FileMultiPassIteratorTest::initTestCase()
{
}

void FileMultiPassIteratorTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FileMultiPassIteratorTest::multiPassTest()
{
  QVERIFY(QTextCodec::codecForName("UTF-8") != nullptr);
  /*
   * Prepare file
   */
  QTemporaryFile file;
  QVERIFY(file.open());
  QVERIFY(file.write(u8"ABCDE\n1234\n") > 0);
  file.close();
  /*
   * Create our iterators
   */
  QVERIFY(file.open());
  FileInputIterator it(&file, "UTF-8");
  auto first = makeFileMultiPassIterator(it);
  auto last = makeFileMultiPassIterator(FileInputIterator());
  QVERIFY(!it.errorOccured());
  /*
   * Check iterating
   */
  QVERIFY(*first == 'A');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == 'B');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == 'C');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == 'D');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == 'E');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '\n');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '1');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '2');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '3');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '4');
  QVERIFY(*first == '4');
  // Step and check
  QVERIFY(first != last);
  ++first;
  QVERIFY(*first == '\n');
  // Now we reach eof
  QVERIFY(first != last);
  ++first;
  QVERIFY(first == last);

  // Check iterating the whole file
///  QVERIFY(*first == 'A');
//   while(first != last){
//     str.append(static_cast<ushort>(*first));
//     ++first;
//   }
//   QCOMPARE(str, QString(u8"ABCDE\n1234\n"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FileMultiPassIteratorTest test;

  return QTest::qExec(&test, argc, argv);
}
