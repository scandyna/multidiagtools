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
#include "mdtErrorTest.h"
#include "mdtApplication.h"
#include "mdtErrorV2.h"
#include "mdt/error/Logger.h"
#include "mdt/error/LoggerBackend.h"
#include "mdt/error/LoggerConsoleBackend.h"
#include "mdt/error/LoggerFileBackend.h"
#include <QObject>
#include <QMetaObject>
#include <QByteArray>
#include <QLatin1String>
#include <QString>
#include <QTemporaryFile>
#include <QFile>
#include <memory>
#include <vector>

#include <QDebug>

/*
 * Logger backend for tests
 */
class mdtErrorLoggerTestBackend : public mdt::error::LoggerBackend
{
 public:
  mdtErrorLoggerTestBackend(){}
  ~mdtErrorLoggerTestBackend(){}
  void logError(const mdtErrorV2 & error)
  {
    errorList.push_back(error);
  }
  std::vector<mdtErrorV2> errorList;
};

/*
 * mdtErrorTest implementation
 */

#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mutex25;
std::condition_variable cv;
QString data;
bool ready = false;
bool proceed = false;

void workerThreadFunction()
{
  // Wait until main sends data
  std::unique_lock<std::mutex> lock(mutex25);
  cv.wait(lock, []{return ready;});

  // Woke up, take the lock
  qDebug() << "Worker, processing...";
  data += " after process";

  // Notify processed
  proceed = true;
  qDebug() << "Worker, processing done";

  // Unlock and notify
  lock.unlock();
  cv.notify_one();
}

void mdtErrorTest::sandbox()
{
  std::thread worker(workerThreadFunction);

  data = "Example data";
  // Send data to wroker
  {
    std::lock_guard<std::mutex> lock(mutex25);
    ready = true;
    qDebug() << "main: data ready";
  }
  cv.notify_one();

  // Wait for the worker
  {
    std::unique_lock<std::mutex> lock(mutex25);
    cv.wait(lock, []{return proceed;});
  }
  qDebug() << "Main: data: " << data;

  worker.join();
}


