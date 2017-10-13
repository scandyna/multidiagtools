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
#include "FileBackendTest.h"
#include "Mdt/ErrorLogger/FileBackend.h"
#include "Mdt/ErrorLogger/FormatEngine.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

using namespace Mdt::ErrorLogger;

void FileBackendTest::initTestCase()
{
}

void FileBackendTest::cleanupTestCase()
{
}

/*
 * Format engine for tests
 */

class TestFormatEngine : public FormatEngine
{
 public:

  QString formatError(const Mdt::Error& error) const override
  {
    return error.text();
  }
};

/*
 * Tests
 */

void FileBackendTest::basicPropertyTest()
{
  /*
   * Initial state
   */
  FileBackend backend;
  QVERIFY(backend.logFilePath().isEmpty());
  QVERIFY(backend.backupLogFilePath().isEmpty());
  QCOMPARE((int)backend.maxFileSize(), 1024*1024);
  /*
   * Set a invalid file path
   */
  QVERIFY(!backend.setLogFilePath("/some/hopefully/non/existing/path"));
  QVERIFY(backend.logFilePath().isEmpty());
  QVERIFY(backend.backupLogFilePath().isEmpty());
  QCOMPARE((int)backend.maxFileSize(), 1024*1024);
  /*
   * Set a valid file path
   */
  QTemporaryFile file;
  QVERIFY(file.open());
  QString expectedFilePath = QFileInfo(file).absoluteFilePath();
  QString expectedBackupFilePath = expectedFilePath + ".save";
  QVERIFY(backend.setLogFilePath(file.fileName(), 100));
  QCOMPARE( QFileInfo(backend.logFilePath()).absoluteFilePath(), expectedFilePath );
  QCOMPARE( QFileInfo(backend.backupLogFilePath()).absoluteFilePath(), expectedBackupFilePath );
  QCOMPARE(backend.maxFileSize(), 100);
}

void FileBackendTest::logErrorTest()
{
  /*
   * Prepare log file for tests
   *
   * We cannot use QTemporaryFile here, because it keeps the file open
   * (See Qt documentation about this),
   * The backend will open the file, log the error and close it.
   * When necessary, it will also backup it.
   * This last setp will fail, at least on Windows.
   */
  QTemporaryDir logDir;
  QVERIFY(logDir.isValid());
  QString logFilePath = QDir::cleanPath(logDir.path() + "/file.log");
  /*
   * Setup backend
   */
  FileBackend backend;
  backend.setFormatEngine<TestFormatEngine>();
  QVERIFY(backend.setLogFilePath(logFilePath));
  QVERIFY( QFile::exists(logFilePath) );
  /*
   * Log a error
   */
  backend.logError( mdtErrorNewQ("error 1", Mdt::Error::Info, this) );
  QCOMPARE( readFile(logFilePath), QString("error 1\n") );
  /*
   * Cleanup and check that the file is not deleted
   */
  backend.cleanup();
  QVERIFY( QFile::exists(logFilePath) );
  QCOMPARE( readFile(logFilePath), QString("error 1\n") );
}

void FileBackendTest::logNoErrorTest()
{
  /*
   * Prepare log file for tests
   */
  QTemporaryDir logDir;
  QVERIFY(logDir.isValid());
  QString logFilePath = QDir::cleanPath(logDir.path() + "/file.log");
  /*
   * Setup backend
   */
  FileBackend backend;
  backend.setFormatEngine<TestFormatEngine>();
  QVERIFY(backend.setLogFilePath(logFilePath));
  QVERIFY( QFile::exists(logFilePath) );
  /*
   * Cleanup and check that the file is not deleted
   */
  QVERIFY( readFile(logFilePath).isEmpty() );
  backend.cleanup();
  QVERIFY( !QFile::exists(logFilePath) );
}

void FileBackendTest::backupTest()
{
  /*
   * Prepare log file for tests
   */
  QTemporaryDir logDir;
  QVERIFY(logDir.isValid());
  QString logFilePath = QDir::cleanPath(logDir.path() + "/file.log");
  /*
   * Setup backend
   */
  FileBackend backend;
  backend.setFormatEngine<TestFormatEngine>();
  QVERIFY(backend.setLogFilePath(logFilePath, 10));
  QVERIFY( QFile::exists(logFilePath) );
  /*
   * Log a error
   * Log file will be below the 10 bytes limit
   */
  backend.logError( mdtErrorNewQ("12345", Mdt::Error::Info, this) );
  QVERIFY( QFile::exists(logFilePath) );
  QCOMPARE( readFile(logFilePath), QString("12345\n") );
  QVERIFY( !QFile::exists(backend.backupLogFilePath()) );
  /*
   * Log a error
   * Log file will reach the 10 bytes limit after this
   */
  backend.logError( mdtErrorNewQ("67890", Mdt::Error::Info, this) );
  QVERIFY( QFile::exists(logFilePath) );
  QCOMPARE( readFile(logFilePath), QString("12345\n67890\n") );
  QVERIFY( !QFile::exists(backend.backupLogFilePath()) );
  /*
   * Log a error
   * Now, a backup must be done
   */
  backend.logError( mdtErrorNewQ("ABCD", Mdt::Error::Info, this) );
  QVERIFY( QFile::exists(logFilePath) );
  QCOMPARE( readFile(logFilePath), QString("ABCD\n") );
  QVERIFY( QFile::exists(backend.backupLogFilePath()) );
  QCOMPARE( readFile(backend.backupLogFilePath()), QString("12345\n67890\n") );
  /*
   * Cleanup and check that the files are not deleted
   */
  backend.cleanup();
  QVERIFY( QFile::exists(logFilePath) );
  QVERIFY( QFile::exists(backend.backupLogFilePath()) );
}

/*
 * Helpers
 */

QString FileBackendTest::readFile(const QString& filePath)
{
  QString str;
  QFile file(filePath);

  if(!file.open(QFile::ReadOnly | QFile::Text)){
    qWarning() << "Unable to open file " << filePath << ": " << file.errorString();
    return str;
  }
  QTextStream stream(&file);
  str = stream.readAll();

  return str;
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  FileBackendTest test;
//   qRegisterMetaType<Mdt::Error>();

  return QTest::qExec(&test, argc, argv);
}
