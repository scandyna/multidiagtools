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
#include "mdtTtTestNodeTestData.h"
#include "mdtTest.h"
#include "mdtTtBase.h"
#include <QList>
#include <QSqlRecord>
#include <QDebug>

void mdtTtTestNodeATestData::clear()
{
  removeLinks();
  removeRelays();
  removeTestNodeData();
}

bool mdtTtTestNodeATestData::populate()
{
  if(!createTestNodeData()){
    return false;
  }
  if(!createRelays()){
    return false;
  }
  if(!createMeasureAndTestConnections()){
    return false;
  }
  if(!createsLinks()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeATestData::createTestNodeData()
{
  QSqlQuery query(pvDatabase);

  if(!beginTransaction()){
    return false;
  }
  if(!query.exec("INSERT INTO VehicleType_tbl (Type, SubType, SeriesNumber) VALUES ('Test system', 'Test node A', '1')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  pvTestNodeId = query.lastInsertId();
  Q_ASSERT(!pvTestNodeId.isNull());
  if(!query.exec("INSERT INTO TestNode_tbl (VehicleType_Id_FK_PK, Alias) VALUES (" + pvTestNodeId.toString() + ", 'A')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtTestNodeATestData::removeTestNodeData()
{
  QSqlQuery query(pvDatabase);

  QVERIFY(beginTransaction());
  QVERIFY(!pvTestNodeId.isNull());
  QVERIFY(query.exec("DELETE FROM TestNode_tbl WHERE VehicleType_Id_FK_PK = " + pvTestNodeId.toString()));
  QVERIFY(query.exec("DELETE FROM VehicleType_tbl WHERE Id_PK = " + pvTestNodeId.toString()));
  QVERIFY(commitTransaction());
}

bool mdtTtTestNodeATestData::createRelays()
{
  Q_ASSERT(!pvTestNodeId.isNull());

  QVariant unitId;
  QVariant connectionId;

  if(!beginTransaction()){
    return false;
  }
  /*
   * Add K1 coupling relay
   */
  // Create unit part
  unitId = addUnit("K1");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "BUSCPLREL", 0)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K2 coupling relay
   */
  // Create unit part
  unitId = addUnit("K2");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "BUSCPLREL", 1)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K3 coupling relay
   */
  // Create unit part
  unitId = addUnit("K3");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "BUSCPLREL", 2)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K4 coupling relay
   */
  // Create unit part
  unitId = addUnit("K4");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "BUSCPLREL", 3)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K30 channel relay
   */
  // Create unit part
  unitId = addUnit("K30");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 29)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K31 channel relay
   */
  // Create unit part
  unitId = addUnit("K31");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 30)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K32 channel relay
   */
  // Create unit part
  unitId = addUnit("K32");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 31)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K33 channel relay
   */
  // Create unit part
  unitId = addUnit("K33");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 32)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K34 channel relay
   */
  // Create unit part
  unitId = addUnit("K34");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 33)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K40 channel relay
   */
  // Create unit part
  unitId = addUnit("K40");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 39)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add K41 channel relay
   */
  // Create unit part
  unitId = addUnit("K41");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "CHANELREL", 40)){
    rollbackTransaction();
    return false;
  }
  // Add connection 13
  connectionId = addUnitConnection(unitId, "13", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection 14
  connectionId = addUnitConnection(unitId, "14", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestNodeATestData::createMeasureAndTestConnections()
{
  Q_ASSERT(!pvTestNodeId.isNull());

  QVariant unitId;
  QVariant connectionId;

  if(!beginTransaction()){
    return false;
  }
  /*
   * Add XMEAS
   */
  // Create unit part
  unitId = addUnit("XMEAS");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "MEASCONNECTOR")){
    rollbackTransaction();
    return false;
  }
  // Add connection +
  connectionId = addUnitConnection(unitId, "+", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection -
  connectionId = addUnitConnection(unitId, "-", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  /*
   * Add XTEST
   */
  // Create unit part
  unitId = addUnit("XTEST");
  if(unitId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnit(unitId, pvTestNodeId, "TESTCONNECTOR")){
    rollbackTransaction();
    return false;
  }
  // Add connection A1
  connectionId = addUnitConnection(unitId, "A1", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection A2
  connectionId = addUnitConnection(unitId, "A2", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection A3
  connectionId = addUnitConnection(unitId, "A3", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }
  // Add connection A4
  connectionId = addUnitConnection(unitId, "A4", 0.0);
  if(connectionId.isNull()){
    rollbackTransaction();
    return false;
  }
  if(!addTestNodeUnitConnection(unitId, connectionId)){
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

QVariant mdtTtTestNodeATestData::addUnit(const QString& schemaPos)
{
  QSqlQuery query(pvDatabase);
  QVariant id;

  if(!query.exec("INSERT INTO Unit_tbl (SchemaPosition) VALUES ('" + schemaPos + "')")){
    debugSqlError(query);
    return QVariant();
  }
  id = query.lastInsertId();
  if(!query.exec("INSERT INTO VehicleType_Unit_tbl (Unit_Id_FK,VehicleType_Id_FK) VALUES (" + id.toString() + "," + pvTestNodeId.toString() + ")")){
    debugSqlError(query);
    return QVariant();
  }

  return id;
}

bool mdtTtTestNodeATestData::addTestNodeUnit(const QVariant& unitId, const QVariant& testNodeId, const QString & type, const QVariant& ioPos)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO TestNodeUnit_tbl (Unit_Id_FK_PK, TestNode_Id_FK, Type_Code_FK, IoPosition)";
  sql += " VALUES (" + unitId.toString() + "," + testNodeId.toString() + ",'" + type + "'," + ioPos.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

bool mdtTtTestNodeATestData::addTestNodeUnit(const QVariant& unitId, const QVariant& testNodeId, const QString& type)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO TestNodeUnit_tbl (Unit_Id_FK_PK, TestNode_Id_FK, Type_Code_FK)";
  sql += " VALUES (" + unitId.toString() + "," + testNodeId.toString() + ",'" + type + "')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

QVariant mdtTtTestNodeATestData::addUnitConnection(const QVariant& unitId, const QString& contactName, const QVariant& resistance)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO UnitConnection_tbl (Unit_Id_FK, UnitContactName, Resistance)";
  sql += " VALUES (" + unitId.toString() + ",'" + contactName + "'," + resistance.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return QVariant();
  }

  return query.lastInsertId();
}

bool mdtTtTestNodeATestData::addTestNodeUnitConnection(const QVariant& unitId, const QVariant& unitConnectionId)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO TestNodeUnitConnection_tbl (UnitConnection_Id_FK_PK, TestNodeUnit_Id_FK)";
  sql += " VALUES (" + unitConnectionId.toString() + "," + unitId.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

void mdtTtTestNodeATestData::removeRelays()
{
  mdtTtBase tb(0, pvDatabase);
  QList<QVariant> unitIdList;
  QString sql;
  bool ok;

  sql = "SELECT Unit_Id_FK FROM VehicleType_Unit_tbl WHERE VehicleType_Id_FK = " + pvTestNodeId.toString();
  unitIdList = tb.getDataList<QVariant>(sql, ok);
  QVERIFY(ok);
  for(auto & id : unitIdList){
    QVERIFY(tb.removeData("TestNodeUnitConnection_tbl", "TestNodeUnit_Id_FK", id));
    QVERIFY(tb.removeData("TestNodeUnit_tbl", "Unit_Id_FK_PK", id));
    QVERIFY(tb.removeData("UnitConnection_tbl", "Unit_Id_FK", id));
    QVERIFY(tb.removeData("VehicleType_Unit_tbl", "Unit_Id_FK", id, "VehicleType_Id_FK", pvTestNodeId));
    QVERIFY(tb.removeData("Unit_tbl", "Id_PK", id));
  }
}

bool mdtTtTestNodeATestData::createsLinks()
{
  if(!beginTransaction()){
    return false;
  }
  /*
   * Add XMEAS - coupling relays links
   */
  if(!addLink("W001", 0.0, "XMEAS", "+", "K1", "13")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W001", 0.0, "XMEAS", "+", "K2", "13")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W002", 0.0, "XMEAS", "-", "K3", "13")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W002", 0.0, "XMEAS", "-", "K4", "13")){
    rollbackTransaction();
    return false;
  }
  /*
   * Add coupling - channel relays links
   */
  if(!addLink("W010", 0.0, "K1", "14", "K3", "14")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W010", 0.0, "K3", "14", "K30", "13")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W010", 0.0, "K30", "13", "K31", "13")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W011", 0.0, "K2", "14", "K4", "14")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W011", 0.0, "K4", "14", "K40", "14")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W011", 0.0, "K40", "14", "K41", "14")){
    rollbackTransaction();
    return false;
  }
  /*
   * Add channel relays - test connector links
   */
  if(!addLink("W030", 0.0, "K30", "14", "XTEST", "A1")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W031", 0.0, "K31", "14", "XTEST", "A3")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W040", 0.0, "K40", "13", "XTEST", "A2")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W041", 0.0, "K41", "13", "XTEST", "A4")){
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestNodeATestData::addLink(const QString & identification, const QVariant & R, const QString& startPos, const QString& startConnection, const QString& endPos, const QString& endConnection)
{
  mdtTtBase tb(0, pvDatabase);
  QSqlQuery query(pvDatabase);
  QString sql;
  bool ok;
  QList<QVariant> dataList;
  QVariant startUnitId, endUnitId;
  QVariant startConnectionId, endConnectionId;

  // Get start unit ID
  sql = "SELECT Id_PK FROM Unit_tbl WHERE SchemaPosition = '" + startPos + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  startUnitId = dataList.at(0);
  // Get end unit ID
  sql = "SELECT Id_PK FROM Unit_tbl WHERE SchemaPosition = '" + endPos + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  endUnitId = dataList.at(0);
  // Get start connection ID
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + startUnitId.toString() + " AND UnitContactName = '" + startConnection + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  startConnectionId = dataList.at(0);
  // Get start connection ID
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + endUnitId.toString() + " AND UnitContactName = '" + endConnection + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  endConnectionId = dataList.at(0);
  // Add to Link_tbl
  sql = "INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, LinkType_Code_FK, LinkDirection_Code_FK, Identification, Resistance)";
  sql += " VALUES (" + startConnectionId.toString() + "," + endConnectionId.toString() + ",'CABLELINK','BID'";
  sql += ",'" + identification + "'," + R.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  // Add to VehicleType_Link_tbl
  sql = "INSERT INTO VehicleType_Link_tbl (VehicleTypeStart_Id_FK,VehicleTypeEnd_Id_FK,UnitConnectionStart_Id_FK,UnitConnectionEnd_Id_FK)";
  sql += " VALUES (" + pvTestNodeId.toString() + "," + pvTestNodeId.toString() + "," + startConnectionId.toString() + "," + endConnectionId.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

void mdtTtTestNodeATestData::removeLinks()
{
  mdtTtBase tb(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  sql = "SELECT * FROM VehicleType_Link_tbl WHERE VehicleTypeStart_Id_FK = " + pvTestNodeId.toString() + " AND VehicleTypeEnd_Id_FK = " + pvTestNodeId.toString();
  dataList = tb.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  for(const auto & rec : dataList){
    removeLink(rec.value("UnitConnectionStart_Id_FK"), rec.value("UnitConnectionEnd_Id_FK"));
  }
}

void mdtTtTestNodeATestData::removeLink(const QVariant& startConnectionId, const QVariant& endConnectionId)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  // Remove from VehicleType_Link_tbl
  sql = "DELETE FROM VehicleType_Link_tbl WHERE UnitConnectionStart_Id_FK = " + startConnectionId.toString() + " AND UnitConnectionEnd_Id_FK = " + endConnectionId.toString();
  sql += " AND VehicleTypeStart_Id_FK = " + pvTestNodeId.toString() + " AND VehicleTypeEnd_Id_FK = " + pvTestNodeId.toString();
  QVERIFY(query.exec(sql));
  // Remove from Link_tbl
  sql = "DELETE FROM Link_tbl WHERE UnitConnectionStart_Id_FK = " + startConnectionId.toString() + " AND UnitConnectionEnd_Id_FK = " + endConnectionId.toString();
  QVERIFY(query.exec(sql));
}

bool mdtTtTestNodeATestData::beginTransaction()
{
  if(!pvDatabase.transaction()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtTestNodeATestData::commitTransaction()
{
  if(!pvDatabase.commit()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtTestNodeATestData::rollbackTransaction()
{
  if(!pvDatabase.rollback()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

void mdtTtTestNodeATestData::debugSqlError(const QSqlError& error)
{
    qDebug() << error;
}

void mdtTtTestNodeATestData::debugSqlError(const QSqlQuery& query)
{
  qDebug() << "Error on query '" << query.lastQuery() << "': " << query.lastError();
}
