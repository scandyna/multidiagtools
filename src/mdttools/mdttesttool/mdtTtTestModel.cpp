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
#include "mdtTtTestModel.h"
#include "mdtTtTestModelItem.h"
#include "mdtTtTestNode.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtSqlRecord.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtTtTestModel::mdtTtTestModel(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

QString mdtTtTestModel::sqlForTestNodeSelection(const QVariant & testModelId) const
{
  QString sql;

  sql = "SELECT\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";
  sql += "WHERE VT.Id_PK NOT IN (";
  sql += " SELECT TestNode_Id_FK FROM TestModel_TestNode_tbl WHERE TestModel_Id_FK = " + testModelId.toString();
  sql += ")";

  return sql;
}

mdtTtTestModelData mdtTtTestModel::getTestModelData(const QVariant & testModelId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QList<QSqlRecord> dataList;

  sql = "SELECT * FROM TestModel_tbl WHERE Id_PK = " + testModelId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return mdtTtTestModelData();
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Could not find data in 'TestModel_tbl' for Id_PK = '") + testModelId.toString() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    *ok = false;
    return mdtTtTestModelData();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

QList<QVariant> mdtTtTestModel::getListOfTestItemIdListByTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> testItemIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT Id_PK FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of test items", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return testItemIdList;
  }
  while(query.next()){
    testItemIdList.append(query.value(0));
  }

  return testItemIdList;
}

QList<QVariant> mdtTtTestModel::getListOfUsedNodeIdListByTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant nodeId;
  QList<QVariant> nodeIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT TestNode_Id_FK FROM TestModelItemNode_view WHERE TestModel_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of used nodes", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return nodeIdList;
  }
  while(query.next()){
    nodeId = query.value(0);
    if((!nodeId.isNull())&&(!nodeIdList.contains(nodeId))){
      nodeIdList.append(nodeId);
    }
  }

  return nodeIdList;
}

QList<QVariant> mdtTtTestModel::getListOfUnusedNodeUnitIdListByTestId(const QVariant & testId, const QVariant & type)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> nodeIdList;
  QVariant nodeUnitId;
  QList<QVariant> nodeUnitIdList;
  int i;

  // Get list of all nodes used by given test
  nodeIdList = getListOfUsedNodeIdListByTestId(testId);
  if(nodeIdList.isEmpty()){
    return nodeUnitIdList;
  }
  // Get list of all node units that are part of a node, and that is not used by given test
  Q_ASSERT(nodeIdList.size() > 0);
  sql = "SELECT Unit_Id_FK_PK FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + nodeIdList.at(0).toString();
  for(i = 1; i < nodeIdList.size(); ++i){
    sql += " OR TestNode_Id_FK = " + nodeIdList.at(i).toString();
  }
  if(!type.isNull()){
    sql += " AND Type_Code_FK = '" + type.toString() + "' ";
  }
  sql += " AND Unit_Id_FK_PK NOT IN (";
  sql += " SELECT Unit_Id_FK_PK FROM TestModelItemNodeUnit_view WHERE TestModel_Id_FK = " + testId.toString();
  sql += ")";
  ///qDebug() << "SQL: " << sql;
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of unused node units", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return nodeUnitIdList;
  }
  while(query.next()){
    nodeUnitId = query.value(0);
    if((!nodeUnitId.isNull())&&(!nodeUnitIdList.contains(nodeUnitId))){
      nodeUnitIdList.append(nodeUnitId);
    }
  }

  return nodeUnitIdList;
}

QVariant mdtTtTestModel::getNextSequenceNumber(const QVariant & testModelId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant lastNumber;

  sql = "SELECT SequenceNumber FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testModelId.toString() + " ORDER BY SequenceNumber ASC";
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of seuqnec number", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return QVariant();
  }
  if(!query.last()){
    return 1;
  }
  lastNumber = query.value(0);
  if(lastNumber.isNull()){
    return 1;
  }

  return lastNumber.toInt() + 1;
}