void mdtErrorTest::constructAndCopyTest()
{
  /*
   * Default constructed
   */
  mdtErrorV2 error1;
  QVERIFY(error1.isNull());
  QVERIFY(error1.level() == mdtErrorV2::NoError);
  QVERIFY(error1.text().isEmpty());
  QVERIFY(error1.informativeText().isEmpty());
  QCOMPARE(error1.fileLine(), 0);
  // Calling error() is allowed on null mdtError
  QCOMPARE(error1.error<int>(), 0);
  /*
   * Copy construct
   */
  // Set some attributes to error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  error1.setInformativeText("error1 info");
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  QCOMPARE(error1.fileLine(), 0);
  // Construct error2 on base of error1
  mdtErrorV2 error2(error1);
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QCOMPARE(error2.informativeText(), QString("error1 info"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  /*
   * COW test
   */
  // Set error2
  error2.setError<int>(2, "error2", mdtErrorV2::Info);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == mdtErrorV2::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  // Update error2 informative text
  error2.setInformativeText("error2 info");
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QCOMPARE(error2.informativeText(), QString("error2 info"));
  QVERIFY(error2.level() == mdtErrorV2::Info);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  // Set error2 to a other error type
  error2.setError<double>(2.1, "error2.1", mdtErrorV2::Warning);
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<double>(), 2.1);
  QCOMPARE(error2.text(), QString("error2.1"));
  QVERIFY(error2.informativeText().isEmpty());
  QVERIFY(error2.level() == mdtErrorV2::Warning);
  // Check that error1 was not modified
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 1);
  QCOMPARE(error1.text(), QString("error1"));
  QCOMPARE(error1.informativeText(), QString("error1 info"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  /*
   * Check many copies of error
   */
  // Set error1
  error1.setError<int>(5, "shared error 5", mdtErrorV2::Info);
  // Copy construct 2 errors
  mdtErrorV2 error3(error1);
  mdtErrorV2 error4(error1);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<int>(), 5);
  QCOMPARE(error3.text(), QString("shared error 5"));
  QVERIFY(error3.level() == mdtErrorV2::Info);
  // Check error4
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  // Set error3
  error3.setError<double>(3.0, "error3", mdtErrorV2::Warning);
  // Check error3
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == mdtErrorV2::Warning);
  // Check that error1 is untouched
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<int>(), 5);
  QCOMPARE(error1.text(), QString("shared error 5"));
  QVERIFY(error1.level() == mdtErrorV2::Info);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  // Clear error1
  error1.clear();
  // Check error1
  QVERIFY(error1.isNull());
  // Check that error3 is untouched
  QVERIFY(!error3.isNull());
  QCOMPARE(error3.error<double>(), 3.0);
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == mdtErrorV2::Warning);
  // Check that error4 is untouched
  QVERIFY(!error4.isNull());
  QCOMPARE(error4.error<int>(), 5);
  QCOMPARE(error4.text(), QString("shared error 5"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
  /*
   * Copy assignment
   */
  // Set error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
  // Assign error1 to error2
  error2 = error1;
  // Check error2
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  // Set error1
  error1.setError<double>(1.1, "error1.1", mdtErrorV2::Warning);
  // Check error1
  QVERIFY(!error1.isNull());
  QCOMPARE(error1.error<double>(), 1.1);
  QCOMPARE(error1.text(), QString("error1.1"));
  QVERIFY(error1.level() == mdtErrorV2::Warning);
  // Check that error2 is untouched
  QVERIFY(!error2.isNull());
  QCOMPARE(error2.error<int>(), 1);
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  /*
   * Update tests
   * (setError() allways resets, check other setters)
   */
  // To check that COW works, we set error2 to other type than error1
  error2.setError<double>(2.5, "error2.5", mdtErrorV2::Warning);
  // Set error1
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
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

void mdtErrorTest::errorStackTest()
{
  std::vector<mdtErrorV2> errorStack;
  /*
   * Some check errors with empty stack, etc...
   */
  mdtErrorV2 error1;
  mdtErrorV2 error2;
  mdtErrorV2 error3;
  mdtErrorV2 error4;
  // Try to get stack from null error
  QVERIFY(error1.isNull());
  QVERIFY(error2.isNull());
  errorStack = error1.getErrorStack();
  QVERIFY(errorStack.empty());
  errorStack = error2.getErrorStack();
  QVERIFY(errorStack.empty());
  // Setup errors
  error1.setError("Error 1", mdtErrorV2::Error);
  error2.setError("Error 2", mdtErrorV2::Info);
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
  mdtErrorV2 systemFunctionError;
  mdtErrorV2 systemFunctionApiError;
  mdtErrorV2 helperFunctionError;
  mdtErrorV2 guiError;

  // First, system API function fails
  systemFunctionApiError.setError<int>(-1, "write access error", mdtErrorV2::Error);
  // Then, system function generates a error with some details and stacks API error
  systemFunctionError.setError<int>(-1, "Could not open file 'fake.txt'", mdtErrorV2::Error);
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
      guiError.setError("Could not open document 'fake.txt' because of missing write access", mdtErrorV2::Error);
      break;
    case -2:
      guiError.setError("Could not open document 'fake.txt' because it allready exists", mdtErrorV2::Error);
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
  error3.setError<long int>(30, "error3", mdtErrorV2::Warning);
  error4.setError<float>(4.0, "error4", mdtErrorV2::Info);
  // Set error1 and error2 to some error
  error1.setError<int>(1, "error1", mdtErrorV2::Error);
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

void mdtErrorTest::setSourceTest()
{
  // Constants for check
  const QString fileName = __FILE__;
  const QString functionName = metaObject()->className() + QLatin1String("::") + QLatin1String(__FUNCTION__) + QLatin1String("()");
  /*
   * Check creating error with helper macros
   */
  const int firstLine = __LINE__;
  mdtErrorV2 error1 = mdtErrorNew("error1", mdtErrorV2::Error, "mdtErrorTest");
  mdtErrorV2 error2 = mdtErrorNewT(int, 2, "error2", mdtErrorV2::Error, "mdtErrorTest");
  mdtErrorV2 error3 = mdtErrorNewQ("error3", mdtErrorV2::Info, this);
  mdtErrorV2 error4 = mdtErrorNewTQ(float, 4.0, "error4", mdtErrorV2::Info, this);
  // Check error1
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  QCOMPARE(error1.fileName(), fileName);
  QCOMPARE(error1.fileLine(), firstLine + 1);
  QCOMPARE(error1.functionName(), functionName);
  // Check error2
  QCOMPARE(error2.error<int>(), 2);
  QCOMPARE(error2.text(), QString("error2"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  QCOMPARE(error2.fileName(), fileName);
  QCOMPARE(error2.fileLine(), firstLine + 2);
  QCOMPARE(error2.functionName(), functionName);
  // Check error3
  QCOMPARE(error3.text(), QString("error3"));
  QVERIFY(error3.level() == mdtErrorV2::Info);
  QCOMPARE(error3.fileName(), fileName);
  QCOMPARE(error3.fileLine(), firstLine + 3);
  QCOMPARE(error3.functionName(), functionName);
  // Check error4
  QCOMPARE(error4.error<float>(), 4.0);
  QCOMPARE(error4.text(), QString("error4"));
  QVERIFY(error4.level() == mdtErrorV2::Info);
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
  QVERIFY(error2.level() == mdtErrorV2::Error);
  QCOMPARE(error2.fileName(), fileName);
  QCOMPARE(error2.fileLine(), firstLine + 1);
  QCOMPARE(error2.functionName(), functionName);
  // Update error2
  error2.setSource("file.cpp", 36, "C", "F");
  // Check error2
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  QCOMPARE(error2.fileName(), QString("file.cpp"));
  QCOMPARE(error2.fileLine(), 36);
  QCOMPARE(error2.functionName(), QString("C::F()"));
  // Check that error1 is untouched
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  QCOMPARE(error1.fileName(), fileName);
  QCOMPARE(error1.fileLine(), firstLine + 1);
  QCOMPARE(error1.functionName(), functionName);
  // Update source of error1
  error1.setSource("file1.cpp", 35, "C1", "F1");
  // Check error1
  QCOMPARE(error1.text(), QString("error1"));
  QVERIFY(error1.level() == mdtErrorV2::Error);
  QCOMPARE(error1.fileName(), QString("file1.cpp"));
  QCOMPARE(error1.fileLine(), 35);
  QCOMPARE(error1.functionName(), QString("C1::F1()"));
  // Check that error2 is untouched
  QCOMPARE(error2.text(), QString("error1"));
  QVERIFY(error2.level() == mdtErrorV2::Error);
  QCOMPARE(error2.fileName(), QString("file.cpp"));
  QCOMPARE(error2.fileLine(), 36);
  QCOMPARE(error2.functionName(), QString("C::F()"));
}

void mdtErrorTest::errorLoggerConsoleBackendTest()
{
  mdt::error::LoggerConsoleBackend backend;

  mdtErrorV2 error = mdtErrorNewTQ(int, 25, "error", mdtErrorV2::Error, this);

  error.setInformativeText("Some more info");
  backend.logError(error);
}

void mdtErrorTest::errorLoggerFileBackendTest()
{
  mdt::error::LoggerFileBackend backend;
  mdtErrorV2 error; // = mdtErrorNewTQ(int, 25, "error", mdtErrorV2::Error, this);
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
  QCOMPARE(backend.maxFileSize(), 1024);
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
  error.setError("1234567890", mdtErrorV2::Error);
  backend.logError(error);
  // Check
  QVERIFY(!QFile::exists(backend.backupLogFilePath()));
  QVERIFY(logFile.open(QIODevice::ReadOnly | QIODevice::Text));
  logFileData = logFile.readAll();
  logFile.close();
  QVERIFY(!logFileData.isEmpty());
  QVERIFY(logFileData.contains("1234567890"));
  // Log error
  error.setError("ABCDEFGH", mdtErrorV2::Error);
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

void mdtErrorTest::errorLoggerTest()
{
  using namespace mdt::error;

  LoggerGuard loggerGard;
  auto fileBackend = std::make_shared<LoggerFileBackend>();
  std::shared_ptr<mdtErrorLoggerTestBackend> backend(new mdtErrorLoggerTestBackend);
  
  QTemporaryFile tmpFile;
  QVERIFY(tmpFile.open());
  tmpFile.close();

  fileBackend->setLogFilePath(tmpFile.fileName());
  
  ///Logger::addBackend(std::make_shared<LoggerConsoleBackend>());
  Logger::addBackend(fileBackend);
  // Use our sentinel backend
  ///Logger::addBackend(std::make_shared<mdtErrorLoggerTestBackend>());
  Logger::addBackend(backend);

  
  
  auto error1 = mdtErrorNewTQ(int, 1, "error1", mdtErrorV2::Error, this);
  Logger::logError(error1);
  Logger::cleanup();
  
  /**
   * \todo Mix logError() cleanup() without wait + wait between.
   */
  
  QVERIFY(tmpFile.open());
  qDebug() << "Logfile: " << tmpFile.readAll();
}

void mdtErrorTest::errorLoggerConcurrentAccessTest()
{
  

  
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtErrorTest errorTest;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&errorTest, argc, argv);
}
