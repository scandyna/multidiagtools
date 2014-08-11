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

/*
 * Connection data container
 */

struct ConnectionData
{
  QVariant testConnectionId;
  QList<QVariant> testConnectionIdList;
  QVariant measureConnexionId;
  QList<QVariant> pathToMeasureConnection;
  QVariant testLinkId;
  QList<mdtTtTestNodeUnitSetupData> setupDataList;

  void setTestModelItemIdToSetupDataList(const QVariant & testModelItemId);
};

void ConnectionData::setTestModelItemIdToSetupDataList(const QVariant& testModelItemId)
{
  int i;

  for(i = 0; i < setupDataList.size(); ++i){
    setupDataList[i].setValue("TestModelItem_Id_FK", testModelItemId);
  }
}

/*
 * mdtTtTestModelAbstractGeneratorHelper implementation
 */

QList< QVariant > mdtTtTestModelAbstractGeneratorHelper::relaysToIgnoreIdList()
{
  return QList<QVariant>();
}


QVariant mdtTtTestModelAbstractGeneratorHelper::getDesignationEN(const QVariant& fromTestLinkId, const QVariant& toTestLinkId)
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
  dataList = pvTestModel->getDataList<QSqlRecord>(sql, ok);
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
  dataList = pvTestModel->getDataList<QSqlRecord>(sql, ok);
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

