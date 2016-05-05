/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClAutoConnectionTest.h"
#include "mdtCableListTestScenario.h"
#include "mdtClAutoConnection.h"
#include "mdtClConnectableCriteria.h"
#include "mdtClUnitConnectionKeyData.h"
#include "Mdt/CableList/UnitConnectionPkList.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClUnitConnection.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtSqlForeignKeySetting.h"
#include "mdtCableListTestScenario.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QFile>
#include <QVariant>
#include <QList>
#include <QLocale>

#include <QDebug>

void mdtClAutoConnectionTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabase.isOpen());
}

void mdtClAutoConnectionTest::cleanupTestCase()
{
}

void mdtClAutoConnectionTest::canConnectConnectionsTest()
{
  mdtClAutoConnection ac(pvDatabase);
  mdtClConnectableCriteria criteria;
  mdtClUnitConnectionData cnxA, cnxB;

  /*
   * In current version, only checkContactType has sense here
   */
  criteria.checkContactType = true;
  /*
   * Setup connections:
   *  A is a socket
   *  B is a socket
   *  -> Must return false
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Socket);
  cnxB.setConnectionType(mdtClConnectionType_t::Socket);
  QVERIFY(!ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a socket
   *  B is a pin
   *  -> Must return true
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Socket);
  cnxB.setConnectionType(mdtClConnectionType_t::Pin);
  QVERIFY(ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a pin
   *  B is a socket
   *  -> Must return true
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Pin);
  cnxB.setConnectionType(mdtClConnectionType_t::Socket);
  QVERIFY(ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a terminal
   *  B is a terminal
   *  -> Must return true
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
  cnxB.setConnectionType(mdtClConnectionType_t::Terminal);
  QVERIFY(ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a terminal
   *  B is a socket
   *  -> Must return false
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
  cnxB.setConnectionType(mdtClConnectionType_t::Socket);
  QVERIFY(!ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a terminal
   *  B is a pin
   *  -> Must return false
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
  cnxB.setConnectionType(mdtClConnectionType_t::Pin);
  QVERIFY(!ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a socket
   *  B is a terminal
   *  -> Must return false
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Socket);
  cnxB.setConnectionType(mdtClConnectionType_t::Terminal);
  QVERIFY(!ac.canConnectConnections(cnxA, cnxB, criteria));
  /*
   * Setup connections:
   *  A is a terminal
   *  B is a pin
   *  -> Must return false
   */
  cnxA.setConnectionType(mdtClConnectionType_t::Terminal);
  cnxB.setConnectionType(mdtClConnectionType_t::Pin);
  QVERIFY(!ac.canConnectConnections(cnxA, cnxB, criteria));
}

void mdtClAutoConnectionTest::checkOrBuildConnectionLinkListByNameTest()
{
  using Mdt::CableList::LinkVersionPk;
  using Mdt::CableList::ModificationPk;
  using Mdt::CableList::ModificationType;

  mdtClAutoConnection ac(pvDatabase);
  mdtClConnectableCriteria criteria;
  QStringList cnxNames;
  QList<QVariant> idList;
  QList<mdtClUnitConnectionData> ucnxAlist, ucnxBlist;
  LinkVersionPk versionPk;
  ModificationPk modificationPk;
  QList<mdtClLinkData> linkDataList;

  /*
   * Setup criteria
   */
  criteria.checkContactType = true;
  /*
   * Setup 2 sets of unit connections:
   *  - ucnxAlist : A, B, C, D, E , all pins
   *  - ucnxBlist : A, B, C, D, E , all sockets
   * This are all connectable
   */
  // Build connection lists
  cnxNames.clear();
  cnxNames << "A" << "B" << "C" << "D" << "E";
  idList.clear();
  idList << 1 << 2 << 3 << 4 << 5;
  ucnxAlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Pin);
  idList.clear();
  idList << 6 << 7 << 8 << 9 << 10;
  ucnxBlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Socket);
  // Check - no link list to build
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria));
  // Check - Buil link list
  versionPk.setVersion(500);
  modificationPk.setType(ModificationType::New);
