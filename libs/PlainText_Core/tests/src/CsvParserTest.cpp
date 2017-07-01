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
#include "CsvParserTest.h"
#include "Mdt/Expected.h"
#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include "Mdt/PlainText/CsvParserSettings.h"
#include "Mdt/PlainText/CsvStringParser.h"
#include "Mdt/PlainText/CsvFileParser.h"
#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QTemporaryFile>

using namespace Mdt::PlainText;

void CsvParserTest::initTestCase()
{
}

void CsvParserTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void CsvParserTest::stringParserReadLineTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedData);
  QFETCH(bool, expectedOk);
  QFETCH(CsvParserSettings, csvSettings);
  CsvStringParser parser;
  Mdt::Expected<StringRecord> record;
  StringRecordList data;
  /*
   * Initial state
   */
  QVERIFY(parser.atEnd());
  // Setup CSV source string and parser
  parser.setCsvSettings(csvSettings);
  parser.setSource(sourceData);
  // Parse line by line
  while(!parser.atEnd()){
    record = parser.readLine();
    if(expectedOk){
      QVERIFY(record.hasValue());
      data.appendRecord(record.value());
    }else{
      QVERIFY(record.hasError());
    }
  }
  // Check
  QCOMPARE(data.rowCount(), expectedData.rowCount());
  for(int row = 0; row < data.rowCount(); ++row){
    QCOMPARE(data.columnCount(row), expectedData.columnCount(row));
    for(int col = 0; col < data.columnCount(row); ++col){
      QCOMPARE(data.data(row, col), expectedData.data(row, col));
    }
  }
}

void CsvParserTest::stringParserReadLineTest_data()
{
  buildParserTestData();
}

void CsvParserTest::stringParserReadAllTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedData);
  QFETCH(bool, expectedOk);
  QFETCH(CsvParserSettings, csvSettings);
  CsvStringParser parser;
  Mdt::Expected<StringRecordList> recList;
  /*
   * Initial state
   */
  QVERIFY(parser.atEnd());
  // Setup CSV source string and parser
  parser.setCsvSettings(csvSettings);
  parser.setSource(sourceData);
  // Parse the entires string
  recList = parser.readAll();
  if(expectedOk){
    QVERIFY(recList.hasValue());
  }else{
    QVERIFY(recList.hasError());
    return;
  }
  QVERIFY(recList.hasValue());
  const auto data = recList.value();
  // Check
  QCOMPARE(data.rowCount(), expectedData.rowCount());
  for(int row = 0; row < data.rowCount(); ++row){
    QCOMPARE(data.columnCount(row), expectedData.columnCount(row));
    for(int col = 0; col < data.columnCount(row); ++col){
      QCOMPARE(data.data(row, col), expectedData.data(row, col));
    }
  }
}

void CsvParserTest::stringParserReadAllTest_data()
{
  buildParserTestData();
}

void CsvParserTest::fileParserReadLineTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedData);
  QFETCH(bool, expectedOk);
  QFETCH(CsvParserSettings, csvSettings);
  CsvFileParser parser;
  Mdt::Expected<StringRecord> record;
  StringRecordList data;
  QTemporaryFile file;
  QTextCodec *codec;
  QByteArray rawFileData;

  /*
   * Prepare file
   */
  codec = QTextCodec::codecForName("UTF-8");
  QVERIFY(codec != nullptr);
  rawFileData = codec->fromUnicode(sourceData);
  QVERIFY(file.open());
  QVERIFY(file.write(rawFileData) >= rawFileData.size());
  file.close();
  /*
   * Initial state
   */
  QVERIFY(!parser.isOpen());
  QVERIFY(parser.atEnd());
  // Setup CSV parser
  parser.setCsvSettings(csvSettings);
  QVERIFY(parser.openFile(file.fileName(), "UTF-8"));
  QVERIFY(parser.isOpen());
  // Parse line by line
  while(!parser.atEnd()){
    record = parser.readLine();
    if(expectedOk){
      QVERIFY(record.hasValue());
      data.appendRecord(record.value());
    }else{
      QVERIFY(record.hasError());
    }
  }
  // Close
  parser.closeFile();
  QVERIFY(!parser.isOpen());
  // Check
  QCOMPARE(data.rowCount(), expectedData.rowCount());
  for(int row = 0; row < data.rowCount(); ++row){
    QCOMPARE(data.columnCount(row), expectedData.columnCount(row));
    for(int col = 0; col < data.columnCount(row); ++col){
      QCOMPARE(data.data(row, col), expectedData.data(row, col));
    }
  }
}

void CsvParserTest::fileParserReadLineTest_data()
{
  buildParserTestData();
}

