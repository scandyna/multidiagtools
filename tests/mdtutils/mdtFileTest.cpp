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
