/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtCsvTest.h"
#include "mdtCsvStringParser.h"
#include "mdtCsvStringParserIterator.h"
#include "mdtCsvFileParserIterator.h"
#include "mdtCsvFileParserIteratorSharedData.h"
#include "mdtCsvFileParser.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdtApplication.h"
#include <QTemporaryFile>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QtTest>
#include <string>
#include <vector>

#include <iostream>
#include <QDebug>

/// Sandbox
#include <mdtCsvParserTemplate.h>

// void mdtCsvTest::sandbox()
// {
//   QString str("ABC\r\nZ,UV\rDE,F\n456\r\nTEF");
//   mdtCsvStringParser csv;
//   mdtCsvRecord record;
// 
//   csv.setSource(str);
//   record = csv.readLine();
//   QVERIFY(!record.errorOccured());
//   QCOMPARE(record.count(), 1);
//   QCOMPARE(record.columnDataList.at(0), QString("ABC"));
// 
//   for(const auto & str : record.columnDataList){
//     std::cout << str.toStdString() << "|";
//   }
//   std::cout << std::endl;
//   
//   record = csv.readLine();
//   for(const auto & str : record.columnDataList){
//     std::cout << str.toStdString() << "|";
//   }
//   std::cout << std::endl;
// 
//   record = csv.readLine();
//   for(const auto & str : record.columnDataList){
//     std::cout << str.toStdString() << "|";
//   }
//   std::cout << std::endl;
// 
// }

// #include <boost/phoenix/core.hpp>
// #include <boost/phoenix/function.hpp>
// #include <vector>
// #include <algorithm>
// 
// template <typename F>
// void print(F f)
// {
//   std::cout << f() << std::endl;
// }
// 
// struct isOddImpl
// {
//   typedef bool result_type;
// 
//   template <typename Arg>
//   bool operator()(Arg arg1) const
//   {
//     return arg1 % 2 == 1;
//   }
// };
// boost::phoenix::function<isOddImpl> isOdd;

// void mdtCsvTest::sandbox()
// {
//   using boost::phoenix::val;
//   using boost::phoenix::ref;
//   using boost::phoenix::arg_names::arg1;
//   using boost::phoenix::arg_names::arg2;
// 
//   print(val(3));
//   print(val("Hello !"));
// 
//   int i = 4;
//   print(ref(i));
//   std::string str = "Hello v2";
// 
//   i = 5;
//   std::cout << arg1(i) << std::endl;
//   i = 6;
//   std::cout << arg1(i, str) << std::endl;
//   i = 7;
//   std::cout << arg2(i, str) << std::endl;
// 
//   std::vector<int> v{1,2,2,2,3,4,5,6,7,8,9};
//   auto it = v.cbegin();
//   it = std::find_if(it, v.cend(), isOdd(arg1));
//   std::cout << "it: " << *it << std::endl;
//   ++it;
//   it = std::find_if(it, v.cend(), isOdd(arg1));
//   std::cout << "it: " << *it << std::endl;
// 
// }

void mdtCsvTest::sandbox()
{
  mdtReadIteratorTestFunction();
}


void mdtCsvTest::settingsTest()
{
  mdtCsvParserSettings s;

  /*
   * Initial state
   */
  QCOMPARE(s.fieldSeparator, ',');
  QCOMPARE(s.fieldProtection, '\"');
  QCOMPARE(s.parseExp, true);
  /*
   * Some setup
   */
  s.fieldSeparator = ';';
  s.fieldProtection = '\'';
  s.parseExp = false;
  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.fieldSeparator, ',');
  QCOMPARE(s.fieldProtection, '\"');
  QCOMPARE(s.parseExp, true);
}

void mdtCsvTest::recordTest()
{
  mdtCsvRecord record;

  /*
   * Initial state
   */
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 0);
  /*
   * Set
   */
  record.setErrorOccured();
  QVERIFY(record.errorOccured());
  record.columnDataList.append("A");
  QCOMPARE(record.count(), 1);
  /*
   * Clear
   */
  record.clear();
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 0);
}

void mdtCsvTest::dataTest()
{
  QSKIP("Not implemented yet");
}

