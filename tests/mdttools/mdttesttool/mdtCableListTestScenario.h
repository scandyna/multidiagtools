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
#ifndef MDT_CABLE_LIST_TEST_SCENARIO_H
#define MDT_CABLE_LIST_TEST_SCENARIO_H

#include <QSqlDatabase>

class mdtCableListTestScenario
{
 public:

  mdtCableListTestScenario(QSqlDatabase db);

  /*
   * Will populate tables with scenario.
   *  See create* methods to know what data is inserted to DB
   */
  void createSenario();
  void removeScenario();

  /*
   * Create some vehicle types for tests:
   *  - Id_PK : 1 , Type : Vehicle type 1
   *  - Id_PK : 2 , Type : Vehicle type 2
   */
  void createTestVehicleTypes();
  /*
   * Remove test vehicle types
   */
  void removeTestVehicleTypes();

  /*
   * Create some connectors for tests:
   *  - Id_PK : 1
   *
   *  - Id_PK : 2
   *
   *  - Id_PK : 3
   *   -> Contact : Id_PK : 300 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : P
   *   -> Contact : Id_PK : 301 (, Connector_Id_FK : 3) , ConnectionType_Code_FK : S
   */
  void createTestConnectors();
  /*
   * Remove test connectors
   */
  void removeTestConnectors();

  /*
   * Create some articles for tests:
   *  - Id_PK : 1 , ArticleCode : 1234 , DesignationEN : Article 1
   *  - Id_PK : 2 , ArticleCode : 5678 , DesignationEN : Article 2
   * Will FAIL on problem
   */
  void createTestArticles();
  /*
   * Remove test articles
   */
  void removeTestArticles();

  /*
   * Create some article connections for tests:
   *  - Id_PK : 10 , Article_Id_FK : 1 , ConnectionType_Code_FK : T , ArticleContactName : 10 , Resistance : 0.1
   *  - Id_PK : 20 , Article_Id_FK : 2 , ConnectionType_Code_FK : T , ArticleContactName : 20 , Resistance : 0.2
   *  - Id_PK : 21 , Article_Id_FK : 2 , ConnectionType_Code_FK : T , ArticleContactName : 21 , Resistance : 0.21
   */
  void createTestArticleConnections();
  /*
   * Remove test article connections
   */
  void removeTestArticleConnections();

  /*
   * Create some article connectors for tests:
   *
   *  - Id_PK : 100 , Article_Id_FK : 1 , Connector_Id_FK : NULL , Name : Article connector 100
   *   -> Connection: Id_PK : 15 (, Article_Id_FK : 1 , ArticleConnector_Id_FK : 100) , ConnectionType_Code_FK : P , ArticleContactName : Article contact 15
   *
   *  - Id_PK : 200 , Article_Id_FK : 2 , Connector_Id_FK : 1 , Name : Article connector 200
   *   -> Connection: Id_PK : 25 (, Article_Id_FK : 2 , ArticleConnector_Id_FK : 200) , ConnectionType_Code_FK : S , ArticleContactName : Article contact 25
   */
  void createTestArticleConnectors();
  /*
   * Remove test article connectors
   */
  void removeTestArticleConnectors();

  /*
   * Create some article links for tests:
   *  - ArticleConnectionStart_Id_FK : 10 , ArticleConnectionEnd_Id_FK : 20 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *  - ArticleConnectionStart_Id_FK : 21 , ArticleConnectionEnd_Id_FK : 20 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   */
  void createTestArticleLinks();
  /*
   * Remove test article links
   */
  void removeTestArticleLinks();

  /*
   * Create some units for tests:
   *  - Id_PK : 1000 , Article_Id_FK : NULL , SchemaPosition : Position 1000
   *  - Id_PK : 2000 , Article_Id_FK : 2    , SchemaPosition : Position 2000
   *  - Id_PK : 2001 , Article_Id_FK : 2    , SchemaPosition : Position 2001
   */
  void createTestUnits();
  /*
   * Remove test units
   */
  void removeTestUnits();

