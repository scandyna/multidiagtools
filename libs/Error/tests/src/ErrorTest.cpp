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
#include "ErrorTest.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorLogger/Logger.h"
#include "Mdt/ErrorLogger/Backend.h"
#include "Mdt/ErrorLogger/ConsoleBackend.h"
#include "Mdt/ErrorLogger/FileBackend.h"
#include <QObject>
#include <QMetaObject>
#include <QMetaType>
#include <QByteArray>
#include <QLatin1String>
#include <QString>
#include <QTemporaryFile>
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include <QCoreApplication>
#include <memory>
#include <vector>

#include <QDebug>

/**
 * \todo Try with QVariant (queued signal/slot)
 *  See also Q_DECLARE_SHARED
 */

/*
 * Logger backend for tests
 */
class ErrorLoggerTestBackend : public Mdt::ErrorLogger::Backend
{
 public:
  ErrorLoggerTestBackend(){}
  ~ErrorLoggerTestBackend(){}
  void logError(const Mdt::Error & error)
  {
    errorList.push_back(error);
  }
  std::vector<Mdt::Error> errorList;
};

/*
 * For data driven tests
 */
typedef QVector<Mdt::Error> ErrorVectorType;
Q_DECLARE_METATYPE(ErrorVectorType);

/*
 * mdtErrorTest implementation
 */

void ErrorTest::constructAndCopyTest()
{
  /*
   * Default constructed
   */
  Mdt::Error error1;
  QVERIFY(error1.isNull());
  QVERIFY(error1.level() == Mdt::Error::NoError);
  QVERIFY(error1.text().isEmpty());
  QVERIFY(error1.informativeText().isEmpty());
  QCOMPARE(error1.fileLine(), 0);
  // Calling error() is allowed on null Mdt::Error
  QCOMPARE(error1.error<int>(), 0);
  /*
   * Copy construct
   */
  // Set some attributes to error1
  error1.setError<int>(1, "error1", Mdt::Error::Critical);
  error1.setInformativeText("error1 info");
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  QCOMPARE(error1.fileLine(), 0);
  // Construct error2 on base of error1
  Mdt::Error error2(error1);
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QCOMPARE(error2.informativeText(), QString("error1 info"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  /*
   * COW test
   */
  // Set error2
  error2.setError<int>(2, "error2", Mdt::Error::Info);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == Mdt::Error::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  // Update error2 informative text
  error2.setInformativeText("error2 info");
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QCOMPARE(error2.informativeText(), QString("error2 info"));
  QVERIFY(error2.level() == Mdt::Error::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  // Set error2 to a other error type
  error2.setError<double>(2.1, "error2.1", Mdt::Error::Warning);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<double>(), 2.1);
  QCOMPARE(error2.text(), QString("error2.1"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == Mdt::Error::Warning);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  /*
   * Check many copies of error
   */
  // Set error1
  error1.setError<int>(5, "shared error 5", Mdt::Error::Info);
  // Copy construct 2 errors
  Mdt::Error error3(error1);
  Mdt::Error error4(error1);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<int>(), 5);
  QCOMPARE(error3.text(), QString("shared error 5"));
  QVERIFY(error3.level() == Mdt::Error::Info);
  // Check error4
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == Mdt::Error::Info);
  // Set error3
  error3.setError<double>(3.0, "error3", Mdt::Error::Warning);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == Mdt::Error::Warning);
  // Check that error1 is untouched
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 5);
  QCOMPARE(error1.text(), QString("shared error 5"));
  QVERIFY(error1.level() == Mdt::Error::Info);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == Mdt::Error::Info);
  // Clear error1
  error1.clear();
  // Check error1
  QVERIFY(error1.isNull());
  // Check that error3 is untouched
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == Mdt::Error::Warning);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == Mdt::Error::Info);
  /*
   * Copy assignment
   */
  // Set error1
  error1.setError<int>(1, "error1", Mdt::Error::Critical);
  // Assign error1 to error2
  error2 = error1;
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  // Set error1
  error1.setError<double>(1.1, "error1.1", Mdt::Error::Warning);
  // Check error1
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<double>(), 1.1);
  QCOMPARE(error1.text(), QString("error1.1"));
  QVERIFY(error1.level() == Mdt::Error::Warning);
  // Check that error2 is untouched
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  /*
   * Update tests
   * (setError() allways resets, check other setters)
   */
  // To check that COW works, we set error2 to other type than error1
  error2.setError<double>(2.5, "error2.5", Mdt::Error::Warning);
  // Set error1
  error1.setError<int>(1, "error1", Mdt::Error::Critical);
  // Assign error1 to error2
  error2 = error1;
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check informative text
  QVERIFY(error1.informativeText().isEmpty());
  QVERIFY(error2.informativeText().isEmpty());
  // Set informative text to error1
  error1.setInformativeText("error1 info");
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check error1
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  // Check that error2 is untouched
  QVERIFY(error2.informativeText().isEmpty());
  // Set informative text to error2
  error2.setInformativeText("error2 info");
  // Check that types are preserved
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error2.error<int>(), 1);
  // Check error2
  QCOMPARE(error2.informativeText(), QString("error2 info"));
  // Check that error1 is untouched
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  /*
   * Move
   */
  /// \todo Implement once understanding move..
  qDebug() << "Move ...";
  error2 = std::move(error1);
  qDebug() << "Done";
  qDebug() << "swap ...";
  std::swap(error1, error2);
  qDebug() << "Done";
}