void mdtCsvTest::csvParserQStringIteratorTest()
{
  QString str;

  /*
   * Constructs and assignements
   */
  str = "ABCDEF";
  // Direct assignement
  mdtCsvStringParserIterator it(str.cbegin());
  QCOMPARE(*it, wchar_t('A'));
  // Default constructed
  mdtCsvStringParserIterator first, last;
  // Assignement
  first = str.cbegin();
  QCOMPARE(*first, wchar_t('A'));
  last = first;
  QCOMPARE(*last, wchar_t('A'));
  /*
   * Increment
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  // Pre-increment
  ++it;
  QCOMPARE(*it, wchar_t('B'));
  // Post-increment
  QCOMPARE(*it++, wchar_t('B'));
  QCOMPARE(*it, wchar_t('C'));
  /*
   * Decrement
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  ++it;
  ++it;
  QCOMPARE(*it, wchar_t('C'));
  // Pre-decrement
  --it;
  QCOMPARE(*it, wchar_t('B'));
  // Post-decrement
  QCOMPARE(*it--, wchar_t('B'));
  QCOMPARE(*it, wchar_t('A'));
  /*
   * Increment and decrement by n
   */
  it = str.cbegin();
  QCOMPARE(*it, wchar_t('A'));
  it += 2;
  QCOMPARE(*it, wchar_t('C'));
  it -= 2;
  QCOMPARE(*it, wchar_t('A'));
  first = it;
  it = first + 2;
  QCOMPARE(*it, wchar_t('C'));
  first = it - 1;
  QCOMPARE(*first, wchar_t('B'));
  /*
   * Index based access
   */
  it = str.cbegin();
  QCOMPARE(it[0], wchar_t('A'));
  QCOMPARE(it[1], wchar_t('B'));
  QCOMPARE(it[2], wchar_t('C'));
  /*
   * Comparisons
   */
  first = str.cbegin();
  it = first;
  QVERIFY(it == first);
  QVERIFY(!(it != first));
  QVERIFY(!(it < first));
  QVERIFY(it <= first);
  QVERIFY(!(it > first));
  QVERIFY(it >= first);
  // Increment it and check
  ++it;
  QVERIFY(!(it == first));
  QVERIFY(it != first);
  QVERIFY(!(it < first));
  QVERIFY(!(it <= first));
  QVERIFY(it > first);
  QVERIFY(it >= first);
}

void mdtCsvTest::csvParserQStringIteratorBenchmark()
{
  QString source = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
  std::vector<wchar_t> destination;

  destination.reserve(source.size());
  QBENCHMARK
  {
    destination.clear();
    mdtCsvStringParserIterator first(source.cbegin());
    mdtCsvStringParserIterator last(source.cend());
    while(first != last){
      destination.push_back(*first);
      ++first;
    }
  }
  QCOMPARE((int)destination.size(), source.size());
}

void mdtCsvTest::stringParserReadLineTest()
{
  QFETCH(QString, sourceData);
  QFETCH(mdtCsvData, expectedData);
  QFETCH(bool, expectedOk);
  QFETCH(mdtCsvParserSettings, csvSettings);
  mdtCsvStringParser parser(csvSettings);
  mdtCsvRecord record;
  mdtCsvData data;

  /*
   * Initial state
   */
  QVERIFY(parser.atEnd());
  // Setup CSV source string and parser
  parser.setSource(sourceData);
  // Parse line by line
  while(!parser.atEnd()){
    record = parser.readLine();
    QVERIFY(record.errorOccured() == !expectedOk);
    data.addRecord(record);
  }
  // Check
  QCOMPARE(data.recordCount(), expectedData.recordCount());
  for(int row = 0; row < data.recordCount(); ++row){
    record = data.recordList.at(row);
    auto expectedRecord = expectedData.recordList.at(row);
    QCOMPARE(record.count(), expectedRecord.count());
    for(int col = 0; col < record.count(); ++col){
      auto colData = record.columnDataList.at(col);
      auto expectedColData = expectedRecord.columnDataList.at(col);
      QCOMPARE(colData, expectedColData);
    }
  }
}

void mdtCsvTest::stringParserReadLineTest_data()
{
  buildCsvTestData();
}