  /*
   * Create some VehicleType-Unit assignations for tests:
   *  - Unit_Id_FK : 1000 , VehicleType_Id_FK : 1
   *  - Unit_Id_FK : 2000 , VehicleType_Id_FK : 2
   *  - Unit_Id_FK : 2001 , VehicleType_Id_FK : 2
   */
  void createTestVehicleTypeUnitAssignations();
  /*
   * Remove VehicleType-Unit assignations
   */
  void removeTestVehicleTypeUnitAssignations();

  /*
   * Create some unit connections for tests:
   *  - Id_PK : 10000 , Unit_Id_FK : 1000 , ArticleConnection_Id_FK : NULL , UnitContactName : Unit contact 10000
   *  - Id_PK : 10001 , Unit_Id_FK : 1000 , ArticleConnection_Id_FK : NULL , UnitContactName : Unit contact 10001
   *  - Id_PK : 20000 , Unit_Id_FK : 2000 , ArticleConnection_Id_FK : 20 , UnitContactName : Unit contact 20000
   *  - Id_PK : 20001 , Unit_Id_FK : 2000 , ArticleConnection_Id_FK : 21 , UnitContactName : Unit contact 20001
   *  - Id_PK : 20002 , Unit_Id_FK : 2001 , ArticleConnection_Id_FK : 20 , UnitContactName : Unit contact 20002
   *  - Id_PK : 20003 , Unit_Id_FK : 2001 , ArticleConnection_Id_FK : 21 , UnitContactName : Unit contact 20003
   *
   * Will also add a link from unit connection 20001 to 20000 and 20003 to 20002
   */
  void createTestUnitConnections();
  /*
   * Remove test unit connections
   */
  void removeTestUnitConnections();

  /*
   * Create some unit connectors for tests:
   *
   *  - Id_PK : 100000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 100000
   *   -> Connection: Id_PK 10005 , ConnectionType_Code_FK : T , UnitContactName : Unit contact 10005
   *
   *  - Id_PK : 200000 , Unit_Id_FK : 2000 , Connector_Id_FK : 1 , ArticleConnector_Id_FK : 200 , Name : Unit connector 200000
   *   -> Connection: Id_PK 20005 , ArticleConnection_Id_FK : 25 , ConnectionType_Code_FK : S , Name : Unit contact 20005
   *
   *  - Id_PK : 300000 , Unit_Id_FK : 1000 , Connector_Id_FK : 3 , ArticleConnector_Id_FK : NULL , Name : Unit connector 300000
   *   -> Connection: Id_PK 30005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : Unit contact 30005
   *   -> Connection: Id_PK 30006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : Unit contact 30006
   *
   *  - Id_PK : 400000 , Unit_Id_FK : 1000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 400000
   *   -> Connection: Id_PK 40005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : P , Name : A
   *   -> Connection: Id_PK 40006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   *
   *  - Id_PK : 500000 , Unit_Id_FK : 2000 , Connector_Id_FK : NULL , ArticleConnector_Id_FK : NULL , Name : Unit connector 500000
   *   -> Connection: Id_PK 50005 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : A
   *   -> Connection: Id_PK 50006 , ArticleConnection_Id_FK : NULL , ConnectionType_Code_FK : S , Name : B
   */
  void createTestUnitConnectors();
  /*
   * Remove test unit connectors
   */
  void removeTestUnitConnectors();

  /*
   * Create links for tests:
   *
   *  - UnitConnectionStart_Id_FK : 10000 , UnitConnectionEnd_Id_FK : 10001 , Identification : Link 10000<->10001 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   *
   *  - UnitConnectionStart_Id_FK : 10001 , UnitConnectionEnd_Id_FK : 20000 , Identification : Link 10001<->20000 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 2
   *
   *  - UnitConnectionStart_Id_FK : 30005 , UnitConnectionEnd_Id_FK : 40005 , Identification : Link 30005<->40005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   *
   *  - UnitConnectionStart_Id_FK : 40005 , UnitConnectionEnd_Id_FK : 50005 , Identification : Link 40005<->50005 , LinkType_Code_FK : CABLELINK , LinkDirection_Code_FK : BID
   *   -> VehicleTypeStart_Id_FK : 1 , VehicleTypeEnd_Id_FK : 1
   */
  void createTestLinks();
  /*
   * Remove test links
   */
  void removeTestLinks();

 private:

  QSqlDatabase pvDatabase;
};

#endif //#ifndef MDT_CABLE_LIST_TEST_SCENARIO_H
