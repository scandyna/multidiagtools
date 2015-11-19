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

void mdtCsvTest::sandbox()
{
  using std::string;
  ///std::string str("A,B,C,D\n");
  std::string str("ABC\r\nZ,UV\rDE,F\n456\r\nTEF");
  ///std::string str("ABC");
  mdtCsvStringParser csv;
  mdtCsvRecord record;

  csv.setSource(str);
  record = csv.readLine();
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 1);
  QCOMPARE(record.rawColumnDataList.at(0), string("ABC"));

  for(const auto & str : record.rawColumnDataList){
    std::cout << str << "|";
  }
  std::cout << std::endl;
  
  record = csv.readLine();
  for(const auto & str : record.rawColumnDataList){
    std::cout << str << "|";
  }
  std::cout << std::endl;

  record = csv.readLine();
  for(const auto & str : record.rawColumnDataList){
    std::cout << str << "|";
  }
  std::cout << std::endl;

}

void mdtCsvTest::settingsTest()
{
  using std::string;

  mdtCsvParserSettings s;

  /*
   * Initial state
   */
  QCOMPARE(s.fieldSeparator, string(","));
  QCOMPARE(s.fieldProtection, string("\""));
  /*
   * Some setup
   */
  s.fieldSeparator = ";";
  s.fieldProtection = "'";
  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.fieldSeparator, string(","));
  QCOMPARE(s.fieldProtection, string("\""));
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
  record.rawColumnDataList.append("A");
  QCOMPARE(record.count(), 1);
  /*
   * Clear
   */
  record.clear();
  QVERIFY(!record.errorOccured());
  QCOMPARE(record.count(), 0);
}

void mdtCsvTest::stringParserReadLineTest()
{
  mdtCsvStringParser parser;
  mdtCsvRecord record;
  mdtCsvData data;
  QFETCH(QByteArray, sourceData);
  QFETCH(mdtCsvData, expectedData);

  // Setup CSV source string and parser
  std::string str = sourceData.toStdString();
  parser.setSource(str);
  // Parse line by line
  while(!parser.atEnd()){
    record = parser.readLine();
    QVERIFY(!record.errorOccured());
    data.addRecord(record);
  }
  // Check
  QCOMPARE(data.recordCount(), expectedData.recordCount());
  for(int row = 0; row < data.recordCount(); ++row){
    record = data.rawRecordList.at(row);
    auto expectedRecord = expectedData.rawRecordList.at(row);
    QCOMPARE(record.count(), expectedRecord.count());
    for(int col = 0; col < record.count(); ++col){
      auto colData = QString::fromStdString(record.rawColumnDataList.at(col));
      auto expectedColData = QString::fromStdString(expectedRecord.rawColumnDataList.at(col));
      QCOMPARE(colData, expectedColData);
    }
  }
}

void mdtCsvTest::stringParserReadLineTest_data()
{
  QTest::addColumn<QByteArray>("sourceData");
  QTest::addColumn<mdtCsvData>("expectedData");

  QByteArray sourceData;
  mdtCsvRecord expectedRecord;
  mdtCsvData expectedData;

  /*
   * Simple CSV tests:
   *  - Field separator: ,
   *  - No field protection
   *  - EOL: \n and \r\n
   */
  // Empty CSV
  sourceData = "";
  expectedData.clear();
  QTest::newRow("Empty(,|FP:None|EOL:None") << sourceData << expectedData;
  // Single char CSV - No EOL
  sourceData = "A";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:None") << sourceData << expectedData;
  // Single char CSV - \n EOL
  sourceData = "A\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\n") << sourceData << expectedData;
  // Single char CSV - \n EOL
  sourceData = "A\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("1 char(,|FP:None|EOL:\\r\\n") << sourceData << expectedData;
  // Single line CSV - No EOL
  sourceData = "A,B,C,D";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D") << sourceData << expectedData;
  // Single line CSV - \n EOL
  sourceData = "A,B,C,D\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n") << sourceData << expectedData;
  // Single line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n") << sourceData << expectedData;
  // 2 line CSV - \n EOL
  sourceData = "A,B,C,D\n1,2,3,4\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\n1,2,3,4\\n") << sourceData << expectedData;
  // 2 line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n1,2,3,4\r\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B" << "C" << "D";
  expectedData.addRecord(expectedRecord);
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "1" << "2" << "3" << "4";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("A,B,C,D\\r\\n1,2,3,4\\r\\n") << sourceData << expectedData;
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
  expectedRecord.rawColumnDataList << "A";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\\n") << sourceData << expectedData;
  // Single line CSV
  sourceData = "\"A\",\"B\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A" << "B";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\",\"B\"\\n") << sourceData << expectedData;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\nB\",\"C\"\\n") << sourceData << expectedData;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\r\nB\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A\r\nB" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\\r\\nB\",\"C\"\\n") << sourceData << expectedData;
  // Single line CSV with , in quoted field
  sourceData = "\"A,B\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A,B" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A,B\",\"C\"\\n") << sourceData << expectedData;
  // Single line CSV with double quoted string in quoted field
  sourceData = "\"A\"\"B\"\"\",\"C\"\n";
  expectedData.clear();
  expectedRecord.clear();
  expectedRecord.rawColumnDataList << "A\"B\"" << "C";
  expectedData.addRecord(expectedRecord);
  QTest::newRow("\"A\"\"B\"\"\",\"C\"\\n") << sourceData << expectedData;

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