void mdtCsvTest::csvFileParserIteratorSharedDataTest()
{
  mdtCsvFileParserIteratorSharedData sd;
  QTemporaryFile file;

  QVERIFY(QTextCodec::codecForName("UTF-8") != nullptr);
  /*
   * Initial state
   */
  QVERIFY(sd.atEnd());
  /*
   * set source to a empty file
   */
  // Pass file that is no allready open + existing encoding
  QVERIFY(!file.isOpen());
  QVERIFY(!sd.setSource(&file, "UTF-8"));
  QVERIFY(!file.isOpen());
  QVERIFY(sd.atEnd());
  // Pass file that is allready open + existing encoding
  QVERIFY(file.open());
  QVERIFY(sd.setSource(&file, "UTF-8"));
  QVERIFY(file.isOpen());
  QVERIFY(sd.atEnd());
  // Pass a file that is allready open + NON existing encoding
  QVERIFY(!sd.setSource(&file, "SomeUnknownEncoding"));
  QVERIFY(sd.atEnd());
  QVERIFY(file.isOpen()); // file must be keeped untouched
  /*
   * Prepare file with some data
   */
  file.close();
  QVERIFY(file.open());
  QVERIFY(file.write("ABCDE") > 0);
  file.close();
  /*
   * Some flags checks
   */
  QVERIFY(file.open());
  QVERIFY(sd.setSource(&file, "UTF-8"));
  QVERIFY(!sd.atEnd());
  QVERIFY(!sd.atEnd());
  // Close file and check that sd is at end
  file.close();
  ///QVERIFY(sd.atEnd());
  /*
   * Check getting data
   */
  QVERIFY(file.open());
  QVERIFY(sd.setSource(&file, "UTF-8"));
  QVERIFY(file.isOpen());
  // Get first char
  QVERIFY(!sd.atEnd());
  QCOMPARE(sd.get(), QChar('A'));
  QCOMPARE(sd.get(), QChar('A'));
  // Step and get
  QVERIFY(!sd.atEnd());
  QVERIFY(sd.advance());
  QCOMPARE(sd.get(), QChar('B'));
  // Step and get
  QVERIFY(!sd.atEnd());
  QVERIFY(sd.advance());
  QCOMPARE(sd.get(), QChar('C'));
  // Step and get
  QVERIFY(!sd.atEnd());
  QVERIFY(sd.advance());
  QCOMPARE(sd.get(), QChar('D'));
  // Step and get
  QVERIFY(!sd.atEnd());
  QVERIFY(sd.advance());
  QCOMPARE(sd.get(), QChar('E'));
  // Step again - Now we reached EOF
  QVERIFY(sd.advance());
  QVERIFY(sd.atEnd());
  /*
   * Check error handling
   */
  // Re-open file
  file.close();
  QVERIFY(file.open());
  mdtCsvFileParserIteratorSharedData sd2(1);
  QVERIFY(sd2.setSource(&file, "UTF-8"));
  // Get first char
  QVERIFY(!sd2.atEnd());
  QCOMPARE(sd2.get(), QChar('A'));
  // Step and get
  QVERIFY(sd2.advance());
  QVERIFY(!sd2.atEnd());
  QCOMPARE(sd2.get(), QChar('B'));
  // Close file
  file.close();
  QVERIFY(!sd2.advance());
  QVERIFY(sd2.atEnd());
}

void mdtCsvTest::csvFileParserIteratorSharedDataReadTest()
{
  QTemporaryFile file;
  QFETCH(QString, fileData);
  QFETCH(QString, fileEncoding);
  QFETCH(int, rawDataBufferSize);
  QString unicodeBuffer;
  QByteArray fileRawData;
  QTextCodec *codec;
  mdtCsvFileParserIteratorSharedData sd(rawDataBufferSize);

  QVERIFY(QTextCodec::codecForName(fileEncoding.toLatin1()) != nullptr);
  /*
   * Write our test data
   */
  // Encode test data
  codec = QTextCodec::codecForName(fileEncoding.toLatin1());
  QVERIFY(codec != nullptr);
  fileRawData = codec->fromUnicode(fileData);
  // Write test data
  QVERIFY(file.open());
  QVERIFY(file.write(fileRawData) == fileRawData.size());
  // Close file (to flush data) and re-open it
  file.close();
  QVERIFY(file.open());
  /*
   * Check
   */
  QVERIFY(unicodeBuffer.isEmpty());
  // Read data
  QVERIFY(sd.setSource(&file, fileEncoding.toLatin1()));
//   sd.setInternalRawDataBufferSize(rawDataBufferSize);
  while(!sd.atEnd()){
    unicodeBuffer.append(sd.get());
    QVERIFY(sd.advance());
  }
  // Check
  QCOMPARE(unicodeBuffer, fileData);
}