void ErrorTest::errorStackTest()
{
  std::vector<Mdt::Error> errorStack;
  /*
   * Some check errors with empty stack, etc...
   */
  Mdt::Error error1;
  Mdt::Error error2;
  Mdt::Error error3;
  Mdt::Error error4;
  // Try to get stack from null error
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());
  errorStack = error1.getErrorStack();
  QVERIFY(errorStack.empty());
  errorStack = error2.getErrorStack();
  QVERIFY(errorStack.empty());
  // Setup errors
  error1.setError("Error 1", Mdt::Error::Critical);
  error2.setError("Error 2", Mdt::Error::Info);
  // Try to get error stack from error that does not have stack
  errorStack = error1.getErrorStack();
  QVERIFY(errorStack.empty());
  errorStack = error2.getErrorStack();
  QVERIFY(errorStack.empty());
  /*
   * Stack error1 -> error2 :
   * error2
   *   |-[error1]
   */
  error2.stackError(error1);
  errorStack = error2.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  /*
   * Immagine som senario:
   * User wants to open a file, using a software with some GUI.
   * The software calls some helper function,
   * witch also call a system function.
   * The system function fails (because of missing write acces, f.ex.)
   * and also returns a error with a system specific (and technical) text.
   * Then, the software helper function automatically fails,
   * and simply gets error returned by the system function.
   * Finally, the software generates a error with a more user friendly message,
   * but also some back trace of what technically failed
   */
  Mdt::Error systemFunctionError;
  Mdt::Error systemFunctionApiError;
  Mdt::Error helperFunctionError;
  Mdt::Error guiError;

  // First, system API function fails
  systemFunctionApiError.setError<int>(-1, "write access error", Mdt::Error::Critical);
  // Then, system function generates a error with some details and stacks API error
  systemFunctionError.setError<int>(-1, "Could not open file 'fake.txt'", Mdt::Error::Critical);
  systemFunctionError.stackError(systemFunctionApiError);
  errorStack = systemFunctionError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("write access error"));
  // Helper function also fails, but does not provide more informations
  helperFunctionError = systemFunctionError;
  errorStack = helperFunctionError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("write access error"));
  // Finally, the GUI function makes some checks, and generates a error with user friendly message
  auto errorCode = helperFunctionError.error<int>();
  switch(errorCode){
    case -1:
      guiError.setError("Could not open document 'fake.txt' because of missing write access", Mdt::Error::Critical);
      break;
    case -2:
      guiError.setError("Could not open document 'fake.txt' because it allready exists", Mdt::Error::Critical);
      break;
  }
  guiError.stackError(helperFunctionError);
  // The guiError is also displayed (f.ex. with a mdtErrorDialog)
  QCOMPARE(guiError.text(), QString("Could not open document 'fake.txt' because of missing write access"));
  errorStack = guiError.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)2);
  QCOMPARE(errorStack.at(0).error<int>(), -1);
  QCOMPARE(errorStack.at(0).text(), QString("Could not open file 'fake.txt'"));
  QCOMPARE(errorStack.at(1).error<int>(), -1);
  QCOMPARE(errorStack.at(1).text(), QString("write access error"));
  /*
   * Check Copy-On-Write
   */
  // error3 and error4 will be stacked to error1 and error2, set them up
  error3.setError<long int>(30, "error3", Mdt::Error::Warning);
  error4.setError<float>(4.0, "error4", Mdt::Error::Info);
  // Set error1 and error2 to some error
  error1.setError<int>(1, "error1", Mdt::Error::Critical);
  // Assign error1 -> error1
  error2 = error1;
  QVERIFY(error1.getErrorStack().empty());
  QVERIFY(error2.getErrorStack().empty());
  // Stack to error1
  error1.stackError(error3);
  // Check error1 stack
  errorStack = error1.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<long int>(), (long int)30);
  QCOMPARE(errorStack.at(0).text(), QString("error3"));
  // Check that error2 is untouched
  QVERIFY(error2.getErrorStack().empty());
  // Stack to error2
  error2.stackError(error4);
  // Check error2
  errorStack = error2.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<float>(), 4.0);
  QCOMPARE(errorStack.at(0).text(), QString("error4"));
  // Check that error1 is untouched
  errorStack = error1.getErrorStack();
  QCOMPARE(errorStack.size(), (size_t)1);
  QCOMPARE(errorStack.at(0).error<long int>(), (long int)30);
  QCOMPARE(errorStack.at(0).text(), QString("error3"));

}