void CsvParserTest::fileParserReadAllTest()
{
  QFETCH(QString, sourceData);
  QFETCH(StringRecordList, expectedData);
  QFETCH(bool, expectedOk);
  QFETCH(CsvParserSettings, csvSettings);
  CsvFileParser parser;
  QTemporaryFile file;
  QTextCodec *codec;
  QByteArray rawFileData;

  /*
   * Prepare file
   */
  codec = QTextCodec::codecForName("UTF-8");
  QVERIFY(codec != nullptr);
  rawFileData = codec->fromUnicode(sourceData);
  QVERIFY(file.open());
  QVERIFY(file.write(rawFileData) >= rawFileData.size());
  file.close();
  /*
   * Initial state
   */
  QVERIFY(!parser.isOpen());
  QVERIFY(parser.atEnd());
  // Setup CSV parser
  parser.setCsvSettings(csvSettings);
  QVERIFY(parser.openFile(file.fileName(), "UTF-8"));
  QVERIFY(parser.isOpen());
  // Parse the file
  const auto recList = parser.readAll();
  if(expectedOk){
    QVERIFY(recList.hasValue());
  }else{
    QVERIFY(recList.hasError());
    return;
  }
  QVERIFY(recList.hasValue());
  const auto data = recList.value();
  // Close
  parser.closeFile();
  QVERIFY(!parser.isOpen());
  // Check
  QCOMPARE(data.rowCount(), expectedData.rowCount());
  for(int row = 0; row < data.rowCount(); ++row){
    QCOMPARE(data.columnCount(row), expectedData.columnCount(row));
    for(int col = 0; col < data.columnCount(row); ++col){
      QCOMPARE(data.data(row, col), expectedData.data(row, col));
    }
  }
}

void CsvParserTest::fileParserReadAllTest_data()
{
  buildParserTestData();
}