void mdtCsvTest::csvFileParserIteratorSharedDataReadTest_data()
{
  QTest::addColumn<QString>("fileData");
  QTest::addColumn<QString>("fileEncoding");
  QTest::addColumn<int>("rawDataBufferSize");

  // Note: only edit this part with a UTF-8 editor
  QTest::newRow("Empty file,UTF-8,1") << "" << "UTF-8" << 1;
  QTest::newRow("Empty file,UTF-8,2") << "" << "UTF-8" << 2;
  QTest::newRow("Empty file,UTF-8,3") << "" << "UTF-8" << 3;
  QTest::newRow("Empty file,UTF-8,1024") << "" << "UTF-8" << 1024;
  QTest::newRow("A,UTF-8,1") << "A" << "UTF-8" << 1;
  QTest::newRow("A,UTF-8,2") << "A" << "UTF-8" << 2;
  QTest::newRow("A,UTF-8,3") << "A" << "UTF-8" << 3;
  QTest::newRow("A,UTF-8,4") << "A" << "UTF-8" << 4;
  QTest::newRow("A,UTF-8,1024") << "A" << "UTF-8" << 1024;
  QTest::newRow("ö,UTF-8,1") << "ö" << "UTF-8" << 1;
  QTest::newRow("ö,UTF-8,2") << "ö" << "UTF-8" << 2;
  QTest::newRow("ö,UTF-8,3") << "ö" << "UTF-8" << 3;
  QTest::newRow("ö,UTF-8,4") << "ö" << "UTF-8" << 4;
  QTest::newRow("ö,UTF-8,1024") << "ö" << "UTF-8" << 1024;
  QTest::newRow("AB,UTF-8,1") << "AB" << "UTF-8" << 1;
  QTest::newRow("AB,UTF-8,2") << "AB" << "UTF-8" << 2;
  QTest::newRow("AB,UTF-8,3") << "AB" << "UTF-8" << 3;
  QTest::newRow("AB,UTF-8,4") << "AB" << "UTF-8" << 4;
  QTest::newRow("AB,UTF-8,1024") << "AB" << "UTF-8" << 1024;
  QTest::newRow("Aö,UTF-8,1") << "Aö" << "UTF-8" << 1;
  QTest::newRow("Aö,UTF-8,2") << "Aö" << "UTF-8" << 2;
  QTest::newRow("Aö,UTF-8,3") << "Aö" << "UTF-8" << 3;
  QTest::newRow("Aö,UTF-8,4") << "Aö" << "UTF-8" << 4;
  QTest::newRow("Aö,UTF-8,1024") << "Aö" << "UTF-8" << 1024;
  QTest::newRow("AöB,UTF-8,1") << "AöB" << "UTF-8" << 1;
  QTest::newRow("AöB,UTF-8,2") << "AöB" << "UTF-8" << 2;
  QTest::newRow("AöB,UTF-8,3") << "AöB" << "UTF-8" << 3;
  QTest::newRow("AöB,UTF-8,4") << "AöB" << "UTF-8" << 4;
  QTest::newRow("AöB,UTF-8,5") << "AöB" << "UTF-8" << 5;
  QTest::newRow("AöB,UTF-8,6") << "AöB" << "UTF-8" << 6;
  QTest::newRow("AöB,UTF-8,1024") << "AöB" << "UTF-8" << 1024;
  QTest::newRow("Medium ASCII string,UTF-8,3") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n0123456789\nabcdefghijklmnopqrstuvwxyz" << "UTF-8" << 3;
  QTest::newRow("Medium ASCII string,UTF-8,1024") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n0123456789\nabcdefghijklmnopqrstuvwxyz" << "UTF-8" << 1024;
  QTest::newRow("Medium string,UTF-8,3") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-8" << 3;
  QTest::newRow("Medium string,UTF-8,1024") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-8" << 1024;
  QTest::newRow("Medium string,UTF-16,3") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-16" << 3;
  QTest::newRow("Medium string,UTF-16,1024") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-16" << 1024;
  QTest::newRow("Medium string,UTF-32,3") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-32" << 3;
  QTest::newRow("Medium string,UTF-32,1024") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ@01€23456789aäbcdeéfghijklmnoöpqrstuvwxyz" << "UTF-32" << 1024;
}

