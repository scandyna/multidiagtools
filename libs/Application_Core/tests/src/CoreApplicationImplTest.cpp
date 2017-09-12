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
#include "CoreApplicationImplTest.h"
#include "Mdt/CoreApplicationImpl.h"
#include <QFileInfo>
#include <QFile>

#include <QDebug>

using namespace Mdt;

void CoreApplicationImplTest::initTestCase()
{
}

void CoreApplicationImplTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void CoreApplicationImplTest::fileLoggingSetupTest()
{
  /*
   * Initial state
   */
  CoreApplicationImpl app;
  QVERIFY(app.logFilePath().isEmpty());
  QVERIFY(!app.isFileLoggingEnabled());
  /*
   * Enable file logging
   */
  app.enableFileLogging(CoreApplicationImpl::ApplicationName);
  QVERIFY(!app.logFilePath().isEmpty());
  QVERIFY(app.isFileLoggingEnabled());
  QFileInfo logFileInfo(app.logFilePath());
  QVERIFY(logFileInfo.isAbsolute());
  /*
   * Check that log file exists and is writable,
   * or it can be created.
   */
  if(logFileInfo.exists()){
    QVERIFY(logFileInfo.isFile());
    QVERIFY(logFileInfo.isWritable());
  }else{
    QVERIFY(!logFileInfo.isDir());
    QFile logFile(logFileInfo.absoluteFilePath());
    QVERIFY(logFile.open(QFile::WriteOnly));
    logFile.remove();
  }
}

void CoreApplicationImplTest::cacheDiretorySetupTest()
{
  CoreApplicationImpl app;
  QFileInfo dir(CoreApplicationImpl::cacheDirectoryPath());
  QVERIFY(dir.exists());
  QVERIFY(dir.isDir());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CoreApplicationImplTest test;

  return QTest::qExec(&test, argc, argv);
}