//   versionPk.versionPk = 500;
//   modificationPk.setModification(mdtClModification_t::New);
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria, &linkDataList, &versionPk, &modificationPk));
  QCOMPARE(linkDataList.size(), 5);
  QVERIFY(!linkDataList.at(0).pk().isNull());
  QVERIFY(linkDataList.at(0).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(0).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(1).pk().isNull());
  QVERIFY(linkDataList.at(1).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(1).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(2).pk().isNull());
  QVERIFY(linkDataList.at(2).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(2).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(3).pk().isNull());
  QVERIFY(linkDataList.at(3).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(3).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(4).pk().isNull());
  QVERIFY(linkDataList.at(4).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(4).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  /*
   * Setup 2 sets of unit connections:
   *  - ucnxAlist : A, B, C, all pins
   *  - ucnxBlist : A, B, C, D, E , all sockets
   * This are all connectable
   */
  // Build connection list A
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  idList.clear();
  idList << 1 << 2 << 3;
  ucnxAlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Pin);
  // Build connection list B
  idList.clear();
  idList << 6 << 7 << 8 << 9 << 10;
  cnxNames.clear();
  cnxNames << "A" << "B" << "C" << "D" << "E";
  ucnxBlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Socket);
  // Check - no link list to build
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria));
  // Check - Buil link list
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria, &linkDataList, &versionPk, &modificationPk));
  QCOMPARE(linkDataList.size(), 3);
  QVERIFY(!linkDataList.at(0).pk().isNull());
  QVERIFY(linkDataList.at(0).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(0).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(1).pk().isNull());
  QVERIFY(linkDataList.at(1).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(1).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(2).pk().isNull());
  QVERIFY(linkDataList.at(2).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(2).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  /*
   * Setup 2 sets of unit connections:
   *  - ucnxAlist : A, B, C, D, E, all pins
   *  - ucnxBlist : A, B, C, all sockets
   * This are all connectable
   */
  // Build connection list A
  cnxNames.clear();
  cnxNames << "A" << "B" << "C" << "D" << "E";
  idList.clear();
  idList << 1 << 2 << 3 << 4  << 5;
  ucnxAlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Pin);
  // Build connection list B
  idList.clear();
  idList << 6 << 7 << 8;
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxBlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Socket);
  // Check - no link list to build
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria));
  // Check - Buil link list
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria, &linkDataList, &versionPk, &modificationPk));
  QCOMPARE(linkDataList.size(), 3);
  QVERIFY(!linkDataList.at(0).pk().isNull());
  QVERIFY(linkDataList.at(0).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(0).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(1).pk().isNull());
  QVERIFY(linkDataList.at(1).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(1).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(2).pk().isNull());
  QVERIFY(linkDataList.at(2).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(2).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  /*
   * Setup 2 sets of unit connections:
   *  - ucnxAlist : C, B, D, A, E, all pins
   *  - ucnxBlist : A, B, C, all sockets
   * This are all connectable
   */
  // Build connection list A
  cnxNames.clear();
  cnxNames << "C" << "B" << "D" << "A" << "E";
  idList.clear();
  idList << 1 << 2 << 3 << 4  << 5;
  ucnxAlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Pin);
  // Build connection list B
  idList.clear();
  idList << 6 << 7 << 8;
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxBlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Socket);
  // Check - no link list to build
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria));
  // Check - Buil link list
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria, &linkDataList, &versionPk, &modificationPk));
  QCOMPARE(linkDataList.size(), 3);
  QVERIFY(!linkDataList.at(0).pk().isNull());
  QVERIFY(linkDataList.at(0).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(0).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(1).pk().isNull());
  QVERIFY(linkDataList.at(1).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(1).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!linkDataList.at(2).pk().isNull());
  QVERIFY(linkDataList.at(2).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(2).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
  /*
   * Setup 2 sets of unit connections:
   *  - ucnxAlist : C, B, D, A, E, all pins
   *  - ucnxBlist : A, Y, Z, all sockets
   * This are all connectable
   */
  // Build connection list A
  cnxNames.clear();
  cnxNames << "C" << "B" << "D" << "A" << "E";
  idList.clear();
  idList << 1 << 2 << 3 << 4  << 5;
  ucnxAlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Pin);
  // Build connection list B
  idList.clear();
  idList << 6 << 7 << 8;
  cnxNames.clear();
  cnxNames << "A" << "Y" << "Z";
  ucnxBlist = buildUnitConnectionList(idList, cnxNames, mdtClConnectionType_t::Socket);
  // Check - no link list to build
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria));
  // Check - Buil link list
  QVERIFY(ac.checkOrBuildConnectionLinkListByName(ucnxAlist, ucnxBlist, criteria, &linkDataList, &versionPk, &modificationPk));
  QCOMPARE(linkDataList.size(), 1);
  QVERIFY(!linkDataList.at(0).pk().isNull());
  QVERIFY(linkDataList.at(0).keyData().linkTypeFk().type() == mdtClLinkType_t::Connection);
  QVERIFY(linkDataList.at(0).keyData().linkDirectionFk().direction() == mdtClLinkDirection_t::Bidirectional);
}

