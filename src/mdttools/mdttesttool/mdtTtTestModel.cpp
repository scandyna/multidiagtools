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
#include <limits>

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

mdtTtTestModelData mdtTtTestModel::getTestModelData(const QVariant & testModelId, bool & ok)
{
  QString sql;
  QList<QSqlRecord> dataList;

  sql = "SELECT * FROM TestModel_tbl WHERE Id_PK = " + testModelId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return mdtTtTestModelData();
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Could not find data in 'TestModel_tbl' for Id_PK = '") + testModelId.toString() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    ok = false;
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
    pvLastError.setError("Cannot get list of sequnec number", mdtError::Error);
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

QList<QSqlRecord> mdtTtTestModel::getTestNodeDataList(const QVariant & testModelId, bool & ok)
{
  QString sql;
  QList<QSqlRecord> dataList;

  sql = "SELECT\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestModel_TestNode_tbl TMTN\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TMTN.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";
  sql += " WHERE TMTN.TestModel_Id_FK = " + testModelId.toString();
  sql += " ORDER BY NodeIdentification ASC";
  dataList = getDataList<QSqlRecord>(sql, ok);

  return dataList;
}

bool mdtTtTestModel::removeTestNodes(const mdtSqlTableSelection & s)
{
  return removeData("TestModel_TestNode_tbl", s, true);
}

bool mdtTtTestModel::removeTestModelItem(const QVariant & testModelItemId, bool removeSetupAndRoute, bool removeTestLinks, bool handleTransaction)
{
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  if(removeSetupAndRoute){
    if(!removeData("TestNodeUnitSetup_tbl", "TestModelItem_Id_FK", testModelItemId)){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
    if(!removeData("TestModelItemRoute_tbl", "TestModelItem_Id_FK", testModelItemId)){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  if(removeTestLinks){
    if(!removeData("TestModelItem_TestLink_tbl", "TestModelItem_Id_FK", testModelItemId)){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  if(!removeData("TestModelItem_tbl", "Id_PK", testModelItemId)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtTtTestModel::removeTestModelItems(const mdtSqlTableSelection & s, bool removeSetupAndRoute, bool removeTestLinks)
{
  int row;

  if(!beginTransaction()){
    return false;
  }
  for(row = 0; row < s.rowCount(); ++row){
    if(!removeTestModelItem(s.data(row, "Id_PK"), removeSetupAndRoute, removeTestLinks, false)){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

QList<QVariant> mdtTtTestModel::getTestLinkIdListForDutConnectionId(const QVariant & testCableId, const QVariant & dutConnectionId, bool & ok)
{
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT Id_PK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND DutConnection_Id_FK = " + dutConnectionId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return dataList;
  }
  if(dataList.isEmpty()){
    ok = false;
    QString testCableIdStr;
    QString dutConnectionName;
    QString msg;
    // Get infos
    testCableIdStr = getTestCableIdString(testCableId);
    dutConnectionName = getUnitContactName(dutConnectionId);
    // Build error text
    msg = tr("A error was detected in cable '") + testCableIdStr + tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    // Build error informative text
    msg = tr("DUT connection '") + dutConnectionName + tr("' is not linked to any test connection.");
    pvLastError.setInformativeText(msg);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return dataList;
  }

  return dataList;
}

QVariant mdtTtTestModel::getTestConnectionId(const QVariant & testLinkId, const QVariant & dutConnectionId, bool & ok)
{
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl";
  sql += " WHERE Id_PK = " + testLinkId.toString();
  sql += " AND DutConnection_Id_FK = " + dutConnectionId.toString();
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

mdtTtTestModelItemRouteData mdtTtTestModel::buildRouteData(const QList<QVariant> & pathConnectionIdList, mdtClPathGraph & graph, bool & ok)
{
  Q_ASSERT(pathConnectionIdList.size() >= 2);

  mdtTtTestModelItemRouteData data;
  mdtTtTestNodeUnitSetupData setupData;
  QVariant relayId;
  int i;

  // Setup setup data
  if(!setupData.setup(database())){
    ok = false;
    pvLastError = setupData.lastError();
    return data;
  }
  // Build relays list
  for(i = 1; i < pathConnectionIdList.size(); ++i){
    Q_ASSERT(!pathConnectionIdList.at(i-1).isNull());
    Q_ASSERT(!pathConnectionIdList.at(i).isNull());
    relayId = graph.getUserData(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
    if(!relayId.isNull()){
      setupData.clearValues();
      setupData.setValue("TestNodeUnit_Id_FK", relayId);
      setupData.setValue("State", true);
      data.addSetupData(setupData);
    }
  }
  ok = true;

  return data;
}

bool mdtTtTestModel::addRoutesToItemData(const QVariant & dutConnectionId1, const QVariant & dutConnectionId2,
                                         const mdtTtTestModelGenerationParameter & parameters, mdtTtTestModelItemData & itemData,
                                         mdtClPathGraph & graph, mdtClPathGraph & graphForShortDetection)
{
  Q_ASSERT(!dutConnectionId1.isNull());
  Q_ASSERT(!dutConnectionId2.isNull());
  Q_ASSERT(!parameters.measureConnexionIdA.isNull());
  Q_ASSERT(!parameters.measureConnexionIdB.isNull());
  Q_ASSERT(!parameters.testCableId.isNull());

  mdtTtTestNode tn(0, database());
  QList<QVariant> testLinkIdList1;
  QList<QVariant> testLinkIdList2;
  QVariant testConnectionId1;
  QVariant testConnectionId2;
  QList<QVariant> pathConnectionIdList1;
  QList<QVariant> pathConnectionIdList2;
  mdtTtTestModelItemRouteData routeData1;
  mdtTtTestModelItemRouteData routeData2;
  bool ok;
  bool noShort;
  int i, k;

  // Get link ID list 1
  testLinkIdList1 = getTestLinkIdListForDutConnectionId(parameters.testCableId, dutConnectionId1, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(testLinkIdList1.size() > 0);
  // Get link ID list 2
  testLinkIdList2 = getTestLinkIdListForDutConnectionId(parameters.testCableId, dutConnectionId2, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(testLinkIdList2.size() > 0);
  /*
   * Try each combinaison of test link
   *  and retain the first that gives a path to measure connections,
   *  and that does not make a short (between measure connections) in test node
   */
  for(i = 0; i < testLinkIdList1.size(); ++i){
    for(k = 0; k < testLinkIdList2.size(); ++k){
      // Get test connection ID 1
      testConnectionId1 = getTestConnectionId(testLinkIdList1.at(i), dutConnectionId1, ok);
      if(!ok){
        return false;
      }
      Q_ASSERT(!testConnectionId1.isNull());
      // Get test connection ID 2
      testConnectionId2 = getTestConnectionId(testLinkIdList2.at(k), dutConnectionId2, ok);
      if(!ok){
        return false;
      }
      Q_ASSERT(!testConnectionId2.isNull());
      // Get path of both test connection to measure connections
      ok = searchPathFromTestConnectionsToMeasureConnections(testConnectionId1, testConnectionId2, parameters.measureConnexionIdA, parameters.measureConnexionIdB, pathConnectionIdList1, pathConnectionIdList2, graph);
      if(!ok){
        continue;
      }
      // Build both route data
      routeData1 = buildRouteData(pathConnectionIdList1, graph, ok);
      if(!ok){
        return false;
      }
      routeData2 = buildRouteData(pathConnectionIdList2, graph, ok);
      if(!ok){
        return false;
      }
      // Set measure connections to both route data
      if((pathConnectionIdList1.contains(parameters.measureConnexionIdA))&&(pathConnectionIdList2.contains(parameters.measureConnexionIdB))){
        routeData1.setMeasureConnectionId(parameters.measureConnexionIdA);
        routeData2.setMeasureConnectionId(parameters.measureConnexionIdB);
      }else{
        routeData1.setMeasureConnectionId(parameters.measureConnexionIdB);
        routeData2.setMeasureConnectionId(parameters.measureConnexionIdA);
      }
      // Update route data 1
      routeData1.setDutConnectionId(dutConnectionId1);
      routeData1.setTestLinkId(testLinkIdList1.at(i));
      routeData1.setTestConnectionId(testConnectionId1);
      // Update route data 2
      routeData2.setDutConnectionId(dutConnectionId2);
      routeData2.setTestLinkId(testLinkIdList2.at(k));
      routeData2.setTestConnectionId(testConnectionId2);
      // Check absence of short
      noShort = tn.ensureAbsenceOfShortCircuit(routeData1.measureConnectionId(), routeData2.measureConnectionId(), 
                                                routeData1.setupDataList() + routeData2.setupDataList(), graphForShortDetection, ok);
      if(!ok){
        pvLastError = tn.lastError();
        return false;
      }
      
      qDebug() << "Has short ? : " << !noShort;
      
      if(noShort){
        // Ok, we have it - Add route data to test model item data
        itemData.addRouteData(routeData1);
        itemData.addRouteData(routeData2);
        return true;
      }
    }
  }
  // We did not found 2 routes
  QString testCableIdStr;
  QString dutConnectionName1;
  QString dutConnectionName2;
  QString msg;
  // Get infos
  testCableIdStr = getTestCableIdString(parameters.testCableId);
  dutConnectionName1 = getUnitContactName(dutConnectionId1);
  dutConnectionName2 = getUnitContactName(dutConnectionId2);
  // Build error text
  msg = tr("A problem was detected in cable '") + testCableIdStr + tr("'.");
  pvLastError.setError(msg, mdtError::Error);
  // Build error informative text
  msg = tr("Could not find routes to for checks between DUT connection '") + dutConnectionName1 + tr("' and DUT connection '") + dutConnectionName2 + tr("'");
  pvLastError.setInformativeText(msg);
  MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
  pvLastError.commit();

  return false;
}


bool mdtTtTestModel::generateTestModel(mdtTtTestModelGenerationParameter & parameters, mdtClPathGraph & graph)
{
  Q_ASSERT(!parameters.measureConnexionIdA.isNull());
  Q_ASSERT(!parameters.measureConnexionIdB.isNull());
  Q_ASSERT(!parameters.testCableId.isNull());
  Q_ASSERT(!parameters.testModelId.isNull());
  Q_ASSERT(!parameters.testNodeId.isNull());

  mdtTtTestNode tn(0, database());
  mdtTtTestModelItem tmi(0, database());
  QList<QVariant> dutConnectionIdList;
  mdtTtTestModelItemRouteData routeData1;
  mdtTtTestModelItemRouteData routeData2;
  mdtTtTestModelItemData itemData;
  QString sql;
  bool ok;
  bool linked;
  mdtClPathGraph graphForShortDetection(database());
  int i, k;
  int sequenceNumber = 0;
  QVariant var;

  // Setup test model item data
  if(!itemData.setup(database())){
    pvLastError = itemData.lastError();
    return false;
  }
  // Get next sequence number
  var = getNextSequenceNumber(parameters.testModelId);
  if(var.isNull()){
    return false;
  }
  sequenceNumber = var.toInt();
  // Get DUT connections from given cable
  sql = "SELECT DutConnection_Id_FK FROM TestLink_tbl WHERE TestCable_Id_FK = " + parameters.testCableId.toString();
  dutConnectionIdList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  // Add relays of test node to graph
  if(!tn.addRelaysToGraph(parameters.testNodeId, graph)){
    pvLastError = tn.lastError();
    return false;
  }
  // Init short detetcion graph
  if(!graphForShortDetection.loadLinkList()){
    pvLastError = graphForShortDetection.lastError();
    return false;
  }
  if(!beginTransaction()){
    return false;
  }
  // Process each DUT connections combinaison
  for(i = 0; i < (dutConnectionIdList.size()-1); ++i){
    for(k = i+1; k < dutConnectionIdList.size(); ++k){
      
      qDebug() << "Processing " << dutConnectionIdList.at(i) << "-" << dutConnectionIdList.at(k) << " ...";
      
      linked = graph.connectionsAreLinked(dutConnectionIdList.at(i), dutConnectionIdList.at(k));
      if((linked)||(parameters.generateForNonLinkedConnections)){
        // Clear previous data
        itemData.clearValues();
        // Build routes
        if(!addRoutesToItemData(dutConnectionIdList.at(i), dutConnectionIdList.at(k), parameters, itemData, graph, graphForShortDetection)){
          rollbackTransaction();
          return false;
        }
        Q_ASSERT(itemData.routeDataList().size() == 2);
        routeData1 = itemData.routeDataList().at(0);
        routeData2 = itemData.routeDataList().at(1);
        // Set items model id
        itemData.setValue("TestModel_Id_FK", parameters.testModelId);
        // Set item's expected values regarding if linked or not
        if(linked){
          itemData.setValue("ExpectedValue", parameters.continuityExpectedValue);
        }else{
          itemData.setValue("ExpectedValue", parameters.isolationExpectedValue);
        }
        // Set designation (regarding parameters)
        itemData.setValue("DesignationEN", getDesignationEN(routeData1.testLinkId(), routeData2.testLinkId()));
        // Set sequenceNumber
        itemData.setValue("SequenceNumber", sequenceNumber);
        ++sequenceNumber;
        // Set value unit
        itemData.setValue("ValueUnit", "Ohm");
        // Add test model item to DB
        if(!tmi.addItem(itemData, false)){
          rollbackTransaction();
          pvLastError = tmi.lastError();
          return false;
        }
      }
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}


bool mdtTtTestModel::searchPathFromTestConnectionsToMeasureConnections(const QVariant & testConnectionId1, const QVariant & testConnectionId2,
                                                         const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB,
                                                         QList<QVariant> & pathConnectionIdList1, QList<QVariant> & pathConnectionIdList2,
                                                         mdtClPathGraph & graph)
{
  bool ok;

  // Try test connection 1 -> measure connection A
  pathConnectionIdList1 = graph.getShortestPath(testConnectionId1, measureConnexionIdA, ok);
  if(!ok){
    pathConnectionIdList1.clear();
    pvLastError = graph.lastError();
    return false;
  }
  if(!pathConnectionIdList1.isEmpty()){
    // Test connection 1 -> measure connection A found, search test connection 2 -> measure connecion B
    pathConnectionIdList2 = graph.getShortestPath(testConnectionId2, measureConnexionIdB, ok);
    if(!ok){
      pathConnectionIdList1.clear();
      pathConnectionIdList2.clear();
      pvLastError = graph.lastError();
      return false;
    }
    if(pathConnectionIdList2.isEmpty()){
      QString tc, mc;
      tc = getUnitContactName(testConnectionId2);
      mc = getUnitContactName(measureConnexionIdB);
      pvLastError.setError(tr("Cannot find a route from test connection ") + tc + tr(" to measure connection ") + mc , mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
      pvLastError.commit();
      return false;
    }
  }else{
    // Search path from test connecion 1 to measure connecion B
    pathConnectionIdList1 = graph.getShortestPath(testConnectionId1, measureConnexionIdB, ok);
    if(!ok){
      pathConnectionIdList1.clear();
      pvLastError = graph.lastError();
      return false;
    }
    if(!pathConnectionIdList1.isEmpty()){
      // Path from test connecion 1 to measure connecion B found, search from test connecion 2 to measure connecion A
      pathConnectionIdList2 = graph.getShortestPath(testConnectionId2, measureConnexionIdA, ok);
      if(!ok){
        pathConnectionIdList1.clear();
        pathConnectionIdList2.clear();
        pvLastError = graph.lastError();
        return false;
      }
      if(pathConnectionIdList2.isEmpty()){
        QString tc, mc;
        tc = getUnitContactName(testConnectionId2);
        mc = getUnitContactName(measureConnexionIdA);
        pvLastError.setError(tr("Cannot find a route from test connection ") + tc + tr(" to measure connection ") + mc , mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
        pvLastError.commit();
        return false;
      }
    }
  }
  Q_ASSERT(pathConnectionIdList1.size() >= 2);
  Q_ASSERT(pathConnectionIdList2.size() >= 2);

  return true;
}

QString mdtTtTestModel::getUnitContactName(const QVariant & id)
{
  bool ok;
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT UnitContactName FROM UnitConnection_tbl WHERE Id_PK = " + id.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return "??";
  }
  if(dataList.isEmpty()){
    return "??";
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).toString();
}

QString mdtTtTestModel::getTestCableIdString(const QVariant & id)
{
  bool ok;
  QString sql;
  QList<QSqlRecord> dataList;
  QString str;

  sql = "SELECT Identification, Key FROM TestCable_tbl WHERE Id_PK = " + id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return "??";
  }
  if(dataList.isEmpty()){
    return "??";
  }
  Q_ASSERT(dataList.size() == 1);
  // Build string
  str = dataList.at(0).value(0).toString();
  str += " (key: " + dataList.at(0).value(1).toString() + ")";

  return str;
}

QVariant mdtTtTestModel::getDesignationEN(const QVariant & fromTestLinkId, const QVariant & toTestLinkId)
{
  QString designation;
  QString str;
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord dataA;
  QSqlRecord dataB;
  bool ok;

  // Get first link part
  sql = "SELECT TestLinkIdentification, DutUnitSchemaPosition, DutUnitAlias, DutConnectorName, DutContactName ";
  sql += "FROM TestLink_view";
  sql += " WHERE Id_PK = " + fromTestLinkId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return "<Error!>";
  }
  if(dataList.isEmpty()){
    return "<Error!>";
  }
  Q_ASSERT(dataList.size() == 1);
  dataA = dataList.at(0);
  // Get second link part
  sql = "SELECT TestLinkIdentification, DutUnitSchemaPosition, DutUnitAlias, DutConnectorName, DutContactName ";
  sql += "FROM TestLink_view";
  sql += " WHERE Id_PK = " + toTestLinkId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return "<Error!>";
  }
  if(dataList.isEmpty()){
    return "<Error!>";
  }
  Q_ASSERT(dataList.size() == 1);
  dataB = dataList.at(0);
  // Build designation
  designation = dataA.value("TestLinkIdentification").toString() + "-" + dataB.value("TestLinkIdentification").toString();
  designation += " ( ";
  designation += dataA.value("DutUnitSchemaPosition").toString();
  str = dataA.value("DutUnitAlias").toString().trimmed();
  if(!str.isEmpty()){
    designation += "(" + str + ") ";
  }
  designation += ",";
  str = dataA.value("DutConnectorName").toString().trimmed();
  if(!str.isEmpty()){
    designation += str;
  }
  designation += ";";
  designation += dataA.value("DutContactName").toString().trimmed();
  designation += " - ";
  designation += dataB.value("DutUnitSchemaPosition").toString();
  str = dataB.value("DutUnitAlias").toString().trimmed();
  if(!str.isEmpty()){
    designation += "(" + str + ") ";
  }
  designation += ",";
  str = dataB.value("DutConnectorName").toString().trimmed();
  if(!str.isEmpty()){
    designation += str;
  }
  designation += ";";
  designation += dataB.value("DutContactName").toString().trimmed();
  designation += " )";

  return designation;
}
