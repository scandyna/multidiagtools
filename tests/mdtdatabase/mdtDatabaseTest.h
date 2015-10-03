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
#ifndef MDT_DATABASE_TEST_H
#define MDT_DATABASE_TEST_H

#include "mdtTest.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>

class mdtDatabaseTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void sqlDriverTypeTest();

  void sqlFieldTypeTest();

  void sqlCollationTest();

  void sqlFieldTest();
  void sqlFieldSQLiteTest();

  void sqlIndexBaseTest();
  void sqlIndexTest();
  void sqlPrimaryKeySqliteTest();

  void sqlSchemaTableTest();
  void sqlSchemaTableSqliteTest();
  void sqlTableSchemaModelTest();

  void sqlTableSetupWidgetTest();

  void sqlFieldSetupWidgetTest();
  void sqlFieldSetupDialogTest();

  void sqlViewSchemaJoinClauseTest();
  void sqlViewSchemaTest();
  void sqlViewSchemaBenchmark();

  void sqlTablePopulationSchemaTest();

  void sqlDatabaseSchemaTest();
  void sqlDatabaseSchemaModelTest();
  void sqlDatabaseSchemaDialogTest();
  void sqlDatabaseSchemaThreadTest();

  void connectionNameWidgetTest();
  void basicInfoWidgetTest();
  void databaseSqliteTest();
  void databaseDialogSqliteTest();

  void databaseManagerTest();

  void sqlTransactionTest();

  /// \todo Add mdtSqlForm test
  /// \todo Add mdtSqlWindow test

  void sqlRecordTest();

  void sqlRelationInfoTest();
  void sqlRelationTest();

  void doubleValueTest();

 private:

  /*
   * Create the test database
   */
  void createTestDatabase();
  // Populate database with some common data
  void populateTestDatabase();
  // Clear test database data
  void clearTestDatabaseData();

  QFileInfo pvDatabaseFileInfo;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_DATABASE_TEST_H