QList<QVariant> mdtTtTestModel::getTestNodeUnitSetupIdList(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> tnusIdList;
  QVariant id;

  // Setup and run query to get data in unit link view
  sql = "SELECT Id_PK FROM TestModelItemNodeUnitSetup_view WHERE TestModel_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of node unit setup", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return tnusIdList;
  }
  while(query.next()){
    id = query.value(0);
    if((!id.isNull())&&(!tnusIdList.contains(id))){
      tnusIdList.append(id);
    }
  }

  return tnusIdList;
}

bool mdtTtTestModel::addTestNode(const QVariant & testNodeId, const QVariant & testModelId)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestModel_TestNode_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("TestNode_Id_FK", testNodeId);
  record.setValue("TestModel_Id_FK", testModelId);

  return addRecord(record, "TestModel_TestNode_tbl");
}

bool mdtTtTestModel::removeTestNodes(const mdtSqlTableSelection & s)
{
  return removeData("TestModel_TestNode_tbl", s, true);
}

bool mdtTtTestModel::addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant sequenceNumber;

  if(!beginTransaction()){
    return false;
  }
  // Prepare query for edition
  sql = "INSERT INTO TestModelItem_tbl (TestModel_Id_FK, TestLinkBusA_Id_FK, TestLinkBusB_Id_FK, SequenceNumber, ExpectedValue) "\
        "VALUES (:TestModel_Id_FK, :TestLinkBusA_Id_FK, :TestLinkBusB_Id_FK, :SequenceNumber, :ExpectedValue)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Get sequence number
  sequenceNumber = getNextSequenceNumber(testId);
  if(sequenceNumber.isNull()){
    rollbackTransaction();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestModel_Id_FK", testId);
  query.bindValue(":TestLinkBusA_Id_FK", testLinkBusAId);
  query.bindValue(":TestLinkBusB_Id_FK", testLinkBusBId);
  query.bindValue(":SequenceNumber", sequenceNumber);
  query.bindValue(":ExpectedValue", expectedValue);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestModel::removeTestItem(const QVariant & testItemId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestModelItem_tbl WHERE Id_PK = " + testItemId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestModel::removeTestItems(const QModelIndexList & indexListOfSelectedRows)
{
  int row;

  for(row = 0; row < indexListOfSelectedRows.size(); ++row){
    if(!removeTestItem(indexListOfSelectedRows.at(row).data())){
      return false;
    }
  }

  return true;
}

QVariant mdtTtTestModel::getTestLinkId(const QVariant & testCableId, const QVariant & testConnectionId, bool & ok)
{
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT Id_PK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND TestConnection_Id_FK = " + testConnectionId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    return QVariant();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

QList<QVariant> mdtTtTestModel::getTestConnectionIdListLinkedToTestConnectionByDutLinks(const QVariant & testCableId, const QVariant & fromTestConnectionId, mdtClPathGraph & graph, bool & ok)
{
  QString sql;
  QVariant fromDutConnection;
  QList<QVariant> linkedDutConnectionIdList;
  QList<QVariant> linkedTestConnectionIdList;
  QList<QVariant> dataList;
  int i;

  // Get DUT connection for given test connection
  sql = "SELECT DutConnection_Id_FK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND TestConnection_Id_FK = " + fromTestConnectionId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return linkedTestConnectionIdList;
  }
  if(dataList.isEmpty()){
    ok = false;
    pvLastError.setError(tr("Cannot find DUT connection for test connecion ID ") + fromTestConnectionId.toString(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return linkedTestConnectionIdList;
  }
  Q_ASSERT(dataList.size() == 1);
  fromDutConnection = dataList.at(0);
  // Get linked DUT connections
  linkedDutConnectionIdList = graph.getLinkedConnectionIdList(fromDutConnection);
  // Build linked test connection id list
  for(i = 0; i < linkedDutConnectionIdList.size(); ++i){
    sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl";
    sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
    sql += " AND DutConnection_Id_FK = " + linkedDutConnectionIdList.at(i).toString();
    sql += " AND TestConnection_Id_FK <> " + fromTestConnectionId.toString();
    dataList = getDataList<QVariant>(sql, ok);
    if(!ok){
      return linkedTestConnectionIdList;
    }
    /**
    if(dataList.isEmpty()){
      ok = false;
      pvLastError.setError(tr("Cannot find test connection for DUT connecion ID ") + linkedDutConnectionIdList.at(i).toString(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
      pvLastError.commit();
      return linkedTestConnectionIdList;
    }
    */
    if(!dataList.isEmpty()){
      Q_ASSERT(dataList.size() == 1);
      linkedTestConnectionIdList.append(dataList.at(0));
    }
  }

  return linkedTestConnectionIdList;
}

bool mdtTtTestModel::generateContinuityTest(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId, const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph)
{
  mdtTtTestNode tn(0, database());
  mdtTtTestModelItem tmi(0, database());
  QSqlQuery query(database());
  QList<QVariant> solvedTestConnectionIdList;
  QList<QVariant> testConnectionIdList;
  QList<QVariant> linkedTestConnectionIdList;
  QList<QVariant> fromTestConnectionPathConnectionIdList;
  QList<QVariant> fromLinkedTestConnectionPathConnectionIdList;
  QVariant forTestConnectionMeasureConnectionId;
  QVariant forLinkedTestConnectionMeasureConnectionId;
  QVariant testConnectionId;
  QVariant linkedTestConnectionId;
  QVariant forTestConnectionLinkId;
  QVariant forLinkedTestConnectionLinkId;
  mdtSqlRecord itemData;
  QVariant testModelItemId;
  ///mdtSqlRecord routeData;
  QList<mdtTtTestNodeUnitSetupData> forTestConnectionSetupDataList;
  QList<mdtTtTestNodeUnitSetupData> forLinkedTestConnectionSetupDataList;
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;
  bool noShort;
  int i, k, m;
  int sequenceNumber = 0;

  // Setup test model item data
  if(!itemData.addAllFields("TestModelItem_tbl", database())){
    pvLastError = itemData.lastError();
    return false;
  }
  // Setup route data
  /**
  if(!routeData.addAllFields("TestModelItemRoute_tbl", database())){
    pvLastError = routeData.lastError();
    return false;
  }
  */
  // Get test connections from given cable
  sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl WHERE TestCable_Id_FK = " + testCableId.toString();
  testConnectionIdList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  // Add relays of test node
  if(!tn.addRelaysToGraph(testNodeId, graph)){
    pvLastError = tn.lastError();
    return false;
  }
  if(!beginTransaction()){
    return false;
  }
  // Process test connections
  for(i = 0; i < testConnectionIdList.size(); ++i){
    testConnectionId = testConnectionIdList.at(i);
    Q_ASSERT(!testConnectionId.isNull());
    if(solvedTestConnectionIdList.contains(testConnectionId)){
      continue;
    }
    // Search paths from test connection to a measure connection
    forTestConnectionMeasureConnectionId = measureConnexionIdA;
    forLinkedTestConnectionMeasureConnectionId = measureConnexionIdB;
    fromTestConnectionPathConnectionIdList = graph.getShortestPath(testConnectionId, measureConnexionIdA);
    if(fromTestConnectionPathConnectionIdList.isEmpty()){
      fromTestConnectionPathConnectionIdList = graph.getShortestPath(testConnectionId, measureConnexionIdB);
      if(fromTestConnectionPathConnectionIdList.isEmpty()){
        pvLastError.setError(tr("Cannot find a route from test connection ID ") + testConnectionId.toString() + tr(" to a measure connection."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
        pvLastError.commit();
        rollbackTransaction();
        return false;
      }
      forTestConnectionMeasureConnectionId = measureConnexionIdB;
      forLinkedTestConnectionMeasureConnectionId = measureConnexionIdA;
    }
    // Build setup data list
    forTestConnectionSetupDataList = tn.getRelayPathSetupDataList(fromTestConnectionPathConnectionIdList, graph, ok);
    if(!ok){
      pvLastError = tn.lastError();
      rollbackTransaction();
      return false;
    }
    // Search test connections that are linked to current test connection, passing by DUT
    linkedTestConnectionIdList = getTestConnectionIdListLinkedToTestConnectionByDutLinks(testCableId, testConnectionId, graph, ok);
    if(!ok){
      rollbackTransaction();
      return false;
    }
    // Get link ID
    forTestConnectionLinkId = getTestLinkId(testCableId, testConnectionId, ok);
    if(!ok){
      rollbackTransaction();
      return false;
    }
    Q_ASSERT(!forTestConnectionLinkId.isNull());
    for(k = 0; k < linkedTestConnectionIdList.size(); ++k){
      linkedTestConnectionId = linkedTestConnectionIdList.at(k);
      Q_ASSERT(!linkedTestConnectionId.isNull());
      // Search path from linked test connection to other measure connection
      fromLinkedTestConnectionPathConnectionIdList = graph.getShortestPath(linkedTestConnectionId, forLinkedTestConnectionMeasureConnectionId);
      if(fromLinkedTestConnectionPathConnectionIdList.isEmpty()){
        pvLastError.setError(tr("Cannot find a route from test connection ID ") + linkedTestConnectionId.toString() + tr(" to a measure connection."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
        pvLastError.commit();
        rollbackTransaction();
        return false;
      }
      // Get setup data list
      forLinkedTestConnectionSetupDataList = tn.getRelayPathSetupDataList(fromLinkedTestConnectionPathConnectionIdList, graph, ok);
      if(!ok){
        pvLastError = tn.lastError();
        rollbackTransaction();
        return false;
      }
      // Check absence of short
      noShort = tn.ensureAbsenceOfShortCircuit(forTestConnectionMeasureConnectionId, forLinkedTestConnectionMeasureConnectionId, forTestConnectionSetupDataList + forLinkedTestConnectionSetupDataList, ok);
      if(!ok){
        pvLastError = tn.lastError();
        rollbackTransaction();
        return false;
      }
      if(!noShort){
        pvLastError.setError(tr("A generated setup makes a internal short between measure connections."), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
        pvLastError.commit();
        rollbackTransaction();
        return false;
      }
      // Get link ID
      forLinkedTestConnectionLinkId = getTestLinkId(testCableId, linkedTestConnectionId, ok);
      if(!ok){
        rollbackTransaction();
        return false;
      }
      Q_ASSERT(!forLinkedTestConnectionLinkId.isNull());
      // Create a new test item
      itemData.setValue("TestModel_Id_FK", testModelId);
      itemData.setValue("SequenceNumber", sequenceNumber);
      ++sequenceNumber;
      itemData.setValue("ExpectedValue", 0.0);
      itemData.setValue("ValueUnit", "Ohm");
      if(!addRecord(itemData, "TestModelItem_tbl", query)){
        rollbackTransaction();
        return false;
      }
      testModelItemId = query.lastInsertId();
      Q_ASSERT(!testModelItemId.isNull());
      // Add test link that concern testConnectionId to item
      if(!tmi.addTestLink(testModelItemId, forTestConnectionLinkId)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      // Update setup data
      for(m = 0; m < forTestConnectionSetupDataList.size(); ++m){
        forTestConnectionSetupDataList[m].setValue("TestModelItem_Id_FK", testModelItemId);
      }
      // Add route from test connection to assigned measure connection
      if(!tmi.addRoute(testModelItemId, forTestConnectionLinkId, forTestConnectionMeasureConnectionId, forTestConnectionSetupDataList, false)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      // Add test link that concern linkedTestConnectionId to item
      if(!tmi.addTestLink(testModelItemId, forLinkedTestConnectionLinkId)){
        rollbackTransaction();
        return false;
      }
      // Update setup data
      for(m = 0; m < forLinkedTestConnectionSetupDataList.size(); ++m){
        forLinkedTestConnectionSetupDataList[m].setValue("TestModelItem_Id_FK", testModelItemId);
      }
      // Add route from linked test connection to assigned measure connection
      if(!tmi.addRoute(testModelItemId, forLinkedTestConnectionLinkId, forLinkedTestConnectionMeasureConnectionId, forLinkedTestConnectionSetupDataList, false)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      solvedTestConnectionIdList.append(linkedTestConnectionId);
    }
    solvedTestConnectionIdList.append(testConnectionId);
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestModel::generateTestNodeUnitSetup(const QVariant & testId)
{
  mdtTtTestModelItem tmi(this, database());
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant testItemId;

  // Get test items for given test ID
  sql = "SELECT Id_PK FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of test items", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return false;
  }
  while(query.next()){
    // Get test item data
    testItemId = query.value(0);
    if(!tmi.generateTestNodeUnitSetup(testItemId)){
      return false;
    }
  }

  return true;
}