void ErrorTest::setSourceTest()
{
  // Constants for check
  const QString fileName = QFileInfo(__FILE__).fileName();
  const QString functionName = metaObject()->className() + QLatin1String("::") + QLatin1String(__FUNCTION__) + QLatin1String("()");
  /*
   * Check creating error with helper macros
   */
  const int firstLine = __LINE__;
  Mdt::Error error1 = mdtErrorNew("error1", Mdt::Error::Critical, "ErrorTest");
  Mdt::Error error2 = mdtErrorNewT(int, 2, "error2", Mdt::Error::Critical, "ErrorTest");
  Mdt::Error error3 = mdtErrorNewQ("error3", Mdt::Error::Info, this);
  Mdt::Error error4 = mdtErrorNewTQ(float, 4.0, "error4", Mdt::Error::Info, this);
  // Check error1
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  QCOMPARE(error1.fileName(), fileName);
  QCOMPARE(error1.fileLine(), firstLine + 1);
  QCOMPARE(error1.functionName(), functionName);
  // Check error2
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  QCOMPARE(error2.fileName(), fileName);
  QCOMPARE(error2.fileLine(), firstLine + 2);
  QCOMPARE(error2.functionName(), functionName);
  // Check error3
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == Mdt::Error::Info);
  QCOMPARE(error3.fileName(), fileName);
  QCOMPARE(error3.fileLine(), firstLine + 3);
  QCOMPARE(error3.functionName(), functionName);
  // Check error4
  QCOMPARE(error4.error<float>(), 4.0);
  QCOMPARE(error4.text(), QString("error4"));
  QVERIFY(error4.level() == Mdt::Error::Info);
  QCOMPARE(error4.fileName(), fileName);
  QCOMPARE(error4.fileLine(), firstLine + 4);
  QCOMPARE(error4.functionName(), functionName);
  /*
   * COW check
   */
  // Assign error2 = error1
  error2 = error1;
  // Check error2
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  QCOMPARE(error2.fileName(), fileName);
  QCOMPARE(error2.fileLine(), firstLine + 1);
  QCOMPARE(error2.functionName(), functionName);
  // Update error2
  error2.setSource("file.cpp", 36, "C", "F");
  // Check error2
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  QCOMPARE(error2.fileName(), QString("file.cpp"));
  QCOMPARE(error2.fileLine(), 36);
  QCOMPARE(error2.functionName(), QString("C::F()"));
  // Check that error1 is untouched
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  QCOMPARE(error1.fileName(), fileName);
  QCOMPARE(error1.fileLine(), firstLine + 1);
  QCOMPARE(error1.functionName(), functionName);
  // Update source of error1
  error1.setSource("file1.cpp", 35, "C1", "F1");
  // Check error1
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == Mdt::Error::Critical);
  QCOMPARE(error1.fileName(), QString("file1.cpp"));
  QCOMPARE(error1.fileLine(), 35);
  QCOMPARE(error1.functionName(), QString("C1::F1()"));
  // Check that error2 is untouched
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == Mdt::Error::Critical);
  QCOMPARE(error2.fileName(), QString("file.cpp"));
  QCOMPARE(error2.fileLine(), 36);
  QCOMPARE(error2.functionName(), QString("C::F()"));
  /*
   * Check MDT_ERROR_SET_SRC and MDT_ERROR_SET_SRC_Q macros
   */
  // Check MDT_ERROR_SET_SRC
  QVERIFY(!error1.isNull());
  MDT_ERROR_SET_SRC(error1, this->metaObject()->className());
  QCOMPARE(error1.fileName(), fileName);
  QCOMPARE(error1.functionName(), QString(this->metaObject()->className() + QLatin1String("::") + QLatin1String(__FUNCTION__) + QLatin1String("()")));
  // Check MDT_ERROR_SET_SRC_Q
  QVERIFY(!error2.isNull());
  MDT_ERROR_SET_SRC_Q(error2, this);
  QCOMPARE(error2.fileName(), fileName);
  QCOMPARE(error2.functionName(), QString(this->metaObject()->className() + QLatin1String("::") + QLatin1String(__FUNCTION__) + QLatin1String("()")));
  /*
   * Check with incomplete function names
   */
  error1.setSource("file1.cpp", 20, "C1", "");
  QCOMPARE(error1.functionName(), QString("C1"));
}

