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
#include "mdtCsvParser.h"
#include "mdtCsvParserIterator.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdtApplication.h"
#include <QTemporaryFile>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <string>

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
  QString str = "A,B,C,D,E,é,ö,à,ä,è,ü\n";
  ///QString str = "A,B,C,D,E\n";
  std::wstring wstr = L"A,B,C,D,E,é,ö,à,ä,è,ü\n";

  mdtCsvParserQStringIterator first(str.begin());
  mdtCsvParserQStringIterator last(str.end());

  qDebug() << "sizeof(ushort): " << sizeof(ushort);
  qDebug() << "sizeof(wchar_t): " << sizeof(wchar_t);
  qDebug() << "sizeof(std::ptrdiff_t): " << sizeof(std::ptrdiff_t);
  qDebug() << "sizeof(QChar): " << sizeof(QChar);
  qDebug() << "sizeof(QChar*): " << sizeof(QChar*);
  
  std::wcout << wstr << std::endl;
  
  std::wcout << *first << std::endl;
  std::wcout << "Get it that is first advanced by 2" << std::endl;
  auto it = first + 2;
  std::wcout << "it: " << *it << " , first: " << *first << std::endl;
  std::wcout << "Get it that is first rewind by 2" << std::endl;
  it = it - 2;
  std::wcout << "it: " << *it << " , first: " << *first << std::endl;
  std::wcout << "Advance first by 2" << std::endl;
  first += 2;
  std::wcout << "first: " << *first << std::endl;
  std::wcout << "Rewind first by 2" << std::endl;
  first -= 2;
  std::wcout << "first: " << *first << std::endl;
  std::wcout << "first[3]: " << first[3] << std::endl;

  while(first != last)
  {
    ///qDebug() << first->unicode();
    ///qDebug() << *first;
    std::cout << "first < last ?: " << (first < last) << std::endl;
    std::cout << "first <= last ?: " << (first <= last) << std::endl;
    std::cout << "first > last ?: " << (first > last) << std::endl;
    std::cout << "first >= last ?: " << (first >= last) << std::endl;
    std::wcout << *first << std::endl;
    ++first;
  }
  std::cout << "first < last ?: " << (first < last) << std::endl;
  std::cout << "first <= last ?: " << (first <= last) << std::endl;
  std::cout << "first > last ?: " << (first > last) << std::endl;
  std::cout << "first >= last ?: " << (first >= last) << std::endl;

  
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;
  --first;
  std::wcout << *first << std::endl;

//   mdtCsvParserTemplate<mdtCsvParserQStringIterator> parser;
//   parser.setSource(str.cbegin(), str.cend());
// //   mdtCsvParserTemplate<std::wstring::const_iterator> parser;
// //   parser.setSource(wstr.cbegin(), wstr.cend());
//   
//   
//   mdtCsvRawRecord rec = parser.readLine();
//   qDebug() << "Error: " << rec.errorOccured();
//   for(const auto & data : rec.columnDataList){
//     std::wcout << data << std::endl;
//   }
}


void mdtCsvTest::settingsTest()
{
  mdtCsvParserSettings s;

  /*
   * Initial state
   */
  QCOMPARE(s.fieldSeparator, ',');
  QCOMPARE(s.fieldProtection, '\"');
  /*
   * Some setup
   */
  s.fieldSeparator = ';';
  s.fieldProtection = '\'';
  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.fieldSeparator, ',');
  QCOMPARE(s.fieldProtection, '\"');
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
  QFAIL("Not implemented yet");
}

void mdtCsvTest::csvParserQStringIteratorTest()
{
  QString str;
  ///wchar_t c;

  /*
   * Constructs and assignements
   */
  str = "ABCDEF";
  // Direct assignement
  mdtCsvParserQStringIterator it(str.cbegin());
  QCOMPARE(*it, wchar_t('A'));
  // Default constructed
  mdtCsvParserQStringIterator first, last;
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

}

void mdtCsvTest::stringParserReadLineTest()
{
  mdtCsvStringParser parser;
  mdtCsvRecord record;
  mdtCsvData data;
  QFETCH(QString, sourceData);
  QFETCH(mdtCsvData, expectedData);
  QFETCH(bool, expectedOk);

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
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<mdtCsvData>("expectedData");
  QTest::addColumn<bool>("expectedOk");

  QString sourceData;
  mdtCsvRecord expectedRecord;
  mdtCsvData expectedData;
  const bool Ok = true;
  const bool Nok = false;

  /*
   * Simple CSV tests:
   *  - Field separator: ,
   *  - No field protection
   *  - EOL: \n and \r\n
   */
  // Empty CSV
  /// \todo Define if it should be Nok
  sourceData = "";
  expectedData.clear();
  QTest::newRow("Empty(,|FP:None|EOL:None") << sourceData << expectedData << Ok;
  // Single char CSV - No EOL
  sourceData = "A";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:None") << sourceData << expectedData << Nok;
  // Single char CSV - \n EOL
  sourceData = "A\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\n") << sourceData << expectedData << Ok;
  // Single char CSV - \n EOL
  sourceData = "A\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\r\\n") << sourceData << expectedData << Ok;
  // Single line CSV - No EOL
  sourceData = "A,B,C,D";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D") << sourceData << expectedData << Nok;
  // Single line CSV - \n EOL
  sourceData = "A,B,C,D\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n") << sourceData << expectedData << Ok;
  // Single line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n") << sourceData << expectedData << Ok;
  // 2 line CSV - \n EOL
  sourceData = "A,B,C,D\n1,2,3,4\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.columnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n1,2,3,4\\n") << sourceData << expectedData << Ok;
  // 2 line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n1,2,3,4\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.columnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n1,2,3,4\\r\\n") << sourceData << expectedData << Ok;
  /*
   * Quoted CSV tests:
   *  - Field separator: ,
   *  - Field protection: "
   *  - EOL: \n
   */
  // Single char CSV
  sourceData = "\"A\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\\n") << sourceData << expectedData << Ok;
  // Single line CSV
  sourceData = "\"A\",\"B\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "B";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\",\"B\"\\n") << sourceData << expectedData << Ok;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\nB\",\"C\"\\n") << sourceData << expectedData << Ok;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\r\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\r\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\r\\nB\",\"C\"\\n") << sourceData << expectedData << Ok;
  // Single line CSV with , in quoted field
  sourceData = "\"A,B\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A,B" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A,B\",\"C\"\\n") << sourceData << expectedData << Ok;
  // Single line CSV with double quoted string in quoted field
  sourceData = "\"A\"\"B\"\"\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A\"B\"" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\"B\"\"\",\"C\"\\n") << sourceData << expectedData << Ok;
  /*
   * Some tests witch non ASCII strings:
   *  - Field separator: ,
   *  - Field protection: "
   *  - EOL: \n
   * Note: only edit this part with a UTF-8 editor !
   */
  // Non quoted
  sourceData = "A,é,à,B,è,ü,ö,ä";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.columnDataList << "A" << "é" << "à" << "B" << "è" << "ü" << "ö" << "ä";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,é,à,B,è,ü,ö,ä") << sourceData << expectedData << Ok;
  /// \todo Add quoted

  /// \todo Non ASCII strings
  
  /// \todo add casese from: http://stackoverflow.com/questions/7436481/how-to-make-my-split-work-only-on-one-real-line-and-be-capable-to-skip-quoted-pa/7462539#7462539

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