QVariant mdtTtTestModelAbstractGeneratorHelper::getDutConnectionId(const QVariant& testCableId, const QVariant& testConnectionId, bool& ok)
{
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT DutConnection_Id_FK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND TestConnection_Id_FK = " + testConnectionId.toString();
  dataList = pvTestModel->getDataList<QVariant>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    return QVariant();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

QVariant mdtTtTestModelAbstractGeneratorHelper::getTestConnectionId(const QVariant& testCableId, const QVariant& dutConnectionId, bool& ok, const QVariant& testConnectionToExclude)
{
  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND DutConnection_Id_FK = " + dutConnectionId.toString();
  if(!testConnectionToExclude.isNull()){
    sql += " AND TestConnection_Id_FK <> " + testConnectionToExclude.toString();
  }
  dataList = pvTestModel->getDataList<QVariant>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    return QVariant();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}


/*
 * mdtTtTestModelContinuityTestGeneratorHelper implementation
 */

mdtTtTestModelContinuityTestGeneratorHelper::mdtTtTestModelContinuityTestGeneratorHelper(mdtTtTestModel* tm)
 : mdtTtTestModelAbstractGeneratorHelper(tm)
{
}

mdtTtTestModelContinuityTestGeneratorHelper::~mdtTtTestModelContinuityTestGeneratorHelper()
{
}

QList<QVariant> mdtTtTestModelContinuityTestGeneratorHelper::getRelatedTestConnectionIdList(const QVariant& testCableId, const QVariant& fromTestConnectionId, mdtClPathGraph& graph, bool& ok)
{
  QVariant fromDutConnection;
  QList<QVariant> linkedDutConnectionIdList;
  QVariant linkedTestConnectionId;
  QList<QVariant> linkedTestConnectionIdList;
  int i;

  // Get DUT connection for given test connection
  fromDutConnection = getDutConnectionId(testCableId, fromTestConnectionId, ok);
  if(!ok){
    return linkedTestConnectionIdList;
  }
  if(fromDutConnection.isNull()){
    ok = false;
    pvLastError.setError(pvTestModel->tr("Cannot find DUT connection for test connecion ID ") + fromTestConnectionId.toString(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelContinuityTestGeneratorHelper");
    pvLastError.commit();
    return linkedTestConnectionIdList;
  }
  // Get linked DUT connections
  linkedDutConnectionIdList = graph.getLinkedConnectionIdList(fromDutConnection);
  // Build linked test connection id list
  for(i = 0; i < linkedDutConnectionIdList.size(); ++i){
    linkedTestConnectionId = getTestConnectionId(testCableId, linkedDutConnectionIdList.at(i), ok, fromTestConnectionId);
    if(!ok){
      return linkedTestConnectionIdList;
    }
    if(!linkedTestConnectionId.isNull()){
      linkedTestConnectionIdList.append(linkedTestConnectionId);
    }
  }

  return linkedTestConnectionIdList;
}

bool mdtTtTestModelContinuityTestGeneratorHelper::queueRelatedConnectionToSolved()
{
  return true;
}

mdtTtTestModelAbstractGeneratorHelper::action_t mdtTtTestModelContinuityTestGeneratorHelper::actionOnShortInTestNode()
{
  return Fail;
}

QVariant mdtTtTestModelContinuityTestGeneratorHelper::expectedValue()
{
  return 0.0;
}

QVariant mdtTtTestModelContinuityTestGeneratorHelper::valueUnit()
{
  return "Ohm";
}

/*
 * mdtTtTestModelIsolationTestGeneratorHelper implementation
 */

mdtTtTestModelIsolationTestGeneratorHelper::mdtTtTestModelIsolationTestGeneratorHelper(mdtTtTestModel* tm)
 : mdtTtTestModelAbstractGeneratorHelper(tm)
{
}

mdtTtTestModelIsolationTestGeneratorHelper::~mdtTtTestModelIsolationTestGeneratorHelper()
{
}

void mdtTtTestModelIsolationTestGeneratorHelper::setRelaysToIgnoreIdList(const QList< QVariant >& idList)
{
  pvRelaysToIgnoreIdList = idList;
}

QList< QVariant > mdtTtTestModelIsolationTestGeneratorHelper::relaysToIgnoreIdList()
{
    return pvRelaysToIgnoreIdList;
}

QList< QVariant > mdtTtTestModelIsolationTestGeneratorHelper::getRelatedTestConnectionIdList(const QVariant& testCableId, const QVariant& fromTestConnectionId, mdtClPathGraph& graph, bool& ok)
{
  QString sql;
  QVariant fromDutConnection;
  QList<QVariant> linkedDutConnectionIdList;
  QList<QVariant> notLinkedTestConnectionIdList;
  int i;

  // Get DUT connection for given test connection
  fromDutConnection = getDutConnectionId(testCableId, fromTestConnectionId, ok);
  if(!ok){
    return notLinkedTestConnectionIdList;
  }
  if(fromDutConnection.isNull()){
    ok = false;
    pvLastError.setError(pvTestModel->tr("Cannot find DUT connection for test connecion ID ") + fromTestConnectionId.toString(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelIsolationTestGeneratorHelper");
    pvLastError.commit();
    return notLinkedTestConnectionIdList;
  }
  // Get not linked DUT connections
  linkedDutConnectionIdList = graph.getLinkedConnectionIdList(fromDutConnection);
  sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += " AND TestConnection_Id_FK <> " + fromTestConnectionId.toString();
  sql += " AND DutConnection_Id_FK NOT IN (";
  for(i = 0; i < (linkedDutConnectionIdList.size()-1); ++i){
    sql += linkedDutConnectionIdList.at(i).toString() + ",";
  }
  sql += linkedDutConnectionIdList.at(linkedDutConnectionIdList.size()-1).toString() + ")";
  notLinkedTestConnectionIdList = pvTestModel->getDataList<QVariant>(sql, ok);

  return notLinkedTestConnectionIdList;
}

bool mdtTtTestModelIsolationTestGeneratorHelper::queueRelatedConnectionToSolved()
{
  return false;
}

mdtTtTestModelAbstractGeneratorHelper::action_t mdtTtTestModelIsolationTestGeneratorHelper::actionOnShortInTestNode()
{
  return IgnoreCurrentItem;
}

QVariant mdtTtTestModelIsolationTestGeneratorHelper::expectedValue()
{
  return std::numeric_limits<double>::infinity();
}

QVariant mdtTtTestModelIsolationTestGeneratorHelper::valueUnit()
{
  return "Ohm";
}


/*
 * mdtTtTestModel class implementation
 */

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

bool mdtTtTestModel::generateContinuityTest(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId,
                                            const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph)
{
  mdtTtTestModelContinuityTestGeneratorHelper helper(this);

  return generateTestModel(testModelId, testCableId, testNodeId, measureConnexionIdA, measureConnexionIdB, graph, helper);
}

bool mdtTtTestModel::generateShortDetectionTest(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId,
                                                const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, mdtClPathGraph & graph,
                                                const QList<QVariant> & relaysToIgnoreIdList)
{
  mdtTtTestModelIsolationTestGeneratorHelper helper(this);

  helper.setRelaysToIgnoreIdList(relaysToIgnoreIdList);

  return generateTestModel(testModelId, testCableId, testNodeId, measureConnexionIdA, measureConnexionIdB, graph, helper);
}


bool mdtTtTestModel::generateTestModel(const QVariant & testModelId, const QVariant & testCableId, const QVariant & testNodeId, 
                                       const QVariant & measureConnexionIdA, const QVariant & measureConnexionIdB, 
                                       mdtClPathGraph & graph, mdtTtTestModelAbstractGeneratorHelper & helper)
{
  mdtTtTestNode tn(0, database());
  mdtTtTestModelItem tmi(0, database());
  QSqlQuery query(database());
  QList<QVariant> solvedTestConnectionIdList;
  ConnectionData data;
  ConnectionData otherData;
  mdtSqlRecord itemData;
  QVariant testModelItemId;
  QString sql;
  bool ok;
  bool noShort;
  mdtClPathGraph graphForShortDetection(database());
  int i, k;
  int sequenceNumber = 0;
  QVariant var;

  // Setup test model item data
  if(!itemData.addAllFields("TestModelItem_tbl", database())){
    pvLastError = itemData.lastError();
    return false;
  }
  // Get next sequence number
  var = getNextSequenceNumber(testModelId);
  if(var.isNull()){
    return false;
  }
  sequenceNumber = var.toInt();
  // Get test connections from given cable
  sql = "SELECT TestConnection_Id_FK FROM TestLink_tbl WHERE TestCable_Id_FK = " + testCableId.toString();
  data.testConnectionIdList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  // Add relays of test node to graph
  if(!tn.addRelaysToGraph(testNodeId, graph, helper.relaysToIgnoreIdList())){
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
  // Process test connections
  for(i = 0; i < data.testConnectionIdList.size(); ++i){
    data.testConnectionId = data.testConnectionIdList.at(i);
    Q_ASSERT(!data.testConnectionId.isNull());
    if(solvedTestConnectionIdList.contains(data.testConnectionId)){
      continue;
    }
    // Search paths from test connection to a measure connection
    data.measureConnexionId = measureConnexionIdA;
    otherData.measureConnexionId = measureConnexionIdB;
    // Search test connections that are related to current test connection, passing by DUT
    otherData.testConnectionIdList = helper.getRelatedTestConnectionIdList(testCableId, data.testConnectionId, graph, ok);
    if(!ok){
      pvLastError = helper.lastError();
      rollbackTransaction();
      return false;
    }
    // Get link ID
    data.testLinkId = getTestLinkId(testCableId, data.testConnectionId, ok);
    if(!ok){
      rollbackTransaction();
      return false;
    }
    Q_ASSERT(!data.testLinkId.isNull());
    // Process related test connections
    for(k = 0; k < otherData.testConnectionIdList.size(); ++k){
      otherData.testConnectionId = otherData.testConnectionIdList.at(k);
      Q_ASSERT(!otherData.testConnectionId.isNull());
      // Search paths between test connections and measure connections
      if(!searchPathFromTestConnectionsToMeasureConnections(data.testConnectionId, otherData.testConnectionId,
                                                            data.measureConnexionId, otherData.measureConnexionId,
                                                            data.pathToMeasureConnection, otherData.pathToMeasureConnection,
                                                            graph)){
        ///rollbackTransaction();
        ///return false;
        continue;
      }
      // Get setup data list of test connection
      data.setupDataList = tn.getRelayPathSetupDataList(data.pathToMeasureConnection, graph, ok);
      if(!ok){
        pvLastError = tn.lastError();
        rollbackTransaction();
        return false;
      }
      // Get setup data list of related test connection
      otherData.setupDataList = tn.getRelayPathSetupDataList(otherData.pathToMeasureConnection, graph, ok);
      if(!ok){
        pvLastError = tn.lastError();
        rollbackTransaction();
        return false;
      }
      // Check absence of short
      noShort = tn.ensureAbsenceOfShortCircuit(data.measureConnexionId, otherData.measureConnexionId, data.setupDataList + otherData.setupDataList, graphForShortDetection, ok);
      if(!ok){
        pvLastError = tn.lastError();
        rollbackTransaction();
        return false;
      }
      if(!noShort){
        switch(helper.actionOnShortInTestNode()){
          case mdtTtTestModelAbstractGeneratorHelper::Fail:
            pvLastError.setError(tr("A generated setup makes a internal short between measure connections."), mdtError::Error);
            MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
            pvLastError.commit();
            rollbackTransaction();
            return false;
            break;
          case mdtTtTestModelAbstractGeneratorHelper::IgnoreCurrentItem:
            qDebug() << "Have a short at k: " << k;
            continue;
        }
      }
      // Get link ID
      otherData.testLinkId = getTestLinkId(testCableId, otherData.testConnectionId, ok);
      if(!ok){
        rollbackTransaction();
        return false;
      }
      Q_ASSERT(!otherData.testLinkId.isNull());
      // Create a new test item
      itemData.setValue("TestModel_Id_FK", testModelId);
      var = helper.getDesignationEN(data.testLinkId, otherData.testLinkId);
      itemData.setValue("DesignationEN", var);
      itemData.setValue("SequenceNumber", sequenceNumber);
      ++sequenceNumber;
      itemData.setValue("ExpectedValue", helper.expectedValue());
      itemData.setValue("ValueUnit", helper.valueUnit());
      if(!addRecord(itemData, "TestModelItem_tbl", query)){
        rollbackTransaction();
        return false;
      }
      testModelItemId = query.lastInsertId();
      Q_ASSERT(!testModelItemId.isNull());
      // Add test link that concern testConnectionId to item
      if(!tmi.addTestLink(testModelItemId, data.testLinkId)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      // Update setup data
      data.setTestModelItemIdToSetupDataList(testModelItemId);
      // Add route from test connection to assigned measure connection
      if(!tmi.addRoute(testModelItemId, data.testLinkId, data.measureConnexionId, data.setupDataList, false)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      // Add test link that concern linkedTestConnectionId to item
      if(!tmi.addTestLink(testModelItemId, otherData.testLinkId)){
        rollbackTransaction();
        return false;
      }
      // Update setup data
      otherData.setTestModelItemIdToSetupDataList(testModelItemId);
      // Add route from linked test connection to assigned measure connection
      if(!tmi.addRoute(testModelItemId, otherData.testLinkId, otherData.measureConnexionId, otherData.setupDataList, false)){
        rollbackTransaction();
        pvLastError = tmi.lastError();
        return false;
      }
      if(helper.queueRelatedConnectionToSolved()){
        solvedTestConnectionIdList.append(otherData.testConnectionId);
      }
    }
    solvedTestConnectionIdList.append(data.testConnectionId);
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

/**
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
*/

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