void mdtCsvTest::csvFileParserIteratorTest()
{
  QTemporaryFile file;
  QString str;

  QVERIFY(QTextCodec::codecForName("UTF-8") != nullptr);
  /*
   * Prepare file
   */
  QVERIFY(file.open());
  QVERIFY(file.write("ABCDE") > 0);
  file.close();
  /*
   * Constructs
   */
  // Default (end-of-stream iterator)
  mdtCsvFileParserIterator it;
  QVERIFY(!it.errorOccured());
  QVERIFY(it.isEof());
  // Copy
  mdtCsvFileParserIterator it2(it);
  QVERIFY(!it2.errorOccured());
  QVERIFY(it2.isEof());
  QVERIFY(it == it2);
  QVERIFY(!(it != it2));
  // Assign file to it
  QVERIFY(file.open());
  QVERIFY(it.setSource(&file, "UTF-8"));
  QVERIFY(!it.errorOccured());
  QVERIFY(!it.isEof());
  file.close();
  // Assign again
  QVERIFY(file.open());
  QVERIFY(it.setSource(&file, "UTF-8"));
  QVERIFY(!it.errorOccured());
  QVERIFY(!it.isEof());
  // Clear
  it.clear();
  QVERIFY(it.isEof());
  file.close();
  // Assign again
  QVERIFY(file.open());
  QVERIFY(it.setSource(&file, "UTF-8"));
  QVERIFY(!it.errorOccured());
  QVERIFY(!it.isEof());
  file.close();
  // Construct it3 that acts on file
  QVERIFY(file.open());
  mdtCsvFileParserIterator it3(&file, "UTF-8");
  QVERIFY(!it3.errorOccured());
  QVERIFY(!it3.isEof());
  QVERIFY(*it3 == 'A');
  // Construct it4 that is a copy of it3
  mdtCsvFileParserIterator it4(it3);
  QVERIFY(!it4.errorOccured());
  QVERIFY(!it4.isEof());
  QVERIFY(*it4 == 'A');
  QVERIFY(it4 == it3);
  // Assign it4 to it
  it = it4;
  QVERIFY(!it.errorOccured());
  QVERIFY(!it.isEof());
  QVERIFY(*it == 'A');
  QVERIFY(it == it4);
  // Step it and check
  QVERIFY(!it.isEof());
  ++it;
  QVERIFY(*it == 'B');
  // Step it and check
  QVERIFY(!it.isEof());
  ++it;
  QVERIFY(*it == 'C');
  // Step it and check
  QVERIFY(!it.isEof());
  ++it;
  QVERIFY(*it == 'D');
  // Step it and check
  QVERIFY(!it.isEof());
  ++it;
  QVERIFY(*it == 'E');
  // Step - Now we reached EOF
  ++it;
  QVERIFY(it.isEof());
  /*
   * Some error handling
   */
  // Close the file and check that we end up with end-of-stream iterators
  file.close();
  QVERIFY(it.isEof());
  QVERIFY(it3.isEof());
  QVERIFY(it4.isEof());
  /*
   * Check reading the whole file
   */
  file.close();
  QVERIFY(file.open());
  mdtCsvFileParserIterator first(&file, "UTF-8");
  mdtCsvFileParserIterator last;
  str.clear();
  while(first != last){
    str.append(static_cast<ushort>(*first));
    ++first;
  }
  QCOMPARE(str, QString("ABCDE"));
}

void mdtCsvTest::csvFileParserMultiPassIteratorTest()
{
  using namespace boost::spirit;
  QTemporaryFile file;
  QString str;

  QVERIFY(QTextCodec::codecForName("UTF-8") != nullptr);
  /*
   * Prepare file
   */
  QVERIFY(file.open());
  QVERIFY(file.write(u8"ABCDE\n1234\n") > 0);
  file.close();
  /*
   * Create our iterators
   */
  QVERIFY(file.open());
  mdtCsvFileParserIterator it(&file, "UTF-8");
  auto first = make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator(it));
  auto last = make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator());
  QVERIFY(!it.errorOccured());
  /*
   * Check iterating
   */