void mdtClAutoConnectionTest::getConnectableConnectionsTest()
{
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::UnitConnectionPkList;

  mdtClAutoConnection ac(pvDatabase);
  mdtClUnitConnection ucnx(pvDatabase);
  mdtCableListTestScenario scenario(pvDatabase);
  mdtClConnectableCriteria criteria;
  QStringList cnxNames;
  QList<mdtClUnitConnectionData> ucnxList;
  UnitConnectionPk pkA, pkB, pkC;
  mdtClUnitConnectionKeyData key;
  mdtClUnitConnectionData ucnxData;
  UnitConnectionPkList pkList;
  bool ok;

  /*
   * Build scenario
   */
  scenario.createTestArticles();
  scenario.createTestUnits();
  /*
   * Build a list of unit connections - Pins
   */
  // Add to unit 1000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(1000, cnxNames, mdtClConnectionType_t::Pin);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  // Add to unit 2000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(2000, cnxNames, mdtClConnectionType_t::Pin);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  /*
   * Build a list of unit connections - Sockets
   */
  // Add to unit 1000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(1000, cnxNames, mdtClConnectionType_t::Socket);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  // Add to unit 2000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(2000, cnxNames, mdtClConnectionType_t::Socket);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  /*
   * Build a list of unit connections - Terminals
   */
  // Add to unit 1000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(1000, cnxNames, mdtClConnectionType_t::Terminal);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  // Add to unit 2000
  cnxNames.clear();
  cnxNames << "A" << "B" << "C";
  ucnxList = buildUnitConnectionList(1000, cnxNames, mdtClConnectionType_t::Terminal);
  QVERIFY(ucnx.addUnitConnectionList(ucnxList, true));
  /*
   * Build a list of references
   */
  // A - Pin
  key.clear();
  key.setUnitId(1000);
  key.setConnectionType(mdtClConnectionType_t::Pin);
  ucnxData.clear();
  ucnxData.setKeyData(key);
  ucnxData.name = "A";
  pkA = ucnx.addUnitConnection(ucnxData, true);
  QVERIFY(!pkA.isNull());
  // B - Socket
  key.clear();
  key.setUnitId(1000);
  key.setConnectionType(mdtClConnectionType_t::Socket);
  ucnxData.clear();
  ucnxData.setKeyData(key);
  ucnxData.name = "B";
  pkB = ucnx.addUnitConnection(ucnxData, true);
  QVERIFY(!pkB.isNull());
  // C - Terminal
  key.clear();
  key.setUnitId(1000);
  key.setConnectionType(mdtClConnectionType_t::Terminal);
  ucnxData.clear();
  ucnxData.setKeyData(key);
  ucnxData.name = "C";
  pkC = ucnx.addUnitConnection(ucnxData, true);
  QVERIFY(!pkC.isNull());
  /*
   * Setup criteria
   */
  criteria.checkContactName = true;
  criteria.checkContactType = true;
  /*
   * Check getting connectable connection - Not filtered on unit
   */
  // Chefor pkA (Pin) - Must return 2 items
  pkList = ac.getConnectableConnectionPkList(pkA, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 2);
  // Chefor pkB (Socket) - Must return 2 items
  pkList = ac.getConnectableConnectionPkList(pkB, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 2);
  // Chefor pkC (Terminal) - Must return 2 items
  pkList = ac.getConnectableConnectionPkList(pkA, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 2);
  /*
   * Check getting connectable connection - Filtered on unit 1000
   */
  // Chefor pkA (Pin) - Must return 1 item
  pkList = ac.getConnectableConnectionPkList(pkA, 1000, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 1);
  // Chefor pkB (Socket) - Must return 1 item
  pkList = ac.getConnectableConnectionPkList(pkB, 1000, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 1);
  // Chefor pkC (Terminal) - Must return 1 item
  pkList = ac.getConnectableConnectionPkList(pkA, 1000, criteria, ok);
  QVERIFY(ok);
  QCOMPARE(pkList.size(), 1);
  /*
   * Remove created unit connections
   */
  QVERIFY(ucnx.removeData("UnitConnection_tbl", "Unit_Id_FK", 1000));
  QVERIFY(ucnx.removeData("UnitConnection_tbl", "Unit_Id_FK", 2000));
}

