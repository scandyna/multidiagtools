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
#ifndef MDT_TT_TEST_SYSTEM_COMPONENT_TEST_H
#define MDT_TT_TEST_SYSTEM_COMPONENT_TEST_H

#include "mdtTest.h"
#include "mdtSqlDatabaseManager.h"
#include <QString>
#include <QVariant>
#include <QSqlDatabase>

/*
 * Create some basic datasets for tests
 */
class mdtTtTestSystemComponentTestData
{
 public:

  // Constructor
  mdtTtTestSystemComponentTestData(QSqlDatabase db)
   : pvDatabase(db)
  {
  }
  // Destructor - will also remove created data
  ~mdtTtTestSystemComponentTestData()
  {
    clear();
  }
  /*
   * Create test systems
   * Will add following test systems:
   *  - Id_PK: 1, NameEN: Test system 01
   *  - Id_PK: 2, NameEN: Test system 02
   */
  bool populateTestSystems();
  // Remove test systems
  void removeTestSystems();
  /*
   * Create test system components
   * Will add following components:
   *  - Id_PK: 11, Alias: TestSysComp11, NameEN: Test system component 11
   *  - Id_PK: 12, Alias: TestSysComp12, NameEN: Test system component 12
   *  - Id_PK: 13, Alias: TestSysComp13, NameEN: Test system component 13
   */
  bool populateTestSystemComponents();
  // Remove test system components
  void removeTestSystemComponents();
  // Clear craeted data
  void clear();

 private:

  /// \todo Candidates for a common base class
  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();
  void debugSqlError(const QSqlError & error);
  void debugSqlError(const QSqlQuery & query);

  Q_DISABLE_COPY(mdtTtTestSystemComponentTestData);

  QSqlDatabase pvDatabase;
};

/*
 * Test system component test
 */
class mdtTtTestSystemComponentTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Will create database schema (see createDatabaseSchema() )
  void initTestCase();
  void cleanupTestCase();

  void testSystemAssignationTest();
  void createUnitTest();

 private:

  // Create test database schema - Will FAIL on problem
  /// \todo Candidates for a common base class
  void createDatabaseSchema();

  mdtSqlDatabaseManager pvDatabaseManager;
  QFileInfo pvDbFileInfo;
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_COMPONENT_TEST_H
