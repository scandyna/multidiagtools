
#include "mdtErrorTest.h"
#include "mdtError.h"
#include "mdtErrorOut.h"
#include <QTest>

void mdtErrorTest::simpleTest()
{
  mdtError e(1, "One", mdtError::Error);

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
  QVERIFY(e.fileName() == __FILE__);
  QVERIFY(e.functionName() == "mdtErrorTest::simpleTest()");
}

void mdtErrorTest::errorOutInitTest()
{
  QString oldLogFile;
  
  // An error out instance was initialized in main, destroy it
  oldLogFile = mdtErrorOut::logFile();
  mdtErrorOut::destroy();
  // Verify that instance is initially null
  QVERIFY(mdtErrorOut::instance() == 0);
  // Chcek that init works
  QVERIFY(mdtErrorOut::init("mdtErrorTest.log"));
  QVERIFY(mdtErrorOut::instance() != 0);
  // Second call of init must fail
  QVERIFY(!mdtErrorOut::init("mdtErrorTest.log"));
  // Delete the instance
  mdtErrorOut::destroy();
  QVERIFY(mdtErrorOut::instance() == 0);
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
  // Add error
  //mdtErrorOut::addError(e);
  e.commit();
}