//   QVERIFY(*first == 'A');
//   ++first;
//   QVERIFY(*first == 'B');
//   --first;
//   QVERIFY(*first == 'A');
  // Check iterating the whole file
  QVERIFY(*first == 'A');
  while(first != last){
    str.append(static_cast<ushort>(*first));
    ++first;
  }
  QCOMPARE(str, QString(u8"ABCDE\n1234\n"));

  ///mdtCsvParserSettings csvSettings;
  ///mdtCsvParserTemplate<mdtCsvFileParserMultiPassIterator> parser(csvSettings);
  ///parser.setSource(first, last);
  ///auto rec = parser.readLine();
  ///auto rec = parser.readLine(first, last);
  ///auto rec = parser.readLine(first, make_multi_pass<mdtCsvFileParserMultiPassPolicy, mdtCsvFileParserIterator>(mdtCsvFileParserIterator()));
  ///qDebug() << rec.columnDataList;

}

void mdtCsvTest::csvFileParserTest()
{
  mdtCsvFileParser parser;
  QTemporaryFile file;
  QString str;
  mdtCsvRecord record;

  QVERIFY(QTextCodec::codecForName("UTF-8") != nullptr);
  /*
   * Prepare file
   */
  QVERIFY(file.open());
  QVERIFY(file.write(u8"ABCDE\n1234\n") > 0);
  file.close();
  /*
   * Initial state
   */
  QVERIFY(parser.atEnd());
  /*
   * Open/close test
   */
  QVERIFY(parser.openFile(file.fileName(), "UTF-8"));
  QVERIFY(!parser.atEnd());
  parser.closeFile();
  QVERIFY(parser.atEnd());
  QVERIFY(parser.openFile(file.fileName(), "UTF-8"));
  QVERIFY(!parser.atEnd());
  /*
   * Read line by line
   */
  // Read first line
  QVERIFY(!parser.atEnd());
  record = parser.readLine();
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 1);
  QCOMPARE(record.columnDataList.at(0), QString("ABCDE"));
  // Read second line
  QVERIFY(!parser.atEnd());
  record = parser.readLine();
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 1);
  QCOMPARE(record.columnDataList.at(0), QString("1234"));
  QVERIFY(parser.atEnd());
}

void mdtCsvTest::csvFileParserReadAllTest()
{

}

void mdtCsvTest::csvFileParserReadAllTest_data()
{
  buildCsvTestData();
}