void ErrorTest::macrosTest()
{
  auto error1 = mdtErrorNew("error1", Mdt::Error::Info, "Class1");
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.level(), Mdt::Error::Info);
  QCOMPARE(error1.functionName(), QString("Class1::macrosTest()"));

  auto error2 = mdtErrorNewT(int, 2, "error2", Mdt::Error::Critical, "Class2");
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QCOMPARE(error2.level(), Mdt::Error::Critical);
  QCOMPARE(error2.functionName(), QString("Class2::macrosTest()"));

  auto error3 = mdtErrorNewQ("error3", Mdt::Error::Info, this);
  QCOMPARE(error3.text(), QString("error3"));
  QCOMPARE(error3.level(), Mdt::Error::Info);
  QCOMPARE(error3.functionName(), QString("ErrorTest::macrosTest()"));

  auto error4 = mdtErrorNewTQ(int, 4, "error4", Mdt::Error::Critical, this);
  QCOMPARE(error4.error<int>(), 4);
  QCOMPARE(error4.text(), QString("error4"));
  QCOMPARE(error4.level(), Mdt::Error::Critical);
  QCOMPARE(error4.functionName(), QString("ErrorTest::macrosTest()"));

  Mdt::Error error5;
  error5.setError("error5", Mdt::Error::Warning);
  MDT_ERROR_SET_SRC(error5, "Class5");
  QCOMPARE(error5.functionName(), QString("Class5::macrosTest()"));

  Mdt::Error error6;
  error6.setError("error6", Mdt::Error::Critical);
  MDT_ERROR_SET_SRC(error6, this);
  QCOMPARE(error6.functionName(), QString("ErrorTest::macrosTest()"));

  QFile file("/msadkj/someNonExistingDirectory/SomeNonExistingFile");
  QVERIFY(!file.open(QFile::ReadOnly));

  auto error7 = mdtErrorFromQFileDevice(file, "Class7");
  QCOMPARE(error7.error<QFile::FileError>(), QFile::OpenError);
  QVERIFY(!error7.text().isEmpty());
  QVERIFY(error7.level() != Mdt::Error::NoError);
  QCOMPARE(error7.functionName(), QString("Class7::macrosTest()"));

  auto error8 = mdtErrorFromQFileDeviceQ(file, this);
  QCOMPARE(error8.error<QFile::FileError>(), QFile::OpenError);
  QVERIFY(!error8.text().isEmpty());
  QVERIFY(error8.level() != Mdt::Error::NoError);
  QCOMPARE(error8.functionName(), QString("ErrorTest::macrosTest()"));

  auto error9 = mdtErrorFromQFile(file, "Class9");
  QCOMPARE(error9.error<QFile::FileError>(), QFile::OpenError);
  QVERIFY(!error9.text().isEmpty());
  QVERIFY(error9.level() != Mdt::Error::NoError);
  QCOMPARE(error9.functionName(), QString("Class9::macrosTest()"));

  auto error10 = mdtErrorFromQFileQ(file, this);
  QCOMPARE(error10.error<QFile::FileError>(), QFile::OpenError);
  QVERIFY(!error10.text().isEmpty());
  QVERIFY(error10.level() != Mdt::Error::NoError);
  QCOMPARE(error10.functionName(), QString("ErrorTest::macrosTest()"));
}