void mdtClAutoConnectionTest::canConnectConnectorsTest()
{
  mdtClAutoConnection ac(pvDatabase);
  mdtClConnectableCriteria criteria;
  mdtClConnectorData a, b;

  /*
   * Setup 2 compatible connectors
   */
  // Setup a
  a.clear();
  a.gender = "male";
  a.form = "round";
  // Setup b
  b.clear();
  b.gender = "female";
  b.form = "round";
  /*
   * Setup criteria
   */
  criteria.checkGenderAreOpposite = true;
  criteria.checkForm = true;
  /*
   * Check
   */
  QVERIFY(ac.canConnectConnectors(a, b, criteria));
  
  /// \todo finish..
}

void mdtClAutoConnectionTest::canConnectUnitConnectorsTest()
{
  mdtClAutoConnection ac(pvDatabase);
  mdtClUnitConnection ucnx(pvDatabase);
  mdtCableListTestScenario scenario(pvDatabase);
  mdtClConnectorData cnrData;
  QList<mdtClConnectorContactData> contactList;
  mdtClConnectorPkData cnrPkA, cnrPkB;
  QStringList contactNames;
  mdtClUnitConnectorData ucnrA, ucnrB;
  mdtClConnectableCriteria criteria;
  bool ok;

  /*
   * Build scenario
   */
  scenario.createTestArticles();
  scenario.createTestUnits();
  /*
   * Build connectors:
   *  - A:
   *   -> Gender: female, Form : round, contacts : A, B, C, D (all sockets)
   *  - B:
   *   -> Gender: male, Form : round, contacts : A, B, C, D (all pins)
   */
  // Build connector A
  cnrData.clear();
  cnrData.gender = "female";
  cnrData.form = "round";
  contactNames.clear();
  contactNames << "A" << "B" << "C" << "D";
  contactList = buildConnectorContactList(contactNames, mdtClConnectionType_t::Socket);
  cnrData.setContactDataList(contactList);
  cnrPkA = ucnx.addConnector(cnrData, true);
  QVERIFY(!cnrPkA.isNull());
  // Build connector B
  cnrData.clear();
  cnrData.gender = "male";
  cnrData.form = "round";
  contactNames.clear();
  contactNames << "A" << "B" << "C" << "D";
  contactList = buildConnectorContactList(contactNames, mdtClConnectionType_t::Pin);
  cnrData.setContactDataList(contactList);
  cnrPkB = ucnx.addConnector(cnrData, true);
  QVERIFY(!cnrPkB.isNull());
  /*
   * Build unit connectors based on freshly created connectors
   */
  ucnrA = buildUnitConnector(cnrPkA, 1000);
  QVERIFY(ucnrA.isBasedOnConnector());
  QVERIFY(!ucnrA.connectionDataList().isEmpty());
  ucnrB = buildUnitConnector(cnrPkB, 1000);
  QVERIFY(ucnrB.isBasedOnConnector());
  QVERIFY(!ucnrB.connectionDataList().isEmpty());
  /*
   * Setup criteria
   */
  criteria.checkGenderAreOpposite = true;
  criteria.checkForm = true;
  criteria.checkInsert = false;   /// \todo Turn ON once implemented
  criteria.checkInsertRotation = false; /// \todo Turn ON once implemented
  /*
   * Check
   */
  QVERIFY(ac.canConnectConnectors(ucnrA, ucnrB, criteria, ok));
  QVERIFY(ok);
  /*
   * Remove created connectors
   */
  QVERIFY(ucnx.removeConnector(cnrPkA, true));
  QVERIFY(ucnx.removeConnector(cnrPkB, true));
}

