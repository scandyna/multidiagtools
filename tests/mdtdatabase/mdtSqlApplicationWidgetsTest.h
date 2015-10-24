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
#ifndef MDT_SQL_APPLICATION_WIDGETS_TEST_H
#define MDT_SQL_APPLICATION_WIDGETS_TEST_H

#include "mdtTest.h"
#include "mdtSqlApplicationWidgets.h"
#include <QSqlDatabase>
#include <QTemporaryFile>

/*
 * Concrete application widgets test class
 */
class TestApplicationWidgets : public mdtSqlApplicationWidgets<TestApplicationWidgets>
{
 Q_OBJECT

  friend class mdtSqlApplicationWidgets<TestApplicationWidgets>;

 public:

  void clearAllWidgets()
  {
    qDebug() << "Clear all widgets ...";
  }
  static void doSomeThing(){
  //instance().displayError(mdtError("Test", mdtError::Warning));
  }

 private:

  TestApplicationWidgets()
  {
    qDebug() << "TestApplicationWidgets() constructor ...";
  }

  ~TestApplicationWidgets()
  {
    qDebug() << "~TestApplicationWidgets() destructor ...";
  }

  Q_DISABLE_COPY(TestApplicationWidgets);
};

/*
 * Test class
 */
class mdtSqlApplicationWidgetsTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void simpleTest();

 private:

  QTemporaryFile pvTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase pvDatabase;

};

#endif // #ifndef MDT_SQL_APPLICATION_WIDGETS_TEST_H
