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
#ifndef MDT_CABLE_LIST_TEST_H
#define MDT_CABLE_LIST_TEST_H

#include "mdtTest.h"
#include "mdtCableListTestScenario.h"
#include "mdtSqlDatabaseManager.h"
#include <QMessageBox>
#include <QFileInfo>

class mdtCableListTest : public mdtTest
{
 Q_OBJECT

 private slots:

  /*
   * Will create database schema (see createDatabaseSchema() )
   * No data is inserted in tables by this method.
   * Each test also has a empty schema at startup,
   *  and must assure that they finish with a empty schema at end.
   */
  void initTestCase();

  void cleanupTestCase();

  void applicationWidgetsTest();

//   void articleConnectionDataTest();
//   void articleConnectorDataTest();
  void articleTest();
  void unitConnectionUpdateTest();

//   void unitConnectionDataTest();
//   void unitConnectorDataTest();
  void unitTest();
//   void unitConnectionTest();

//   void mdtClLinkDataTest();
  void linkTest();  /// \todo Ne pas oublier: UnitLink_view et LinkList_view !
  void linkUpdateFromArticleLinkTest();
//   void linkConnectableConnectorTest();
//   void linkAutoConnectionTest();

  void pathGraphTest();
  void directLinkTest();

  void linkBeamTest();

 private:

  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  mdtSqlDatabaseManager pvDatabaseManager;
  QFileInfo pvDbFileInfo;
};

#endif // #ifndef MDT_CABLE_LIST_TEST_H