void ErrorTest::errorLoggerConsoleBackendTest()
{
  Mdt::ErrorLogger::ConsoleBackend backend;

  Mdt::Error error = mdtErrorNewTQ(int, 25, "error", Mdt::Error::Critical, this);
  error.stackError(mdtErrorNewQ("system error 2", Mdt::Error::Critical, this));
  error.stackError(mdtErrorNewQ("system error 1", Mdt::Error::Critical, this));

  error.setInformativeText("Some more info");
  backend.logError(error);
}

/// \todo Should backend remove end of lines of text and informative text ?
/// \todo Strange char after each log (simple space or EOF, or...?)
void ErrorTest::errorLoggerFileBackendTest()
{
  Mdt::ErrorLogger::FileBackend backend;
  Mdt::Error error; // = mdtErrorNewTQ(int, 25, "error", mdtErrorV2::Error, this);
  QTemporaryFile tmpFile;
  QFile logFile;
  QByteArray logFileData;

  // Prepare log file for tests
  QVERIFY(tmpFile.open());
  tmpFile.close();
  logFile.setFileName(tmpFile.fileName());

  /*
   * Initial state
   */
  QCOMPARE(backend.maxFileSize(), 1024*1024);
  /*
   * Setup logger backend
   */
  // Setup
  QVERIFY(backend.setLogFilePath(logFile.fileName(), 10));
  // Check
  QCOMPARE(backend.logFilePath(), logFile.fileName());
  QCOMPARE(backend.maxFileSize(), 10);
  QVERIFY(!backend.backupLogFilePath().isEmpty());
  QVERIFY(backend.backupLogFilePath() != backend.logFilePath());
  /*
   * Check log file backup
   * Note: we do not use QTemporaryFile for check, but another QFile object.
   *       Not doing so gave strange problem (on Linux, at least):
   *       - In test, close log file
   *       - Log a new error (backend also does backup (rename)
   *       - In test, open log file
   *       - Now, its the backup file that is read !
   */
  // Check first
  QVERIFY(QFile::exists(backend.logFilePath()));
  QVERIFY(!QFile::exists(backend.backupLogFilePath()));
  // Log error
  error.setError("1234567890", Mdt::Error::Critical);
  backend.logError(error);
  // Check
  QVERIFY(!QFile::exists(backend.backupLogFilePath()));
  QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
  logFileData = logFile.readAll();
  logFile.close();
  QVERIFY(!logFileData.isEmpty());
  QVERIFY(logFileData.contains("1234567890"));
  // Log error
  error.setError("ABCDEFGH", Mdt::Error::Critical);
  backend.logError(error);
  // Check - We know that we reached maximum log file size after previous log
  QVERIFY(QFile::exists(backend.backupLogFilePath()));
  QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
  logFileData = logFile.readAll();
  logFile.close();
  QVERIFY(!logFileData.isEmpty());
  QVERIFY(!logFileData.contains("1234567890"));
  QVERIFY(logFileData.contains("ABCDEFGH"));
  // Check backed up log file
  QFile backupFile(backend.backupLogFilePath());
  QVERIFY(backupFile.open(QIODevice::ReadOnly | QIODevice::Text));
  logFileData = backupFile.readAll();
  QVERIFY(!logFileData.isEmpty());
  QVERIFY(logFileData.contains("1234567890"));

  // Remove created backup file
  QFile::remove(backend.backupLogFilePath());
}

