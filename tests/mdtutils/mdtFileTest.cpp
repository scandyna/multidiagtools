/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include <QTemporaryFile>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QList>
#include <QStringList>

#include <QDebug>

void mdtFileTest::csvFileWriteTest()
{
  QTemporaryFile tmp;
  mdtCsvFile csv;
  QByteArray data;

  // Create a temporary file
  QVERIFY(tmp.open());
  tmp.close();

  // Create a CSV file
  csv.setFileName(tmp.fileName());
  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));

  // Write data
  data = "0123;ABCD";
  data += "\n";
  data += "UIJ;88";
  QVERIFY(csv.write(data) == (qint64)data.size());
  csv.close();

  // Check written data
  QVERIFY(tmp.open());

#ifdef Q_OS_WIN
  // On Windows, we reconstruct ref data with CRLF sequence
  data = "0123;ABCD";
  data += (char)0x0D; // CR
  data += "\n";       // LF
  data += "UIJ;88";
#endif
  QVERIFY(tmp.readAll() == data);
  tmp.close();
}

void mdtFileTest::csvFileReadTest()
{
  QTemporaryFile tmp, tmpErr, tmpComment;
  mdtCsvFile csv;
  QStringList line;
  QList<QStringList> data;
  QList<QStringList> refData;
  QString str;

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
        line << "\n";
      }
    }
    data << line;
  }
  // Write data
  for(int i=0; i<data.size(); i++){
    for(int j=0; j<data.at(i).size(); j++){
      QVERIFY(tmp.write(data.at(i).at(j).toAscii()) == (qint64)data.at(i).at(j).toAscii().size());
    }
  }
  // Close tmp file
  tmp.close();

  // Create a CSV file
  csv.setFileName(tmp.fileName());
  QVERIFY(csv.open(QIODevice::ReadWrite | QIODevice::Text));

  // Read file and verify data
  QVERIFY(csv.readLines(separator.toAscii(), dataProtection.toAscii()));
  for(int i=0; i<refData.size(); i++){
    for(int j=0; j<refData.at(i).size(); j++){
      QVERIFY(csv.valueAt(i, j) == refData.at(i).at(j));
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
  QVERIFY(csv.valueAt(0, 0) == "Valid data");
  QVERIFY(csv.valueAt(0, 1) == "with data ; protection");
  QVERIFY(csv.valueAt(0, 2) == "and without");
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
  QVERIFY(csv.valueAt(0, 0) == "Some invalid");
  QVERIFY(csv.valueAt(1, 0) == "And 2 seps");
  QVERIFY(csv.valueAt(1, 1) == "");
  QVERIFY(csv.valueAt(1, 2) == "With other data and no end of line at end");

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
  QVERIFY(csv.valueAt(0, 0) == "Uncommented data");
  QVERIFY(csv.valueAt(0, 1) == "Field with protection");
  QVERIFY(csv.valueAt(0, 2) == "0123");
  QVERIFY(csv.valueAt(1, 0) == "Uncommented data");
  QVERIFY(csv.valueAt(1, 1) == "Some data");
  QVERIFY(csv.valueAt(1, 2) == "987");

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

void mdtFileTest::mdtPartitionAttributesTest()
{
  mdtPartitionAttributes pa;
  QStringList paList;
  
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
#endif
  
  paList = pa.availablePartitions();
  qDebug() << "Available partitions:";
  for(int i=0; i<paList.size(); i++){
    qDebug() << paList.at(i) << ":";
    pa.setPath(paList.at(i));
    qDebug() << " -> Name: " << pa.name() << " , rootPath: " << pa.rootPath() << " , FS: " << pa.fileSystem() << " , RD only: " << pa.isReadOnly();
  }
}