void CsvParserTest::buildParserTestData()
{
  QTest::addColumn<QString>("sourceData");
  QTest::addColumn<StringRecordList>("expectedData");
  QTest::addColumn<bool>("expectedOk");
  QTest::addColumn<CsvParserSettings>("csvSettings");

  QString sourceData;
  StringRecordList expectedData;
  CsvParserSettings csvSettings;
  const bool Ok = true;
  const bool Nok = false;

  /*
   * Tests are generally based on CSV-1203 standard.
   * Some exceptions also comes from RFC 4180
   */

  /*
   * Empty CSV source
   * Parser should simply return a empty result.
   * (the user of the library can then choose to simply display/process nothing,
   *  or warn the user of his application about empty source)
   */
  sourceData = "";
  expectedData.clear();
  QTest::newRow("Empty(,|FP:None|EOL:None") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Simple CSV tests:
   *  - Tests witout field protection
   *  - Test with EOL: \n and \r\n
   */
  csvSettings.setFieldSeparator(',');
  csvSettings.setFieldProtection('"');
  csvSettings.setParseExp(false);
  // Single char CSV - No EOL (No EOL is allowed by RFC 4180)
  sourceData = "A";
  expectedData = {{"A"}};
  QTest::newRow("1 char(,|FP:None|EOL:None") << sourceData << expectedData << Ok << csvSettings;
  // Single char CSV - \n EOL
  sourceData = "A\n";
  expectedData = {{"A"}};
  QTest::newRow("1 char(,|FP:None|EOL:\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single char CSV - \n EOL
  sourceData = "A\r\n";
  expectedData = {{"A"}};
  QTest::newRow("1 char(,|FP:None|EOL:\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - No EOL (No EOL is allowed by RFC 4180)
  sourceData = "A,B,C,D";
  expectedData = {{"A","B","C","D"}};
  QTest::newRow("A,B,C,D") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - \n EOL
  sourceData = "A,B,C,D\n";
  expectedData = {{"A","B","C","D"}};
  QTest::newRow("A,B,C,D\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (more than 1 char per field) - \n EOL
  sourceData = "ABCD\n";
  expectedData = {{"ABCD"}};
  QTest::newRow("ABCD\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (more than 1 char per field) - \n EOL
  sourceData = "ABCD,EFGH\n";
  expectedData = {{"ABCD","EFGH"}};
  QTest::newRow("ABCD,EFGH\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n";
  expectedData = {{"A","B","C","D"}};
  QTest::newRow("A,B,C,D\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with empty parts
  sourceData = "A,,B,C\r\n";
  expectedData = {{"A","","B","C"}};
  QTest::newRow("A,,B,C\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with empty parts (at end)
  sourceData = "A,,B,\r\n";
  expectedData = {{"A","","B",""}};
  QTest::newRow("A,,B,\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with more than 1 char per field and empty parts
  sourceData = "AB,,C,D\r\n";
  expectedData = {{"AB","","C","D"}};
  QTest::newRow("AB,,C,D\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  // 2 line CSV - \n EOL
  sourceData = "A,B,C,D\n1,2,3,4\n";
  expectedData = {{"A","B","C","D"},{"1","2","3","4"}};
  QTest::newRow("A,B,C,D\\n1,2,3,4\\n") << sourceData << expectedData << Ok << csvSettings;
  // 2 line CSV - \r\n EOL
  sourceData = "A,B,C,D\r\n1,2,3,4\r\n";
  expectedData = {{"A","B","C","D"},{"1","2","3","4"}};
  QTest::newRow("A,B,C,D\\r\\n1,2,3,4\\r\\n") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Non quoted with spaces
   */
  // 1 line of single char
  /// \todo check if error should be reported
  sourceData = "A ";
  expectedData = {{"A "}};
  QTest::newRow("A ") << sourceData << expectedData << Ok << csvSettings;
  // 1 line of many chars
  /// \todo check if error should be reported
  sourceData = "AB ";
  expectedData = {{"AB "}};
  QTest::newRow("AB ") << sourceData << expectedData << Ok << csvSettings;
  // 1 line of many chars
  /// \todo check if error should be reported
  sourceData = "A B";
  expectedData = {{"A B"}};
  QTest::newRow("A B") << sourceData << expectedData << Ok << csvSettings;
  // 2 lines of single char
  /// \todo check if error should be reported
  sourceData = "A \nB";
  expectedData = {{"A "},{"B"}};
  QTest::newRow("A \\nB") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Quoted CSV tests:
   *  - Test with EOL: \n and \r\n
   */
  csvSettings.setFieldSeparator(',');
  csvSettings.setFieldProtection('"');
  csvSettings.setParseExp(false);
  // Single char CSV
  sourceData = "\"A\"\n";
  expectedData = {{"A"}};
  QTest::newRow("\"A\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV
  sourceData = "\"ABCD\"\n";
  expectedData = {{"ABCD"}};
  QTest::newRow("\"ABCD\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV with quoted parts
  sourceData = "\"A\"\"BC\"\"D\"\n";
  expectedData = {{"A\"BC\"D"}};
  QTest::newRow("\"A\"\"BC\"\"D\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV
  sourceData = "\"A\",\"B\"\n";
  expectedData = {{"A","B"}};
  QTest::newRow("\"A\",\"B\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with empty parts
  sourceData = "\"A\",\"\",\"B\",\"C\"\n";
  expectedData = {{"A","","B","C"}};
  QTest::newRow("\"A\",\"\",\"B\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with empty parts (at end)
  sourceData = "\"A\",\"\",\"B\",\"\"\n";
  expectedData = {{"A","","B",""}};
  QTest::newRow("\"A\",\"\",\"B\",\"\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV (with many charachter strings)
  sourceData = "\"ABCD\",\"EFGH\"\n";
  expectedData = {{"ABCD","EFGH"}};
  QTest::newRow("\"ABCD\",\"EFGH\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\nB\",\"C\"\n";
  expectedData = {{"A\nB","C"}};
  QTest::newRow("\"A\\nB\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with EOL in quoted field
  sourceData = "\"A\r\nB\",\"C\"\n";
  expectedData = {{"A\r\nB","C"}};
  QTest::newRow("\"A\\r\\nB\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with , in quoted field
  sourceData = "\"A,B\",\"C\"\n";
  expectedData = {{"A,B","C"}};
  QTest::newRow("\"A,B\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line CSV with double quoted string in quoted field
  sourceData = "\"A\"\"B\"\"\",\"C\"\n";
  expectedData = {{"A\"B\"","C"}};
  QTest::newRow("\"A\"\"B\"\"\",\"C\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single line with more than 1 char per field and empty parts
  sourceData = "\"AB\",\"\",\"C\",\"D\"\n";
  expectedData = {{"AB","","C","D"}};
  QTest::newRow("\"AB\",\"\",\"C\",\"D\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // RFC 4180 escape quote. We are not conform, but check that this does not crasch
  /// \todo Determine if this should generate a error or not
  sourceData = "\"A\"\"B\"\n";
  expectedData = {{"A\"B"}};
  QTest::newRow("\"A\"\"B\"\\n") << sourceData << expectedData << Nok << csvSettings;
  /*
   * Quoted with spaces
   */
  // 1 line of single char
  sourceData = "\"A \"";
  expectedData = {{"A "}};
  QTest::newRow("\"A \"") << sourceData << expectedData << Ok << csvSettings;
  // 1 line of many chars
  sourceData = "\"AB \"";
  expectedData = {{"AB "}};
  QTest::newRow("\"AB \"") << sourceData << expectedData << Ok << csvSettings;
  // 1 line of many chars
  sourceData = "\"A B\"";
  expectedData = {{"A B"}};
  QTest::newRow("\"A B\"") << sourceData << expectedData << Ok << csvSettings;
  // 2 lines of single char
  sourceData = "\"A \"\n\"B\"";
  expectedData = {{"A "},{"B"}};
  QTest::newRow("\"A \"\\n\"B\"") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Excel protection marker CSV tests
   */
  csvSettings.setFieldSeparator(',');
  csvSettings.setFieldProtection('"');
  csvSettings.setParseExp(true);
  // Single char CSV
  sourceData = "~A\n";
  expectedData = {{"A"}};
  QTest::newRow("~A\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single string CSV
  sourceData = "~ABCD\n";
  expectedData = {{"ABCD"}};
  QTest::newRow("~ABCD\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single CSV line
  sourceData = "~ABCD,EFGH\n";
  expectedData = {{"ABCD","EFGH"}};
  QTest::newRow("~ABCD,EFGH\\n") << sourceData << expectedData << Ok << csvSettings;
  // Single quoted CSV line
  sourceData = "\"~ABCD\",\"EFGH\"\n";
  expectedData = {{"ABCD","EFGH"}};
  QTest::newRow("\"~ABCD\",\"EFGH\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Do not parse EXP
  csvSettings.setParseExp(false);
  // Single CSV line
  sourceData = "~ABCD,EFGH\n";
  expectedData = {{"~ABCD","EFGH"}};
  QTest::newRow("~ABCD,EFGH\\n(EXP ignored)") << sourceData << expectedData << Ok << csvSettings;
  // Single quoted CSV line
  sourceData = "\"~ABCD\",\"EFGH\"\n";
  expectedData = {{"~ABCD","EFGH"}};
  QTest::newRow("\"~ABCD\",\"EFGH\"\\n(EXP ignored)") << sourceData << expectedData << Ok << csvSettings;
  /*
   * Some tests witch non ASCII strings:
   * Note: only edit this part with a UTF-8 editor !
   */
  csvSettings.setFieldSeparator(',');
  csvSettings.setFieldProtection('"');
  csvSettings.setParseExp(true);
  // Non quoted
  sourceData = u8"A,é,à,B,è,ü,ö,ä,\u03B1,\u012C\n";
  expectedData = {{"A","é","à","B","è","ü","ö","ä",u8"\u03B1",u8"\u012C"}};
  QTest::newRow(u8"A,é,à,B,è,ü,ö,ä,\u03B1,\u012C\\n") << sourceData << expectedData << Ok << csvSettings;
  // Non quoted
  sourceData = u8"A,éà,Bèü,öä\u03B1\u012C\n";
  expectedData = {{"A",u8"éà",u8"Bèü",u8"öä\u03B1\u012C"}};
  QTest::newRow(u8"A,éà,Bèü,öä\u03B1\u012C\\n") << sourceData << expectedData << Ok << csvSettings;
  // Quoted
  sourceData = u8"\"A\",\"é\",\"à\",\"B\",\"è\",\"ü\",\"ö\",\"ä\",\"\u03B1\"\n";
  expectedData = {{"A","é","à","B","è","ü","ö","ä",u8"\u03B1"}};
  QTest::newRow(u8"\"A\",\"é\",\"à\",\"B\",\"è\",\"ü\",\"ö\",\"ä\",\"\u03B1\"\\n") << sourceData << expectedData << Ok << csvSettings;
  // Quoted
  sourceData = u8"\"A,é\",\"à,B\",\"è,ü\",\"ö,ä,\u03B1\"\n";
  expectedData = {{u8"A,é",u8"à,B",u8"è,ü",u8"ö,ä,\u03B1"}};
  QTest::newRow(u8"\"A,é\",\"à,B\",\"è,ü\",\"ö,ä,\u03B1\"\\n") << sourceData << expectedData << Ok << csvSettings;

  /// \todo add casese from: http://stackoverflow.com/questions/7436481/how-to-make-my-split-work-only-on-one-real-line-and-be-capable-to-skip-quoted-pa/7462539#7462539

  /// \todo Add some from ~/programmation/c_cpp/libreoffiche/git/core/sc/qa/unit/data/contentCSV/
  
  /// \todo Check if A B,C\n should be valid (space between A and B, unquoted)

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  CsvParserTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
