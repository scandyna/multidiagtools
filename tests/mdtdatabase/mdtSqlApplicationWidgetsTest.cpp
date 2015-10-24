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
#include "mdtSqlApplicationWidgetsTest.h"
#include "mdtApplication.h"

#include <memory>

#include <QPushButton>
#include <QPointer>

#include <QDebug>

void mdtSqlApplicationWidgetsTest::initTestCase()
{
  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
}

void mdtSqlApplicationWidgetsTest::cleanupTestCase()
{
}

void mdtSqlApplicationWidgetsTest::simpleTest()
{
  // Following line must not compile
  // TestApplicationWidgets twa;

  mdtSqlApplicationWidgetsGuard<TestApplicationWidgets> testAppWidgetsGuard;

  /*
   * First calls
   */
  QVERIFY(!TestApplicationWidgets::database().isValid());
  QVERIFY(!TestApplicationWidgets::database().isOpen());
  TestApplicationWidgets::setDatabase(pvDatabase);
  QVERIFY(TestApplicationWidgets::database().isValid());
  QVERIFY(TestApplicationWidgets::database().isOpen());
  /*
   * Destruct and repeat
   */
  TestApplicationWidgets::destruct();
  QVERIFY(!TestApplicationWidgets::database().isValid());
  QVERIFY(!TestApplicationWidgets::database().isOpen());
  TestApplicationWidgets::setDatabase(pvDatabase);
  QVERIFY(TestApplicationWidgets::database().isValid());
  QVERIFY(TestApplicationWidgets::database().isOpen());

}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSqlApplicationWidgetsTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
