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
#include "mdtFileTest.h"
#include "mdtCsvFile.h"
#include "mdtPartitionAttributes.h"
#include "mdtFileCopier.h"
#include "mdtApplication.h"
#include <QTemporaryFile>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QCryptographicHash>

#include <QDebug>

// We need a list of byte array for readLine data results
typedef QList<QByteArray> byteArrayList;
Q_DECLARE_METATYPE(byteArrayList);

void mdtFileTest::csvFileSettingsTest()
{
  mdtCsvFileSettings s;

  /*
   * Initial settings
   */
  QCOMPARE(s.separator, QString(","));
  QCOMPARE(s.eol, QString("\r\n"));
  QCOMPARE(s.eol.at(0), QChar(0x0D));
  QCOMPARE(s.eol.at(1), QChar(0x0A));
  QVERIFY(s.dataProtection.isEmpty());
  QVERIFY(s.dataProtectionEscapeChar.isNull());
  QVERIFY(s.comment.isEmpty());

  /*
   * Some setup..
   */
  s.separator = ";";
  s.eol = "FAKEEOL";
  s.dataProtection = "\"";
  s.dataProtectionEscapeChar = '\\';
  s.comment = "#";
  QCOMPARE(s.separator, QString(";"));
  QCOMPARE(s.eol, QString("FAKEEOL"));
  QCOMPARE(s.dataProtection, QString("\""));
  QCOMPARE(s.dataProtectionEscapeChar, QChar('\\'));
  QCOMPARE(s.comment, QString("#"));

  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.separator, QString(","));
  QCOMPARE(s.eol, QString("\r\n"));
  QCOMPARE(s.eol.at(0), QChar(0x0D));
  QCOMPARE(s.eol.at(1), QChar(0x0A));
  QVERIFY(s.dataProtection.isEmpty());
  QVERIFY(s.dataProtectionEscapeChar.isNull());
  QVERIFY(s.comment.isEmpty());
}


void mdtFileTest::csvFileWriteTest()
{
  QTemporaryFile tmp;
  mdtCsvFile csv;
  mdtCsvFileSettings settings;
  QStringList line;
  QByteArray data;
  QByteArray refData;

  // Create a temporary file
  QVERIFY(tmp.open());
  tmp.close();

  // Create a CSV file
  csv.setFileName(tmp.fileName());

  // Write some simple data - Use native EOL (note: file open in text mode, setting is not relevant)
  QVERIFY(csv.open(QIODevice::WriteOnly | QIODevice::Text));
  line << "A" << "B" << "C" << "D";
  ///QVERIFY(csv.writeLine(line, ";"));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "A;B;C;D";
  refData = "A,B,C,D";
  refData += MDT_NATIVE_EOL;
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();

  // Write some simple data - Use \n EOL
  settings.eol = "\n";
  QVERIFY(csv.open(QIODevice::WriteOnly));
  line << "1" << "2" << "3" << "4";
  ///QVERIFY(csv.writeLine(line, ";", "", '\0', "\n"));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "1;2;3;4";
  refData = "1,2,3,4";
  refData += "\n";
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();

  // Write some simple data - Use \r\n EOL (witch is CSV default, regarding RFC 4180)
  settings.clear();
  QVERIFY(csv.open(QIODevice::WriteOnly));
  line << "E" << "F" << "G" << "H";
  ///QVERIFY(csv.writeLine(line, ";", "", '\0', "\r\n"));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "E;F;G;H";
  refData = "E,F,G,H";
  refData += "\r\n";
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();

  // Check data protection function - Use native EOL
  settings.clear();
  settings.dataProtection = "'";
  settings.dataProtectionEscapeChar = '\\';
  QVERIFY(csv.open(QIODevice::WriteOnly | QIODevice::Text));
  line << "A" << "B" << "C" << "D";
  ///QVERIFY(csv.writeLine(line, ";", "'", '\\'));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "'A';'B';'C';'D'";
  refData = "'A','B','C','D'";
  refData += MDT_NATIVE_EOL;
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();

  // Check escape function - Use native EOL
  settings.clear();
  settings.dataProtection = "'";
  settings.dataProtectionEscapeChar = '\\';
  QVERIFY(csv.open(QIODevice::WriteOnly | QIODevice::Text));
  line << "'A'" << "B" << "'C'" << "D";
  ///QVERIFY(csv.writeLine(line, ";", "'", '\\'));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "'\\'A\\'';'B';'\\'C\\'';'D'";
  refData = "'\\'A\\'','B','\\'C\\'','D'";
  refData += MDT_NATIVE_EOL;
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();

  // Write 2 lines of simple data - Use native EOL
  settings.clear();
  QVERIFY(csv.open(QIODevice::WriteOnly | QIODevice::Text));
  line << "A" << "B" << "C" << "D";
  ///QVERIFY(csv.writeLine(line, ";"));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  line << "1" << "2" << "3" << "4";
  ///QVERIFY(csv.writeLine(line, ";"));
  QVERIFY(csv.writeLine(line, settings));
  line.clear();
  csv.close();
  // Check written data
  ///refData = "A;B;C;D";
  refData = "A,B,C,D";
  refData += MDT_NATIVE_EOL;
  ///refData += "1;2;3;4";
  refData += "1,2,3,4";
  refData += MDT_NATIVE_EOL;
  QVERIFY(tmp.open());
  QCOMPARE(QString(tmp.readAll()), QString(refData));
  tmp.close();
}

