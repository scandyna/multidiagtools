/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtError.h"
#include "mdtErrorOut.h"
#include "mdtApplication.h"
#include <QTest>
#include <QFile>

/// \todo Check if number is usable anymore, and update test if needed.
void mdtErrorTest::simpleTest()
{
  mdtError e(1, "One", mdtError::Error);
  enum errorType { A = -1, B, C};

  // Check given values
  QVERIFY(e.number() == 1);
  QVERIFY(e.text() == "One");
  QVERIFY(e.level() == mdtError::Error);
  // Check default values
  QVERIFY(e.systemNumber() == 0);
  QVERIFY(e.systemText() == "");
  QVERIFY(e.functionName() == "");
  QVERIFY(e.fileName() == "");
  QVERIFY(e.fileLine() == 0);
  // Add system error
  e.setSystemError(-25, "Bad fd");
  QVERIFY(e.systemNumber() == -25);
  QVERIFY(e.systemText() == "Bad fd");
  // Add source
  MDT_ERROR_SET_SRC(e, "mdtErrorTest");
  QVERIFY(e.fileLine() == (__LINE__ - 1));
  //QVERIFY(e.fileName() == __FILE__);
  QVERIFY(e.functionName() == "mdtErrorTest::simpleTest()");
  /*
   * Check type storage
   */
  e.setType<errorType>(A);
  QVERIFY(e.type<errorType>() == A);
  /*
   * Following expressions must not compile
   */
  //e.setType<int>(0);
  //QVERIFY(e.type<int>() == 0);
  //auto a = e.type<int>();
}

void mdtErrorTest::errorOutInitTest()
{
  QString oldLogFile;

  QFile::remove("mdtErrorTest.log");
  // An error out instance was initialized in main, destroy it
  oldLogFile = mdtErrorOut::logFile();
  mdtErrorOut::destroy();
  // Verify that instance is initially null
  QVERIFY(mdtErrorOut::instance() == 0);
  // Check that init works
  QVERIFY(mdtErrorOut::init("mdtErrorTest.log"));
  QVERIFY(mdtErrorOut::instance() != 0);
  QVERIFY(QFile::exists("mdtErrorTest.log"));
  // Second call of init must fail
  QVERIFY(!mdtErrorOut::init("mdtErrorTest.log"));
  // Delete the instance
  mdtErrorOut::destroy();
  QVERIFY(mdtErrorOut::instance() == 0);
  // No log was written, log file must be removed
  QVERIFY(!QFile::exists("mdtErrorTest.log"));
  // Re-init
  QVERIFY(mdtErrorOut::init("mdtErrorTest.log"));
  QVERIFY(mdtErrorOut::instance() != 0);
  mdtErrorOut::setDialogLevelsMask(0);
  QVERIFY(QFile::exists("mdtErrorTest.log"));
  // Add a error
  mdtError e1(1 , "Error 1", mdtError::Error);
  e1.commit();
  // Delete the instance
  mdtErrorOut::destroy();
  QVERIFY(mdtErrorOut::instance() == 0);
  // Log was written, log file must not be removed
  QVERIFY(QFile::exists("mdtErrorTest.log"));

  // Re-init the original out
  QVERIFY(mdtErrorOut::init(oldLogFile));
  mdtErrorOut::setDialogLevelsMask(0);
}

void mdtErrorTest::errorOutAddTest()
{
  mdtError e(24, "Error 24", mdtError::Warning);

  // Setup error
  e.setSystemError(-85, "Arguments invalides");
  MDT_ERROR_SET_SRC(e, "mdtErrorTest");
  e.commit();
}

void mdtErrorTest::errorOutBackupTest()
{
  QString oldLogFile;
  mdtError e1(1 , "Error 1", mdtError::Error);
  mdtError e2(2 , "Error 2", mdtError::Error);
  mdtError e3(3 , "Error 3", mdtError::Error);

  MDT_ERROR_SET_SRC(e1, "mdtErrorTest");
  MDT_ERROR_SET_SRC(e2, "mdtErrorTest");
  MDT_ERROR_SET_SRC(e3, "mdtErrorTest");

  // An error out instance was initialized in main, destroy it
  oldLogFile = mdtErrorOut::logFile();
  mdtErrorOut::destroy();
  QVERIFY(mdtErrorOut::instance() == 0);

  // Init
  QFile::remove("mdtErrorTest.log");
  QFile::remove("mdtErrorTest.log.bak");
  QFile::remove("mdtErrorTest.log.bak.bak");
  QVERIFY(!QFile::exists("mdtErrorTest.log"));
  QVERIFY(!QFile::exists("mdtErrorTest.log.bak"));
  QVERIFY(!QFile::exists("mdtErrorTest.log.bak.bak"));
  QVERIFY(mdtErrorOut::init("mdtErrorTest.log"));
  QVERIFY(mdtErrorOut::instance() != 0);
  mdtErrorOut::setDialogLevelsMask(0);
  mdtErrorOut::setLogFileMaxSize(300);

  // Add a first error , and check that logfile exists, but no backup
  e1.commit();
  QTest::qWait(100);
  QVERIFY(QFile::exists("mdtErrorTest.log"));
  QVERIFY(!QFile::exists("mdtErrorTest.log.bak"));
  // Add a second error , and check that backup was created
  e2.commit();
  QTest::qWait(100);
  QVERIFY(!QFile::exists("mdtErrorTest.log"));
  QVERIFY(QFile::exists("mdtErrorTest.log.bak"));
  // Add a error , and check that logfile exists
  e3.commit();
  QTest::qWait(100);
  QVERIFY(QFile::exists("mdtErrorTest.log"));
  QVERIFY(QFile::exists("mdtErrorTest.log.bak"));
  QVERIFY(!QFile::exists("mdtErrorTest.log.bak.bak"));

  // Remove created files
  QFile::remove("mdtErrorTest.log");
  QFile::remove("mdtErrorTest.log.bak");

  // Delete the instance
  mdtErrorOut::destroy();
  QVERIFY(mdtErrorOut::instance() == 0);
  // Re-init the original out
  QVERIFY(mdtErrorOut::init(oldLogFile));
  mdtErrorOut::setDialogLevelsMask(0);
}

void mdtErrorTest::copyTest()
{
  mdtError e1("One", mdtError::Warning);
  mdtError e2;

  // Check e1 and e2
  QCOMPARE(e1.text(), QString("One"));
  QVERIFY(e1.level() == mdtError::Warning);
  QCOMPARE(e2.text(), QString(""));
  QVERIFY(e2.level() == mdtError::NoError);
  // Copy and check
  e2 = e1;
  QCOMPARE(e1.text(), QString("One"));
  QVERIFY(e1.level() == mdtError::Warning);
  QCOMPARE(e2.text(), QString("One"));
  QVERIFY(e2.level() == mdtError::Warning);
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtErrorTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