void ErrorTest::errorLoggerTest()
{
  using namespace Mdt::ErrorLogger;

  LoggerGuard loggerGard;
  auto testBackend = std::make_shared<ErrorLoggerTestBackend>();
  QFETCH(ErrorVectorType, errorList); // ErrorVectorType == QVector<Mdt::Error>
  QFETCH(int, interLogWaitTime);
  QFETCH(int, endWaitTime);

  // Setup logger
  Logger::addBackend(testBackend);
  // Log errors
  for(const auto & error : errorList){
    QVERIFY(!error.isNull());
    Logger::logError(error);
    if(interLogWaitTime > 0){
      QTest::qWait(interLogWaitTime);
    }
  }
  // Explicitly cleanup here (to join thread)
  if(endWaitTime > 0){
    QTest::qWait(endWaitTime);
  }
  Logger::cleanup();
  // Check
  QCOMPARE((int)testBackend->errorList.size(), errorList.size());
  for(int i = 0; i < errorList.size(); ++i){
    auto error = testBackend->errorList.at(i);
    auto expectedError = errorList.at(i);
    QVERIFY(error.level() == expectedError.level());
    QCOMPARE(error.text(), expectedError.text());
    QCOMPARE(error.informativeText(), expectedError.informativeText());
  }
}

void ErrorTest::errorLoggerTest_data()
{
  QTest::addColumn<ErrorVectorType>("errorList");
  QTest::addColumn<int>("interLogWaitTime");
  QTest::addColumn<int>("endWaitTime");

  Mdt::Error error;
  ErrorVectorType errorList;

  // 1 error
  error.setError("error1", Mdt::Error::Critical);
  errorList.append(error);
  QTest::newRow("1,0,0") << errorList << 0 << 0;
  QTest::newRow("1,1,0") << errorList << 1 << 0;
  QTest::newRow("1,0,1") << errorList << 0 << 1;
  QTest::newRow("1,10,10") << errorList << 10 << 10;
  // 2 errors
  error.setError("error2", Mdt::Error::Info);
  errorList.append(error);
  QTest::newRow("2,0,0") << errorList << 0 << 0;
  QTest::newRow("2,1,0") << errorList << 1 << 0;
  QTest::newRow("2,0,1") << errorList << 0 << 1;
  // 3 errors
  error.setError("error3", Mdt::Error::Warning);
  errorList.append(error);
  QTest::newRow("3,0,0") << errorList << 0 << 0;
  QTest::newRow("3,1,0") << errorList << 1 << 0;
  QTest::newRow("3,0,1") << errorList << 0 << 1;
  QTest::newRow("3,0,1") << errorList << 1 << 1;
}

void errorLoggerConcurrentAccessTestWorker(const Mdt::Error & error)
{
  Mdt::ErrorLogger::Logger::logError(error);
}

void ErrorTest::errorLoggerConcurrentAccessTest()
{
  using namespace Mdt::ErrorLogger;

  LoggerGuard loggerGard;
  auto testBackend = std::make_shared<ErrorLoggerTestBackend>();
  std::vector<std::thread> threadList;
  QFETCH(ErrorVectorType, errorList); // ErrorVectorType == QVector<Mdt::Error>

  // Setup logger
  Logger::addBackend(testBackend);
  // Create threads
  for(int i = 0; i < errorList.size(); ++i){
    threadList.emplace_back(std::thread(errorLoggerConcurrentAccessTestWorker, errorList.at(i)));
  }
  // Join our threads
  for(auto & thd : threadList){
    thd.join();
  }
  // Explicitly cleanup here (to join logger thread)
  Logger::cleanup();
  // Check that all errors where logged
  QCOMPARE((int)testBackend->errorList.size(), errorList.size());
}

void ErrorTest::errorLoggerConcurrentAccessTest_data()
{
  QTest::addColumn<ErrorVectorType>("errorList");

  ErrorVectorType errorList;
  const int maxThreads = 20;

  for(int i = 0; i <= maxThreads; ++i){
    Mdt::Error error = mdtErrorNewQ("error" + QString::number(i), Mdt::Error::Critical, this);
    errorList.append(error);
    QTest::newRow(QString::number(i).toStdString().c_str()) << errorList;
  }
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  ErrorTest errorTest;

  return QTest::qExec(&errorTest, argc, argv);
}