void mdtFileTest::csvFileReadLineTest()
{
  QTemporaryFile tmp;
  mdtCsvFile csv;
  int bufferSize, i;
  QFETCH(QByteArray, fileData);
  QFETCH(QString, dataProtection);
  QFETCH(QString, comment);
  QFETCH(QChar, escapeChar);
  QFETCH(QByteArray, eol);
  QFETCH(byteArrayList, result);

  // Create a temporary file
  QVERIFY(tmp.open());
  QVERIFY(!tmp.isTextModeEnabled());

  // Write test data
  if(fileData.size() > 0){
    QVERIFY(tmp.write(fileData));
  }
  tmp.close();

  QVERIFY(tmp.open());
  tmp.close();
  // Check readLine() method with different buffer sizes
  for(bufferSize = 1; bufferSize < 17; bufferSize++){
    // Read file and verify data
    csv.setFileName(tmp.fileName());
    csv.setReadLineBufferSize(bufferSize);
    QVERIFY(csv.open(QIODevice::ReadOnly));
    i=0;
    while(csv.hasMoreLines()){
      QVERIFY(i < result.size());
      QCOMPARE(csv.readLine(dataProtection, comment, escapeChar, eol), result.at(i));
      i++;
    }
    QCOMPARE(i, result.size());
    csv.close();
  }
}

