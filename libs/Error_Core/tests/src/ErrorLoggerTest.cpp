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
#include "ErrorLoggerTest.h"
#include <QCoreApplication>
#include "Mdt/ErrorLogger/Logger.h"
#include "Mdt/ErrorLogger/Backend.h"
#include "Mdt/ErrorLogger/ConsoleBackend.h"
#include "Mdt/ErrorLogger/FileBackend.h"
#include <QLatin1String>
#include <QString>
#include <QTemporaryFile>

#include <vector>
#include <memory>

#include <QDebug>

using namespace Mdt::ErrorLogger;

/*
 * Helper functions
 */

void logErrors(const ErrorVectorType & errorList, int interLogWaitTime)
{
  for(const auto & error : errorList){
    Q_ASSERT(!error.isNull());
    Logger::logError(error);
    if(interLogWaitTime > 0){
      QTest::qWait(interLogWaitTime);
    }
  }
}

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
 * Init/cleanup
 */

void ErrorLoggerTest::initTestCase()
{
}

void ErrorLoggerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ErrorLoggerTest::loggerTest()
{
  LoggerGuard loggerGard;
  auto testBackend = std::make_shared<ErrorLoggerTestBackend>();  /// \todo Why using shared pointer ?
  QFETCH(ErrorVectorType, errorList); // ErrorVectorType == QVector<Mdt::Error>
  QFETCH(int, interLogWaitTime);
  QFETCH(int, endWaitTime);

  // Setup logger
  Logger::addBackend(testBackend);
  // Log errors
  logErrors(errorList, interLogWaitTime);
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

void ErrorLoggerTest::loggerTest_data()
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

void ErrorLoggerTest::loggerConcurrentAccessTest()
{
  LoggerGuard loggerGard;
  auto testBackend = std::make_shared<ErrorLoggerTestBackend>();  /// \todo Why using shared pointer ?
  QFETCH(int, threadsCount);

  // Build list or errors
  ErrorVectorType errorList;
  for(int i = 0; i <= threadsCount; ++i){
    auto error = mdtErrorNewQ("error" + QString::number(i), Mdt::Error::Critical, this);
    errorList.append(error);
  }
  // Setup logger
  Logger::addBackend(testBackend);
  // Create threads
  QVector<ErrorProducerThread*> threadList;
  for(int i = 0; i < errorList.size(); ++i){
    auto *thd = new ErrorProducerThread(errorList.at(i));
    threadList.append(thd);
    thd->start();
  }
  // Join our threads
  for(auto & thd : threadList){
    thd->wait();
    delete thd;
  }
  threadList.clear();
  // Explicitly cleanup here (to join logger thread)
  Logger::cleanup();
  // Check that all errors where logged
  QCOMPARE((int)testBackend->errorList.size(), errorList.size());
}

void ErrorLoggerTest::loggerConcurrentAccessTest_data()
{
  QTest::addColumn<int>("threadsCount");

  QTest::newRow("1") << 1;
  QTest::newRow("2") << 2;
  QTest::newRow("3") << 3;
  QTest::newRow("4") << 4;
  QTest::newRow("5") << 5;
  QTest::newRow("6") << 6;
  QTest::newRow("7") << 7;
  QTest::newRow("8") << 8;
  QTest::newRow("9") << 9;
  QTest::newRow("10") << 10;
  QTest::newRow("11") << 11;
  QTest::newRow("12") << 12;
  QTest::newRow("13") << 13;
  QTest::newRow("14") << 14;
  QTest::newRow("15") << 15;
  QTest::newRow("16") << 16;
  QTest::newRow("17") << 17;
  QTest::newRow("18") << 18;
  QTest::newRow("19") << 19;
  QTest::newRow("20") << 20;
}

void ErrorLoggerTest::consoleOutFromMainThreadTest()
{
  /*
   * We only check that we have no crash here
   */
  QFETCH(ErrorVectorType, errorList);
  QFETCH(int, interLogWaitTime);

  // Setup logger
  LoggerGuard loggerGard;
  auto backend = std::make_shared<ConsoleBackend>();  /// \todo Why using shared pointer ?
  Logger::addBackend(backend);
  // Log errors
  logErrors(errorList, interLogWaitTime);
}

void ErrorLoggerTest::consoleOutFromMainThreadTest_data()
{
  createOutputTestData();
}

void ErrorLoggerTest::consoleOutFromOtherThreadTest()
{
  /*
   * We only check that we have no crash here
   */
  QFETCH(ErrorVectorType, errorList);
  QFETCH(int, interLogWaitTime);

  // Setup logger
  LoggerGuard loggerGard;
  auto backend = std::make_shared<ConsoleBackend>();  /// \todo Why using shared pointer ?
  Logger::addBackend(backend);
  // Log errors
  ErrorProducerThread thread(errorList, interLogWaitTime);
  thread.start();
  thread.wait();
}

void ErrorLoggerTest::consoleOutFromOtherThreadTest_data()
{
  createOutputTestData();
}

void ErrorLoggerTest::consoleOutFromMultipleThreadsTest()
{
  /*
   * We only check that we have no crash here
   */
  QFETCH(ErrorVectorType, errorList);
  QFETCH(int, interLogWaitTime);
  QFETCH(int, nonMainThreadCount);

  // Setup logger
  LoggerGuard loggerGard;
  auto backend = std::make_shared<ConsoleBackend>();  /// \todo Why using shared pointer ?
  Logger::addBackend(backend);
  // Log errors from other threads
  QVector<ErrorProducerThread*> threadList;
  for(int i = 0; i < nonMainThreadCount; ++i){
    auto *thd = new ErrorProducerThread(errorList, interLogWaitTime);
    threadList.append(thd);
    thd->start();
  }
  // Log errors from main thread
  logErrors(errorList, interLogWaitTime);
  // Join our produces threads
  for(auto & thd : threadList){
    thd->wait();
    delete thd;
  }
  threadList.clear();
}

void ErrorLoggerTest::consoleOutFromMultipleThreadsTest_data()
{
  createOutputTestData();
}

void ErrorLoggerTest::consoleOutBenchmark()
{
  LoggerGuard loggerGard;
  auto backend = std::make_shared<ConsoleBackend>();  /// \todo Why using shared pointer ?
  Logger::addBackend(backend);
  auto error = mdtErrorNewQ("error 1", Mdt::Error::Warning, this);
  QBENCHMARK_ONCE{
    Logger::logError(error);
  }
}

void ErrorLoggerTest::fileOutFromMultipleThreadsTest()
{
  /*
   * We only check that we have no crash here
   */
  QFETCH(ErrorVectorType, errorList);
  QFETCH(int, interLogWaitTime);
  QFETCH(int, nonMainThreadCount);

  // Setup logger
  QTemporaryFile logFile;
  QVERIFY(logFile.open());
  LoggerGuard loggerGard;
  auto backend = std::make_shared<FileBackend>();  /// \todo Why using shared pointer ?
  QVERIFY(backend->setLogFilePath(logFile.fileName()));
  Logger::addBackend(backend);
  // Log errors from other threads
  QVector<ErrorProducerThread*> threadList;
  for(int i = 0; i < nonMainThreadCount; ++i){
    auto *thd = new ErrorProducerThread(errorList, interLogWaitTime);
    threadList.append(thd);
    thd->start();
  }
  // Log errors from main thread
  logErrors(errorList, interLogWaitTime);
  // Join our produces threads
  for(auto & thd : threadList){
    thd->wait();
    delete thd;
  }
  threadList.clear();
  Logger::cleanup();
  QVERIFY(logFile.size() > 0);
}

void ErrorLoggerTest::fileOutFromMultipleThreadsTest_data()
{
  createOutputTestData();
}

void ErrorLoggerTest::fileOutBenchmark()
{
  QTemporaryFile logFile;
  QVERIFY(logFile.open());
  LoggerGuard loggerGard;
  auto backend = std::make_shared<FileBackend>();  /// \todo Why using shared pointer ?
  QVERIFY(backend->setLogFilePath(logFile.fileName()));
  Logger::addBackend(backend);
  auto error = mdtErrorNewQ("error 1", Mdt::Error::Warning, this);
  QBENCHMARK_ONCE{
    Logger::logError(error);
  }
}

void ErrorLoggerTest::createOutputTestData()
{
  QTest::addColumn<ErrorVectorType>("errorList");
  QTest::addColumn<int>("interLogWaitTime");
  QTest::addColumn<int>("nonMainThreadCount");

  QTest::newRow("1,0,0") << buildErrorList(1) << 0 << 0;
  QTest::newRow("1,0,1") << buildErrorList(1) << 0 << 1;
  QTest::newRow("1,0,2") << buildErrorList(1) << 0 << 2;
  QTest::newRow("2,0,0") << buildErrorList(2) << 0 << 0;
  QTest::newRow("2,1,0") << buildErrorList(2) << 1 << 0;
  QTest::newRow("2,0,1") << buildErrorList(2) << 0 << 1;
  QTest::newRow("2,1,1") << buildErrorList(2) << 1 << 1;
  QTest::newRow("2,0,2") << buildErrorList(2) << 0 << 2;
  QTest::newRow("2,1,2") << buildErrorList(2) << 1 << 2;
  QTest::newRow("3,0,0") << buildErrorList(3) << 0 << 0;
  QTest::newRow("3,2,0") << buildErrorList(3) << 2 << 0;
  QTest::newRow("3,2,1") << buildErrorList(3) << 2 << 1;
}

Mdt::Error ErrorLoggerTest::buildError(int number, Mdt::Error::Level level)
{
  return mdtErrorNew(QLatin1String("Error ") + QString::number(number), level, "ErrorLoggerTest");
}

ErrorVectorType ErrorLoggerTest::buildErrorList(int n)
{
  ErrorVectorType errorList;

  for(int i = 0; i < n; ++i){
    errorList << buildError(i, Mdt::Error::Warning);
  }

  return errorList;
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  ErrorLoggerTest test;

  return QTest::qExec(&test, argc, argv);
}