void mdtCsvTest::buildCsvTestData()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<mdtCsvData>("expectedData");
  QTest::addColumn<bool>("expectedOk");
  QTest::addColumn<mdtCsvParserSettings>("csvSettings");

  QString sourceData;
  mdtCsvRecord expectedRecord;
  mdtCsvData expectedData;
  mdtCsvParserSettings csvSettings;
  const bool Ok = true;
  const bool Nok = false;

  /*
   * Testst are generally based on CSV-1203 standard.
   * Some exceptions also comes from RFC 4180
   */

  /*
   * Simple CSV tests:
   *  - Tests witout field protection
   *  - Test with EOL: \n and \r\n
   */
  csvSettings.fieldSeparator = ',';
  csvSettings.fieldProtection = '"';
  csvSettings.parseExp = false;
  // Empty CSV
  sourceData = "";
  expectedData.clear();
  QTest::newRow("Empty(,|FP:None|EOL:None") << sourceData << expectedData << Nok << csvSettings;
  // Single char CSV - No EOL (No EOL is allowed by RFC 4180)
  sourceData = "A";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:None") << sourceData << expectedData << Ok << csvSettings;
  // Single char CSV - \n EOL
  sourceData = "A\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single char CSV - \n EOL
  sourceData = "A\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - No EOL (No EOL is allowed by RFC 4180)
  sourceData = "A,B,C,D";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - \n EOL
  sourceData = "A,B,C,D\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (more than 1 char per field) - \n EOL
  sourceData = "ABCD\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("ABCD\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (more than 1 char per field) - \n EOL
  sourceData = "ABCD,EFGH\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("ABCD,EFGH\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // 2 line CSV - \n EOL
  sourceData = "A,B,C,D\n1,2,3,4\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.columnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n1,2,3,4\\n") << sourceData << expectedData << Ok << csvSettings;
  // 2 line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n1,2,3,4\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.columnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n1,2,3,4\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Quoted CSV tests:
   *  - Test with EOL: \n and \r\n
   */
  csvSettings.fieldSeparator = ',';
  csvSettings.fieldProtection = '"';
  csvSettings.parseExp = false;
  // Single char CSV
  sourceData = "\"A\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV
  sourceData = "\"ABCD\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"ABCD\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV with quoted parts
  sourceData = "\"A\"\"BC\"\"D\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\"BC\"D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\"BC\"\"D\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV
  sourceData = "\"A\",\"B\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\",\"B\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (with many charachter strings)
  sourceData = "\"ABCD\",\"EFGH\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"ABCD\",\"EFGH\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\nB\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\r\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\r\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\r\\nB\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with , in quoted field
  sourceData = "\"A,B\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A,B" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A,B\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with double quoted string in quoted field
  sourceData = "\"A\"\"B\"\"\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\"B\"" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\"B\"\"\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // RFC 4180 escape quote. We are not conform, but check that this does not crasch
  /// \todo Determine if this should generate a error or not
  sourceData = "\"A\"\"B\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\"B";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\"B\"\\n") << sourceData << expectedData << Nok << csvSettings;
  /*
   * Excel protection marker CSV tests
   */
  csvSettings.fieldSeparator = ',';
  csvSettings.fieldProtection = '\"';
  csvSettings.parseExp = true;
  // Single char CSV
  sourceData = "~A\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("~A\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV
  sourceData = "~ABCD\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("~ABCD\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single CSV line
  sourceData = "~ABCD,EFGH\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("~ABCD,EFGH\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single quoted CSV line
  sourceData = "\"~ABCD\",\"EFGH\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"~ABCD\",\"EFGH\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Do not parse EXP
  csvSettings.parseExp = false;
  // Single CSV line
  sourceData = "~ABCD,EFGH\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "~ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("~ABCD,EFGH\\n(EXP ignored)") << sourceData << expectedData << Ok << csvSettings;
  // Single quoted CSV line
  sourceData = "\"~ABCD\",\"EFGH\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "~ABCD" << "EFGH";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"~ABCD\",\"EFGH\"\\n(EXP ignored)") << sourceData << expectedData << Ok << csvSettings;

  /*
   * Some tests witch non ASCII strings:
   * Note: only edit this part with a UTF-8 editor !
   */
  csvSettings.fieldSeparator = ',';
  csvSettings.fieldProtection = '\"';
  csvSettings.parseExp = true;
  // Non quoted
  sourceData = u8"A,é,à,B,è,ü,ö,ä,\u03B1,\u012C\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "é" << "à" << "B" << "è" << "ü" << "ö" << "ä" << u8"\u03B1" << u8"\u012C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow(u8"A,é,à,B,è,ü,ö,ä,\u03B1,\u012C\\n") << sourceData << expectedData << Ok << csvSettings;
  // Non quoted
  sourceData = u8"A,éà,Bèü,öä\u03B1\u012C\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << u8"éà" << u8"Bèü" << u8"öä\u03B1\u012C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow(u8"A,éà,Bèü,öä\u03B1\u012C\\n") << sourceData << expectedData << Ok << csvSettings;
  // Quoted
  sourceData = u8"\"A\",\"é\",\"à\",\"B\",\"è\",\"ü\",\"ö\",\"ä\",\"\u03B1\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "é" << "à" << "B" << "è" << "ü" << "ö" << "ä" << u8"\u03B1";
  expectedData.addRecord(expectedRecord);
  QTest::newRow(u8"\"A\",\"é\",\"à\",\"B\",\"è\",\"ü\",\"ö\",\"ä\",\"\u03B1\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Quoted
  sourceData = u8"\"A,é\",\"à,B\",\"è,ü\",\"ö,ä,\u03B1\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << u8"A,é" << u8"à,B" << u8"è,ü" << u8"ö,ä,\u03B1";
  expectedData.addRecord(expectedRecord);
  QTest::newRow(u8"\"A,é\",\"à,B\",\"è,ü\",\"ö,ä,\u03B1\"\\n") << sourceData << expectedData << Ok << csvSettings;

  /// \todo add casese from: http://stackoverflow.com/questions/7436481/how-to-make-my-split-work-only-on-one-real-line-and-be-capable-to-skip-quoted-pa/7462539#7462539

  /// \todo Add some from /home/philippe/programmation/c_cpp/libreoffiche/git/core/sc/qa/unit/data/contentCSV/
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtCsvTest csvTest;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&csvTest, argc, argv);
}