void mdtFileTest::csvFileReadLineTest_data()
{
  QTest::addColumn<QByteArray>("fileData");
  QTest::addColumn<QString>("dataProtection");
  QTest::addColumn<QString>("comment");
  QTest::addColumn<QChar>("escapeChar");
  QTest::addColumn<QByteArray>("eol");
  QTest::addColumn<byteArrayList>("result");

  QByteArray fileData;
  QString dataProtection;
  QString comment;
  QChar escapeChar;
  QByteArray eol;
  byteArrayList result;

  /*
   * Tests without data protection, without escape char
   * EOL: \n
   */
  dataProtection = "";
  escapeChar = '\0';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "B";
  result.clear();
  result << "B";
  QTest::newRow("1 char miss. EOL (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n";
  result.clear();
  result << "ABCD";
  QTest::newRow("1 line (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD";
  result.clear();
  result << "ABCD";
  QTest::newRow("1 line miss. EOL (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (-,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests without data protection, without escape char
   * EOL: \r\n
   */
  dataProtection = "";
  escapeChar = '\0';
  eol = "\r\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\r\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "B";
  result.clear();
  result << "B";
  QTest::newRow("1 char miss. EOL (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\n";
  result.clear();
  result << "ABCD";
  QTest::newRow("1 line (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD";
  result.clear();
  result << "ABCD";
  QTest::newRow("1 line miss. EOL (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH\r\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    fileData += result.at(i);
    fileData += "\r\n";
  }
  QTest::newRow("n lines (-,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests without escape char
   * dataProtection: '
   * EOL: \n
   */
  dataProtection = "'";
  escapeChar = '\0';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "''\n";
  result.clear();
  result << "''";
  QTest::newRow("Only DP (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'A'\n";
  result.clear();
  result << "'A'";
  QTest::newRow("1 char (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\nEFGH\n";
  result.clear();
  result << "'ABCD'" << "EFGH";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n'EFGH'\n";
  result.clear();
  result << "ABCD" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\n'EFGH'\n";
  result.clear();
  result << "'ABCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'AB\nCD'\n'EFGH'\n";
  result.clear();
  result << "'AB\nCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\n'EF\nGH'\n";
  result.clear();
  result << "'ABCD'" << "'EF\nGH'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'\nABCD'\n'EFGH'\n";
  result.clear();
  result << "'\nABCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\n'EFGH\n'\n";
  result.clear();
  result << "'ABCD'" << "'EFGH\n'";
  QTest::newRow("2 lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("'ABCDEFGHIJKLMNOPQRSTUVWXYZ'");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("'ABC\nDEFG\nHIJKLMNOPQRSTUVWXYZ'");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (',-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests without escape char
   * dataProtection: '
   * EOL: \r\n
   */
  dataProtection = "'";
  escapeChar = '\0';
  eol = "\r\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\r\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH\r\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "''\r\n";
  result.clear();
  result << "''";
  QTest::newRow("Only DP (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'A'\r\n";
  result.clear();
  result << "'A'";
  QTest::newRow("1 char (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\r\nEFGH\r\n";
  result.clear();
  result << "'ABCD'" << "EFGH";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\n'EFGH'\r\n";
  result.clear();
  result << "ABCD" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\r\n'EFGH'\r\n";
  result.clear();
  result << "'ABCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'AB\r\nCD'\r\n'EFGH'\r\n";
  result.clear();
  result << "'AB\r\nCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\r\n'EF\r\nGH'\r\n";
  result.clear();
  result << "'ABCD'" << "'EF\r\nGH'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'\r\nABCD'\r\n'EF\r\nGH'\r\n";
  result.clear();
  result << "'\r\nABCD'" << "'EF\r\nGH'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\r\n'EFGH\r\n'\r\n";
  result.clear();
  result << "'ABCD'" << "'EFGH\r\n'";
  QTest::newRow("2 lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("'ABCDEFGHIJKLMNOPQRSTUVWXYZ'");
    fileData += result.at(i);
    fileData += "\r\n";
  }
  QTest::newRow("n lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("'ABC\r\nDEFGHIJ\r\nKLMNOPQRSTUVWXYZ'");
    fileData += result.at(i);
    fileData += "\r\n";
  }
  QTest::newRow("n lines (',-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests without escape char
   * dataProtection: <DP>
   * EOL: \n
   */
  dataProtection = "<DP>";
  escapeChar = '\0';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP><DP>\n";
  result.clear();
  result << "<DP><DP>";
  QTest::newRow("Only DP (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>A<DP>\n";
  result.clear();
  result << "<DP>A<DP>";
  QTest::newRow("1 char (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\nEFGH\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "EFGH";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n<DP>EFGH<DP>\n";
  result.clear();
  result << "ABCD" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\n<DP>EFGH<DP>\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>AB\nCD<DP>\n<DP>EFGH<DP>\n";
  result.clear();
  result << "<DP>AB\nCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\n<DP>EF\nGH<DP>\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EF\nGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>\nABCD<DP>\n<DP>EFGH<DP>\n";
  result.clear();
  result << "<DP>\nABCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\n<DP>EFGH\n<DP>\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EFGH\n<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("<DP>ABCDEFGHIJKLMNOPQRSTUVWXYZ<DP>");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("<DP>ABC\nDEFG\nHIJKLMNOPQRSTUVWXYZ<DP>");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (<DP>,-,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests without escape char
   * dataProtection: <DP>
   * EOL: \r\n
   */
  dataProtection = "<DP>";
  escapeChar = '\0';
  eol = "\r\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\r\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH\r\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\nEFGH";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines miss. EOL (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP><DP>\r\n";
  result.clear();
  result << "<DP><DP>";
  QTest::newRow("Only DP (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>A<DP>\r\n";
  result.clear();
  result << "<DP>A<DP>";
  QTest::newRow("1 char (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\r\nEFGH\r\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "EFGH";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\r\n<DP>EFGH<DP>\r\n";
  result.clear();
  result << "ABCD" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\r\n<DP>EFGH<DP>\r\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>AB\r\nCD<DP>\r\n<DP>EFGH<DP>\r\n";
  result.clear();
  result << "<DP>AB\r\nCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\r\n<DP>EF\r\nGH<DP>\r\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EF\r\nGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>\r\nABCD<DP>\r\n<DP>EFGH<DP>\r\n";
  result.clear();
  result << "<DP>\r\nABCD<DP>" << "<DP>EFGH<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "<DP>ABCD<DP>\r\n<DP>EFGH\r\n<DP>\r\n";
  result.clear();
  result << "<DP>ABCD<DP>" << "<DP>EFGH\r\n<DP>";
  QTest::newRow("2 lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("<DP>ABCDEFGHIJKLMNOPQRSTUVWXYZ<DP>");
    fileData += result.at(i);
    fileData += "\r\n";
  }
  QTest::newRow("n lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("<DP>ABC\r\nDEFG\r\nHIJKLMNOPQRSTUVWXYZ<DP>");
    fileData += result.at(i);
    fileData += "\r\n";
  }
  QTest::newRow("n lines (<DP>,-,-,\\r\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests
   * dataProtection: '
   * escapeChar: \
   * EOL: \n
   */
  dataProtection = "'";
  escapeChar = '\\';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "''\n";
  result.clear();
  result << "''";
  QTest::newRow("1 char (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "\\'\\'\n";
  result.clear();
  result << "\\'\\'";
  QTest::newRow("1 char (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'ABCD'\n'EFGH'\n";
  result.clear();
  result << "'ABCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'AB\\'CD'\n'EFGH'\n";
  result.clear();
  result << "'AB\\'CD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'AB\nCD'\n'EFGH'\n";
  result.clear();
  result << "'AB\nCD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\\'B\nCD\nEFGH\n";
  result.clear();
  result << "A\\'B" << "CD" << "EFGH";
  QTest::newRow("3 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\\'B\\'\nCD\nEFGH\n";
  result.clear();
  result << "A\\'B\\'" << "CD" << "EFGH";
  QTest::newRow("3 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\\'B\n\\'\nCD\nEFGH\n";
  result.clear();
  result << "A\\'B" << "\\'" << "CD" << "EFGH";
  QTest::newRow("4 lines (',-,\\,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Tests
   * dataProtection: '
   * escapeChar: '
   * EOL: \n
   */
  dataProtection = "'";
  escapeChar = '\'';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (',-,',\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (',-,',\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "''\n";
  result.clear();
  result << "''";
  QTest::newRow("1 char (',-,',\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "''''\n";
  result.clear();
  result << "''''";
  QTest::newRow("1 char (',-,',\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "'AB''CD'\n'EFGH'\n";
  result.clear();
  result << "'AB''CD'" << "'EFGH'";
  QTest::newRow("2 lines (',-,',\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  /*
   * Check commented lines
   */
  dataProtection = "";
  comment = "#";
  escapeChar = '\0';
  eol = "\n";

  fileData = "";
  result.clear();
  QTest::newRow("Empty file (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A\n";
  result.clear();
  result << "A";
  QTest::newRow("1 char (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#\n";
  result.clear();
  result << "";
  QTest::newRow("1 char (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#A\n";
  result.clear();
  result << "";
  QTest::newRow("1 char (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "B";
  result.clear();
  result << "B";
  QTest::newRow("1 char miss. EOL (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#";
  result.clear();
  result << "";
  QTest::newRow("1 char miss. EOL (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#B";
  result.clear();
  result << "";
  QTest::newRow("1 char miss. EOL (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n";
  result.clear();
  result << "ABCD";
  QTest::newRow("1 line (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#ABCD\n";
  result.clear();
  result << "";
  QTest::newRow("1 line (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "#ABCD";
  result.clear();
  result << "";
  QTest::newRow("1 line miss. EOL (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\nEFGH\n";
  result.clear();
  result << "ABCD" << "EFGH";
  QTest::newRow("2 lines (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n#EFGH\n";
  result.clear();
  result << "ABCD" << "";
  QTest::newRow("2 lines (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "A#BCD\nEFGH#\n";
  result.clear();
  result << "A#BCD" << "EFGH#";
  QTest::newRow("2 lines (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  fileData = "ABCD\n#EFGH";
  result.clear();
  result << "ABCD" << "";
  QTest::newRow("2 lines miss. EOL (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

  result.clear();
  fileData.clear();
  for(int i=0; i<10; i++){
    result.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    fileData += result.at(i);
    fileData += "\n";
  }
  QTest::newRow("n lines (-,#,-,\\n)") << fileData << dataProtection  << comment << escapeChar << eol << result;

}

void mdtFileTest::csvFileReadTest()
{
  QTemporaryFile tmp, tmpErr, tmpComment, tmpEscape, tmpW;
  mdtCsvFile csv;
  QStringList line;
  QList<QStringList> data;
  QList<QStringList> refData;
  QString str;
  QStringList refHeaders;

  // Get data
  QFETCH(QStringList, refLine1);
  QFETCH(QStringList, refLine2);
  QFETCH(QStringList, refLine3);
  QFETCH(QString, separator);
  QFETCH(QString, dataProtection);

  // Create a temporary file
  QVERIFY(tmp.open());

  // Create refData
  refData << refLine1;
  refData << refLine2;
  refData << refLine3;

  // Copy refData to data and add separator + dataProtection
  for(int i=0; i<refData.size(); i++){
    line.clear();
    for(int j=0; j<refData.at(i).size(); j++){
      // Create line
      line << dataProtection << refData.at(i).at(j) << dataProtection;
      // Add separator/end of line
      if((j>=0)&&(j<(refData.at(i).size()-1))){
        line << separator;
      }else{
        line << MDT_NATIVE_EOL;
      }
    }
    data << line;
  }
  // Write data
  for(int i=0; i<data.size(); i++){
    for(int j=0; j<data.at(i).size(); j++){
      QVERIFY(tmp.write(data.at(i).at(j).toLocal8Bit()) == (qint64)data.at(i).at(j).toLocal8Bit().size());
    }
  }
  // Close tmp file
  tmp.close();

  // Create a CSV file
  csv.setFileName(tmp.fileName());
  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));

  // Read file and verify data
  QVERIFY(csv.readLines(separator.toLocal8Bit(), dataProtection.toLocal8Bit()));
  for(int i=0; i<refData.size(); i++){
    for(int j=0; j<refData.at(i).size(); j++){
      QCOMPARE(csv.valueAt(i, j), refData.at(i).at(j));
    }
  }
  csv.close();

  // Create a second new temp file for following tests
  QVERIFY(tmpErr.open());

  // Set CSV file name
  csv.setFileName(tmpErr.fileName());

  /*
   * Combining data test
   */

  // Test with combining data protection and no data protection
  QVERIFY(tmpErr.write("Valid data;'with data ; protection';and without\n"));
  tmpErr.close();

  // Read file and verify data
  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));
  QVERIFY(csv.readLines(";", "'"));
  QCOMPARE(csv.valueAt(0, 0) , QString("Valid data"));
  QCOMPARE(csv.valueAt(0, 1) , QString("with data ; protection"));
  QCOMPARE(csv.valueAt(0, 2) , QString("and without"));
  csv.close();

  /*
   * Invalid data test
   */

  // Test with invalid data (no infinit loop + must not crash) - 5 fields
  QVERIFY(tmpErr.open());
  QVERIFY(tmpErr.write("Some invalid;data ' Only one data protection\nAnd 2 seps;;With other data and no end of line at end"));
  tmpErr.close();
  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));
  // Read file and verify data
  QVERIFY(csv.readLines(";", "'"));
  /**
  QCOMPARE(csv.valueAt(0, 0) , QString("Some invalid"));
  QCOMPARE(csv.valueAt(1, 0) , QString("And 2 seps"));
  QCOMPARE(csv.valueAt(1, 1) , QString(""));
  QCOMPARE(csv.valueAt(1, 2) , QString("With other data and no end of line at end"));
  */

  // Access invalid indexes - Must not crash, and value must be a empty string
  QVERIFY(csv.valueAt(-1, 0) == "");
  QVERIFY(csv.valueAt(0, -1) == "");
  QVERIFY(csv.valueAt(-1, -1) == "");
  QVERIFY(csv.valueAt(2000, 0) == "");
  QVERIFY(csv.valueAt(0, 2000) == "");
  QVERIFY(csv.valueAt(2000, 2000) == "");
  csv.close();

  /*
   * Commented line test
   */

  // Create another new temp file for following tests
  QVERIFY(tmpComment.open());

  // Set CSV file name
  csv.setFileName(tmpComment.fileName());

  // Write data to tmp file
  QVERIFY(tmpComment.write("Uncommented data;'Field with protection';0123\n"));
  QVERIFY(tmpComment.write("# Commented data;'Field with protection';'ABCD'\n"));
  QVERIFY(tmpComment.write(" # Commented data with space;'Field with protection';'456'\n"));
  QVERIFY(tmpComment.write("Uncommented data;Some data;'987'\n"));
  tmpComment.close();

  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));
  // Read file and verify data
  QVERIFY(csv.readLines(";", "'", "#"));
  QCOMPARE(csv.valueAt(0, 0) , QString("Uncommented data"));
  QCOMPARE(csv.valueAt(0, 1) , QString("Field with protection"));
  QCOMPARE(csv.valueAt(0, 2) , QString("0123"));
  QCOMPARE(csv.valueAt(1, 0) , QString("Uncommented data"));
  QCOMPARE(csv.valueAt(1, 1) , QString("Some data"));
  QCOMPARE(csv.valueAt(1, 2) , QString("987"));
  csv.close();

  /*
   * Data protection and escape test
   *  DP: '
   *  Escape: \
   */
  QVERIFY(tmpEscape.open());
  QVERIFY(tmpEscape.write("ABCD;'EFGH'\n"));
  QVERIFY(tmpEscape.write("'12\\'34';'5678'\n"));
  tmpEscape.close();
  // Open CSV file and check
  csv.setFileName(tmpEscape.fileName());
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QVERIFY(csv.readLines(";", "'", "#", '\\', MDT_NATIVE_EOL));
  QCOMPARE(csv.valueAt(0, 0) , QString("ABCD"));
  QCOMPARE(csv.valueAt(0, 1) , QString("EFGH"));
  QCOMPARE(csv.valueAt(1, 0) , QString("12'34"));
  QCOMPARE(csv.valueAt(1, 1) , QString("5678"));
  csv.close();

  /*
   * Data protection and escape test
   *  DP: '
   *  Escape: '
   */
  QVERIFY(tmpEscape.open());
  QVERIFY(tmpEscape.resize(0));
  QVERIFY(tmpEscape.write("ABCD;'EFGH'\n"));
  QVERIFY(tmpEscape.write("'12''34';'5678'\n"));
  tmpEscape.close();
  // Open CSV file and check
  csv.setFileName(tmpEscape.fileName());
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QVERIFY(csv.readLines(";", "'", "#", '\'', MDT_NATIVE_EOL));
  QCOMPARE(csv.valueAt(0, 0) , QString("ABCD"));
  QCOMPARE(csv.valueAt(0, 1) , QString("EFGH"));
  QCOMPARE(csv.valueAt(1, 0) , QString("12'34"));
  QCOMPARE(csv.valueAt(1, 1) , QString("5678"));
  csv.close();
  // Check header read
  refHeaders.clear();
  refHeaders << "ABCD" << "EFGH";
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QCOMPARE(csv.readHeader(";", "'", "#", '\'', MDT_NATIVE_EOL), refHeaders);
  QVERIFY(csv.readLines(";", "'", "#", '\'', MDT_NATIVE_EOL));
  QCOMPARE(csv.valueAt(0, 0) , QString("12'34"));
  QCOMPARE(csv.valueAt(0, 1) , QString("5678"));
  csv.close();

  /*
   * Check with fr Wikipedia example
   */
  QVERIFY(tmpW.open());
  QVERIFY(tmpW.write("\"Michel\";\"Durand\";\"av. de la Ferme, 89\"\n"));
  QVERIFY(tmpW.write("\"Michel \"\"Michele\"\"\";\"Durand\";\"av. de la Ferme, 89\"\n"));
  QVERIFY(tmpW.write("\"Michel;Michele\";\"Durand\";\"av. de la Ferme, 89\""));
  tmpW.close();
  // Open CSV file and check
  csv.setFileName(tmpW.fileName());
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QVERIFY(csv.readLines(";", "\"", "", '"', MDT_NATIVE_EOL));
  QCOMPARE(csv.valueAt(0, 0) , QString("Michel"));
  QCOMPARE(csv.valueAt(0, 1) , QString("Durand"));
  QCOMPARE(csv.valueAt(0, 2) , QString("av. de la Ferme, 89"));
  QCOMPARE(csv.valueAt(1, 0) , QString("Michel \"Michele\""));
  QCOMPARE(csv.valueAt(1, 1) , QString("Durand"));
  QCOMPARE(csv.valueAt(1, 2) , QString("av. de la Ferme, 89"));
  QCOMPARE(csv.valueAt(2, 0) , QString("Michel;Michele"));
  QCOMPARE(csv.valueAt(2, 1) , QString("Durand"));
  QCOMPARE(csv.valueAt(2, 2) , QString("av. de la Ferme, 89"));
  csv.close();
}

void mdtFileTest::csvFileReadTest_data()
{
  QTest::addColumn<QStringList>("refLine1");
  QTest::addColumn<QStringList>("refLine2");
  QTest::addColumn<QStringList>("refLine3");
  QTest::addColumn<QString>("separator");
  QTest::addColumn<QString>("dataProtection");

  QStringList line1;
  QStringList line2;
  QStringList line3;

  // Empty data
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "";
  line2 << "";
  line3 << "";
  QTest::newRow("Empty data, sep: ; , dp: ") << line1 << line2 << line3 << ";" << "";
  QTest::newRow("Empty data, sep: !- , dp: ") << line1 << line2 << line3 << "!-" << "";
  QTest::newRow("Empty data, sep: <data> , dp: ") << line1 << line2 << line3 << "<data>" << "";

  // Empty data
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "";
  line2 << "";
  line3 << "";
  QTest::newRow("Empty data, sep: ; , dp: \" ") << line1 << line2 << line3 << ";" << "\"";
  QTest::newRow("Empty data, sep: !- , dp: \" ") << line1 << line2 << line3 << "!-" << "\"";
  QTest::newRow("Empty data, sep: <data> , dp: \" ") << line1 << line2 << line3 << "<data>" << "\"";

  // Empty data
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "";
  line2 << "";
  line3 << "";
  QTest::newRow("Empty data, sep: ; , dp: {REM} ") << line1 << line2 << line3 << ";" << "{REM}";
  QTest::newRow("Empty data, sep: !- , dp: {REM} ") << line1 << line2 << line3 << "!-" << "{REM}";
  QTest::newRow("Empty data, sep: <data> , dp: {REM} ") << line1 << line2 << line3 << "<data>" << "{REM}";

  // Data with ; separator
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << "1" << "0123" << "JKLM";
  line3 << "JK" << "inni" << "Some data";
  QTest::newRow("Separator: ; , dp: ") << line1 << line2 << line3 << ";" << "";

  // Data with ; separator and ' data protection
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << ";1" << "0123" << "JKLM;";
  line3 << "JK" << "IJKL;MNOP" << "Some data";
  QTest::newRow("Separator: ; , dp: ' ") << line1 << line2 << line3 << ";" << "'";

  // Data with ; separator and {REM} data protection
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << ";1" << "0123" << "JKLM;";
  line3 << "JK" << "IJKL;MNOP" << "Some data";
  QTest::newRow("Separator: ; , dp: {REM} ") << line1 << line2 << line3 << ";" << "{REM}";

  // Data with <data> separator
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << "1" << "data" << "JKLM";
  line3 << "JK" << "inni" << "Some data";
  QTest::newRow("Separator: <data> , dp: ") << line1 << line2 << line3 << "<data>" << "";

  // Data with <data> separator and ' data protection
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << "1" << "data" << "JKLM";
  line3 << "JK" << "inni" << "Some data";
  QTest::newRow("Separator: <data> , dp: ' ") << line1 << line2 << line3 << "<data>" << "'";

  // Data with <data> separator and {REM} data protection
  line1.clear();
  line2.clear();
  line3.clear();
  line1 << "A" << "B" << "C" << "D";
  line2 << "" << "1" << "data" << "JKLM";
  line3 << "JK" << "inni" << "Some data";
  QTest::newRow("Separator: <data> , dp: {REM} ") << line1 << line2 << line3 << "<data>" << "{REM}";
}

void mdtFileTest::csvFileReadEolTest()
{
  QTemporaryFile tmp, tmp2;
  mdtCsvFile csv;

  // Create a temporary file
  QVERIFY(tmp.open());
  QVERIFY(!tmp.isTextModeEnabled());

  // Write a some data with \n EOL
  QVERIFY(tmp.write("A;'B';C;'D'\n"));
  QVERIFY(tmp.write("1;2;3;4\n"));
  tmp.close();

  // Read file and verify data
  csv.setFileName(tmp.fileName());
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QVERIFY(csv.readLines(";", "'", "#", '\0', "\n"));
  QCOMPARE(csv.valueAt(0, 0) , QString("A"));
  QCOMPARE(csv.valueAt(0, 1) , QString("B"));
  QCOMPARE(csv.valueAt(0, 2) , QString("C"));
  QCOMPARE(csv.valueAt(0, 3) , QString("D"));
  QCOMPARE(csv.valueAt(1, 0) , QString("1"));
  QCOMPARE(csv.valueAt(1, 1) , QString("2"));
  QCOMPARE(csv.valueAt(1, 2) , QString("3"));
  QCOMPARE(csv.valueAt(1, 3) , QString("4"));
  csv.close();

  // Create a second temporary file
  QVERIFY(tmp2.open());
  QVERIFY(!tmp2.isTextModeEnabled());

  // Write a some data with \r\n EOL
  QVERIFY(tmp2.write("E;'F';G;'H'\r\n"));
  QVERIFY(tmp2.write("5;6;7;8\r\n"));
  tmp2.close();

  // Read file and verify data
  csv.setFileName(tmp2.fileName());
  QVERIFY(csv.open(QIODevice::ReadOnly));
  QVERIFY(csv.readLines(";", "'", "#", '\0', "\r\n"));
  QCOMPARE(csv.valueAt(0, 0) , QString("E"));
  QCOMPARE(csv.valueAt(0, 1) , QString("F"));
  QCOMPARE(csv.valueAt(0, 2) , QString("G"));
  QCOMPARE(csv.valueAt(0, 3) , QString("H"));
  QCOMPARE(csv.valueAt(1, 0) , QString("5"));
  QCOMPARE(csv.valueAt(1, 1) , QString("6"));
  QCOMPARE(csv.valueAt(1, 2) , QString("7"));
  QCOMPARE(csv.valueAt(1, 3) , QString("8"));
  csv.close();
}

void mdtFileTest::mdtPartitionAttributesTest()
{
  mdtPartitionAttributes pa;
  QStringList paList, ignoreList;

  // Initial states
  QVERIFY(pa.rootPath() == "");
  QVERIFY(pa.name() == "");
  QVERIFY(pa.fileSystem() == "");

  // We make some very basic checks
#ifdef Q_OS_UNIX
  QVERIFY(pa.setPath("/"));
  QVERIFY(pa.rootPath() == "/");
  QVERIFY(!pa.setPath(""));
  QVERIFY(pa.rootPath() == "");
  QVERIFY(pa.name() == "");
  QVERIFY(pa.fileSystem() == "");
  paList = pa.availablePartitions();
  QVERIFY(paList.contains("/"));
  // Check that ignore list works
  ignoreList << "/";
  paList = pa.availablePartitions(ignoreList);
  QVERIFY(!paList.contains("/"));
#endif
#ifdef Q_OS_WIN
  QVERIFY(pa.setPath("C:\\"));
  QVERIFY(pa.rootPath() == "C:/");
  QVERIFY(pa.rootPath(true) == "C:\\");
  QVERIFY(pa.rootPath() == "C:/");
  QVERIFY(!pa.setPath(""));
  QVERIFY(pa.rootPath() == "");
  QVERIFY(pa.name() == "");
  QVERIFY(pa.fileSystem() == "");
  QVERIFY(pa.setPath("C:/"));
  QVERIFY(pa.rootPath() == "C:/");
  QVERIFY(pa.rootPath(true) == "C:\\");
  paList = pa.availablePartitions();
  QVERIFY(paList.contains("C:/"));
  // Check that ignore list works
  ignoreList << "C:/";
  paList = pa.availablePartitions(ignoreList);
  QVERIFY(!paList.contains("C:/"));
  ignoreList.clear();
  ignoreList << "c:/";
  paList = pa.availablePartitions(ignoreList);
  QVERIFY(!paList.contains("C:/"));
  // It can happen that drive list (QDir::drives() )contains not the terminal / on a drive
  // Happend with wine, drive H was returned H: , and not H:/ . Best is to handle this
  QVERIFY(pa.setPath("C:"));
  QVERIFY(pa.rootPath() == "C:/");
#endif

  paList = pa.availablePartitions();
  qDebug() << "Available partitions:";
  for(int i=0; i<paList.size(); i++){
    qDebug() << paList.at(i) << ":";
    pa.setPath(paList.at(i));
    qDebug() << " -> Name: " << pa.name() << " , rootPath: " << pa.rootPath() << " , FS: " << pa.fileSystem() << " , RD only: " << pa.isReadOnly();
  }
  
}

void mdtFileTest::mdtFileCopierTest()
{
  mdtFileCopier fc;
  QFileInfo srcFileInfo;
  QFileInfo destFileInfo;
  QTemporaryFile *srcFile, *srcFile2;
  QList<QTemporaryFile*> srcFilesList;
  QByteArray *srcData;
  QTemporaryFile *destFile, *destFile2;
  QList<QTemporaryFile*> destFilesList;
  qint64 written;
  QCryptographicHash srcHash(QCryptographicHash::Sha1);
  QCryptographicHash destHash(QCryptographicHash::Sha1);
  int i, j;
  int fileSize;
  int filesCount = 10;

  randomValueInit();
  srcData = new QByteArray;

  // Configure copy
  fc.setDirectDestOverwrite(true);
  fc.setTestMode(true);

  /*
   * Source file not found test
   */

  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  QVERIFY(srcFile->remove());
  delete srcFile;
  fc.addCopy(srcFileInfo.absoluteFilePath(), QDir::tempPath());

  // Run copy
  QVERIFY(!fc.startCopy());
  QVERIFY(!fc.waitFinished());

  /*
   * Source file is a directory
   */

  // Add a directory to copier
  fc.addCopy(QDir::homePath(), QDir::tempPath());

  // Run copy
  QVERIFY(!fc.startCopy());
  QVERIFY(!fc.waitFinished());

  /*
   * Destination file is source file
   */

  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  fc.addCopy(srcFileInfo.absoluteFilePath(), srcFileInfo.absoluteFilePath());
  fc.addCopy(srcFileInfo.absoluteFilePath(), QDir::tempPath());

  // Run copy
  QVERIFY(!fc.startCopy());
  QVERIFY(!fc.waitFinished());
  delete srcFile;

  /*
   * Combinaisons
   */

  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());

  // Create a file, delte it,  and add it to copier
  srcFile2 = new QTemporaryFile;
  QVERIFY(srcFile2->open());
  srcFileInfo.setFile(*srcFile2);
  QVERIFY(srcFile2->remove());
  delete srcFile2;
  destFile2 = new QTemporaryFile;
  QVERIFY(destFile2->open());
  destFileInfo.setFile(*destFile2);
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());

  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;
  delete destFile2;

  /*
   * Real copy with different sizes test
   * (created while trying to reproduce a strange bug with crash)
   */

  fileSize = 0;
  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  *srcData = "";
  for(i=0; i<fileSize; i++){
    srcData->append((char)randomValue(0, 255));
  }
  written = srcFile->write(*srcData);
  QVERIFY(written == (qint64)srcData->size());
  // Reopen source file
  srcFile->close();
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  srcFile->close();
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  destFile->close();
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());
  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;

  fileSize = 1;
  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  *srcData = "";
  for(i=0; i<fileSize; i++){
    srcData->append((char)randomValue(0, 255));
  }
  written = srcFile->write(*srcData);
  QVERIFY(written == (qint64)srcData->size());
  // Reopen source file
  srcFile->close();
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  srcFile->close();
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  destFile->close();
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());
  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;

  fileSize = 10000;
  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  *srcData = "";
  for(i=0; i<fileSize; i++){
    srcData->append((char)randomValue(0, 255));
  }
  written = srcFile->write(*srcData);
  QVERIFY(written == (qint64)srcData->size());
  // Reopen source file
  srcFile->close();
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  srcFile->close();
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  destFile->close();
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());
  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;

  fileSize = 16384;
  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  *srcData = "";
  for(i=0; i<fileSize; i++){
    srcData->append((char)randomValue(0, 255));
  }
  written = srcFile->write(*srcData);
  QVERIFY(written == (qint64)srcData->size());
  // Reopen source file
  srcFile->close();
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  srcFile->close();
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  destFile->close();
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());
  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;

  fileSize = 2*16384-1;
  // Create a file and add it to copier
  srcFile = new QTemporaryFile;
  QVERIFY(srcFile->open());
  *srcData = "";
  for(i=0; i<fileSize; i++){
    srcData->append((char)randomValue(0, 255));
  }
  written = srcFile->write(*srcData);
  QVERIFY(written == (qint64)srcData->size());
  // Reopen source file
  srcFile->close();
  QVERIFY(srcFile->open());
  srcFileInfo.setFile(*srcFile);
  srcFile->close();
  destFile = new QTemporaryFile;
  QVERIFY(destFile->open());
  destFileInfo.setFile(*destFile);
  destFile->close();
  fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath());
  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());
  delete srcFile;
  delete destFile;

  /*
   * Real copy test: middle size files
   */

  // Create files
  for(i=0; i<filesCount; i++){
    // Create a source file
    srcFile = new QTemporaryFile;
    QVERIFY(srcFile->open());
    // Generate some data
    *srcData = "";
    fileSize = randomValue(100, 10000000);
    for(j=0; j<fileSize; j++){
      srcData->append((char)randomValue(0, 255));
    }
    written = srcFile->write(*srcData);
    QVERIFY(written == (qint64)srcData->size());
    // Reopen source file
    srcFile->close();
    QVERIFY(srcFile->open());
    // calculate source hash
    srcHash.reset();
    srcHash.addData(srcFile->readAll());
    // Append to source lists
    srcFile->close();
    srcFilesList.append(srcFile);
    // Create a destination file
    destFile = new QTemporaryFile;
    QVERIFY(destFile->open());
    // calculate destination hash
    destHash.reset();
    destHash.addData(destFile->readAll());
    // Append to destination list
    destFile->close();
    destFilesList.append(destFile);
    // Check that source and destination files are different
    QVERIFY(srcHash.result() != destHash.result());
    // Add to copy list
    srcFileInfo.setFile(*srcFile);
    destFileInfo.setFile(*destFile);
    fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath(), true, true);
  }

  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());

  // Check copy
  for(i=0; i<filesCount; i++){
    // Open source file and calculate hash
    srcFile = srcFilesList.at(i);
    QVERIFY(srcFile->open());
    srcHash.reset();
    srcHash.addData(srcFile->readAll());
    // Open destination file and calculate hash
    destFile = destFilesList.at(i);
    QVERIFY(destFile->open());
    destHash.reset();
    destHash.addData(destFile->readAll());
    // Check
    QVERIFY(destHash.result() == srcHash.result());
    // Close files
    srcFile->close();
    destFile->close();
  }

  // Free ressources
  qDeleteAll(srcFilesList);
  srcFilesList.clear();
  qDeleteAll(destFilesList);
  destFilesList.clear();

  /*
   * Real copy test: little size files , with sync and check
   */

  // Create files
  filesCount = randomValue(100, 300);
  for(i=0; i<filesCount; i++){
    // Create a source file
    srcFile = new QTemporaryFile;
    QVERIFY(srcFile->open());
    // Generate some data
    *srcData = "";
    fileSize = randomValue(1, 100);
    for(j=0; j<fileSize; j++){
      srcData->append((char)randomValue(0, 255));
    }
    written = srcFile->write(*srcData);
    QVERIFY(written == (qint64)srcData->size());
    // Reopen source file
    srcFile->close();
    QVERIFY(srcFile->open());
    // calculate source hash
    srcHash.reset();
    srcHash.addData(srcFile->readAll());
    // Append to source lists
    srcFile->close();
    srcFilesList.append(srcFile);
    // Create a destination file
    destFile = new QTemporaryFile;
    QVERIFY(destFile->open());
    // calculate destination hash
    destHash.reset();
    destHash.addData(destFile->readAll());
    // Append to destination list
    destFile->close();
    destFilesList.append(destFile);
    // Check that source and destination files are different
    QVERIFY(srcHash.result() != destHash.result());
    // Add to copy list
    srcFileInfo.setFile(*srcFile);
    destFileInfo.setFile(*destFile);
    fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath(), true, true);
    srcFile->close();
    destFile->close();
  }

  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());

  // Check copy
  for(i=0; i<filesCount; i++){
    // Open source file and calculate hash
    srcFile = srcFilesList.at(i);
    QVERIFY(srcFile != 0);
    QVERIFY(srcFile->open());
    srcHash.reset();
    srcHash.addData(srcFile->readAll());
    // Open destination file and calculate hash
    destFile = destFilesList.at(i);
    QVERIFY(destFile != 0);
    QVERIFY(destFile->open());
    destHash.reset();
    destHash.addData(destFile->readAll());
    // Check
    QVERIFY(destHash.result() == srcHash.result());
    // Close files
    srcFile->close();
    destFile->close();
  }

  // Free ressources
  qDeleteAll(srcFilesList);
  srcFilesList.clear();
  qDeleteAll(destFilesList);
  destFilesList.clear();

  /*
   * Real copy test: little size files , no sync and no check
   */

  // Create files
  filesCount = randomValue(100, 300);
  for(i=0; i<filesCount; i++){
    // Create a source file
    srcFile = new QTemporaryFile;
    QVERIFY(srcFile->open());
    // Generate some data
    *srcData = "";
    fileSize = randomValue(1, 100);
    for(j=0; j<fileSize; j++){
      srcData->append((char)randomValue(0, 255));
    }
    written = srcFile->write(*srcData);
    QVERIFY(written == (qint64)srcData->size());
    // Reopen source file
    srcFile->close();
    QVERIFY(srcFile->open());
    // calculate source hash
    srcHash.reset();
    srcHash.addData(srcFile->readAll());
    // Append to source lists
    srcFile->close();
    srcFilesList.append(srcFile);
    // Create a destination file
    destFile = new QTemporaryFile;
    QVERIFY(destFile->open());
    // calculate destination hash
    destHash.reset();
    destHash.addData(destFile->readAll());
    // Append to destination list
    destFile->close();
    destFilesList.append(destFile);
    // Check that source and destination files are different
    QVERIFY(srcHash.result() != destHash.result());
    // Add to copy list
    srcFileInfo.setFile(*srcFile);
    destFileInfo.setFile(*destFile);
    fc.addCopy(srcFileInfo.absoluteFilePath(), destFileInfo.absoluteFilePath(), false, false);
    srcFile->close();
    destFile->close();
  }

  // Run copy
  QVERIFY(fc.startCopy());
  QVERIFY(fc.waitFinished());

  // Free ressources
  qDeleteAll(srcFilesList);
  qDeleteAll(destFilesList);
  delete srcData;
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtFileTest fileTest;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&fileTest, argc, argv);
}
