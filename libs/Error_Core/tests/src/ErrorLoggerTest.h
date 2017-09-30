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
#ifndef MDT_ERROR_LOGGER_TEST_H
#define MDT_ERROR_LOGGER_TEST_H

#include "Mdt/Error.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QThread>
#include <QVector>
#include <QMetaType>
// #include <atomic>

/*
 * Also usable by data driven tests
 */
using ErrorVectorType = QVector<Mdt::Error>;
Q_DECLARE_METATYPE(ErrorVectorType);

/*
 * Helper function to log errors
 */
void logErrors(const ErrorVectorType & errorList, int interLogWaitTime = 0);

/*
 * Thread that logs errors
 */
class ErrorProducerThread : public QThread
{
 Q_OBJECT

 public:

  ErrorProducerThread(const ErrorVectorType & errorList, int interLogWaitTime = 0,  QObject* parent = nullptr)
   : QThread(parent),
     mInterLogWaitTime(interLogWaitTime),
     mErrorList(errorList)
  {
  }

  ErrorProducerThread(const Mdt::Error & error,  QObject* parent = nullptr)
   : QThread(parent),
     mInterLogWaitTime(0),
     mErrorList({error})
  {
  }

 private:

  void run() override
  {
    logErrors(mErrorList, mInterLogWaitTime);
  }

  /*
   * As long as we pass all attributes
   * in the constructor, and never change them later,
   * we do not need synchronization.
   */
  int mInterLogWaitTime;
  ErrorVectorType mErrorList;
//   std::atomic<int> mInterLogWaitTime;
};

class ErrorLoggerTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void loggerMainThreadTest();
  void loggerMainThreadTest_data();
  void loggerSeparateThreadTest();
  void loggerSeparateThreadTest_data();

 private:

  void createLoggerTestData();

 private slots:

  void mainThreadConcurrentAccessTest();
  void mainThreadConcurrentAccessTest_data();
  void separateThreadConcurrentAccessTest();
  void separateThreadConcurrentAccessTest_data();

 private:

  void createConcurrentAccessTestData();

 private slots:

  void consoleOutFromMainThreadTest();
  void consoleOutFromMainThreadTest_data();
  void consoleOutFromOtherThreadTest();
  void consoleOutFromOtherThreadTest_data();
  void consoleOutFromMultipleThreadsTest();
  void consoleOutFromMultipleThreadsTest_data();
  void consoleOutBenchmark();

  void fileOutFromMultipleThreadsTest();
  void fileOutFromMultipleThreadsTest_data();
  void fileOutBenchmark();

 private:

  void createOutputTestData();
  static Mdt::Error buildError(int number, Mdt::Error::Level level);
  static ErrorVectorType buildErrorList(int n);
};

#endif // #ifndef MDT_ERROR_LOGGER_TEST_H