void mdtClAutoConnectionTest::getConnectableConnectorsList()
{
  QFAIL("Not implemented yet");
}

void mdtClAutoConnectionTest::connectByContactNameTest()
{
  QFAIL("Not implemented yet");
}

/*
 * Test database helper methods
 */

QList<mdtClUnitConnectionData> mdtClAutoConnectionTest::buildUnitConnectionList(const QList<QVariant> & idList, const QStringList & contactNames, mdtClConnectionType_t t)
{
  using Mdt::CableList::UnitConnectionPk;

  Q_ASSERT(idList.size() == contactNames.size());

  QList<mdtClUnitConnectionData> ucnxList;

  for(int i = 0; i < idList.size(); ++i){
    mdtClUnitConnectionKeyData key;
    key.setPk(UnitConnectionPk( idList.at(i).toLongLong() ));
    mdtClUnitConnectionData data;
    data.setKeyData(key);
    data.setConnectionType(t);
    data.name = contactNames.at(i);
    ucnxList << data;
  }

  return ucnxList;
}

QList<mdtClUnitConnectionData> mdtClAutoConnectionTest::buildUnitConnectionList(const QVariant &unitId, const QStringList &contactNames, mdtClConnectionType_t t)
{
  QList<mdtClUnitConnectionData> ucnxList;

  for(const auto & name : contactNames){
    mdtClUnitConnectionKeyData key;
    key.setUnitId(unitId);
    key.setConnectionType(t);
    mdtClUnitConnectionData data;
    data.setKeyData(key);
    data.name = name;
    ucnxList << data;
  }

  return ucnxList;
}

QList<mdtClConnectorContactData> mdtClAutoConnectionTest::buildConnectorContactList(const QStringList &contactNames, mdtClConnectionType_t t)
{
  QList<mdtClConnectorContactData> cnxList;

  for(const auto & name : contactNames){
    mdtClConnectorContactKeyData key;
    key.setConnectionType(t);
    mdtClConnectorContactData data;
    data.setKeyData(key);
    data.name = name;
    cnxList << data;
  }

  return cnxList;
}

mdtClUnitConnectorData mdtClAutoConnectionTest::buildUnitConnector(const mdtClConnectorPkData &cnrPk, const QVariant &unitId)
{
  mdtClUnitConnectorData data;
  mdtClUnitConnectorKeyData key;
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClConnectorData cnrData;
  bool ok;

  cnrData = ucnx.getConnectorData(cnrPk, true, ok);
  if(!ok){
    return data;
  }
  if(cnrData.isNull()){
    return data;
  }
  key.setUnitId(unitId);
  key.setConnectorFk(cnrData.pk());
  data.setKeyData(key);
  ucnx.addConnectionsToUnitConnector(data, cnrData.contactDataList());

  return data;
}

void mdtClAutoConnectionTest::createDatabaseSchema()
{
  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
  /*
   * Check Sqlite database creation
   */
  mdtTtDatabaseSchema schema;
  QVERIFY(schema.buildSchema());
  QVERIFY(schema.databaseSchema().createSchema(pvDatabase));
  QVERIFY(schema.checkSchema(pvDatabase));
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtClAutoConnectionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
