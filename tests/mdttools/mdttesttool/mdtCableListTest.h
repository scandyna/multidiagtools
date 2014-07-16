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
#ifndef MDT_CABLE_LIST_TEST_H
#define MDT_CABLE_LIST_TEST_H

#include "mdtTest.h"
#include "mdtCableListTestScenario.h"
#include "mdtSqlDatabaseManager.h"
#include <boost/graph/graph_concepts.hpp> /// \todo Utile ?
#include <QMessageBox>

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

  void sandbox();
  
  void articleConnectionDataTest();
  void articleConnectorDataTest();
  void articleTest();

  void unitConnectionDataTest();
  void unitConnectorDataTest();
  void unitTest();
  void unitConnectionTest();
  /// Créer des fonction (pas slot) qui construisent à mesure un sénario

  void mdtClLinkDataTest();
  void linkTest();  /// \todo Ne pas oublier: UnitLink_view et LinkList_view !
  void linkAutoConnectionTest();

  void pathGraphTest();
  void directLinkTest();

  void linkBeamTest();

 private:

  /*
   * Create some vehicle types for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestVehicleTypes();
  /*
   * Remove test vehicle types
   */
  void removeTestVehicleTypes();

  /*
   * Create some connectors for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestConnectors();
  /*
   * Remove test connectors
   */
  void removeTestConnectors();

  /*
   * Create some articles for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestArticles();
  /*
   * Remove test articles
   */
  void removeTestArticles();

  /*
   * Create some article connections for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestArticleConnections();
  /*
   * Remove test article connections
   */
  void removeTestArticleConnections();

  /*
   * Create some article connectors for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestArticleConnectors();
  /*
   * Remove test article connectors
   */
  void removeTestArticleConnectors();

  /*
   * Create some article links for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestArticleLinks();
  /*
   * Remove test article links
   */
  void removeTestArticleLinks();

  /*
   * Create some units for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestUnits();
  /*
   * Remove test units
   */
  void removeTestUnits();

  /*
   * Create some VehicleType-Unit assignations for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestVehicleTypeUnitAssignations();
  /*
   * Remove VehicleType-Unit assignations
   */
  void removeTestVehicleTypeUnitAssignations();

  /*
   * Create some unit connections for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestUnitConnections();
  /*
   * Remove test unit connections
   */
  void removeTestUnitConnections();

  /*
   * Create some unit connectors for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestUnitConnectors();
  /*
   * Remove test unit connectors
   */
  void removeTestUnitConnectors();

  /*
   * Create links for tests
   *  See mdtCableListTestScenario class for details.
   */
  void createTestLinks();
  /*
   * Remove test links
   */
  void removeTestLinks();

  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  mdtSqlDatabaseManager pvDatabaseManager;
  mdtCableListTestScenario *pvScenario;
};

#endif // #ifndef MDT_CABLE_LIST_TEST_H
