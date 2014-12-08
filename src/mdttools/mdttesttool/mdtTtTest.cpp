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
#include "mdtTtTest.h"
#include "mdtTtTestModel.h"
#include "mdtSqlRecord.h"
#include "mdtTtTestNodeSetupData.h"
#include "mdtTtTestModelItemRouteData.h"
#include "mdtSqlRelationInfo.h"
#include "mdtClPathGraph.h"
#include "mdtTtTestNode.h"
#include <QSqlTableModel>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QVector>
#include <QDate>
#include <QDateTime>

#include <QDebug>

mdtTtTest::mdtTtTest(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
  pvDatabase = db;
  pvCurrentTestItemRow = -1;
}

/**
void mdtTtTest::setTestData(const mdtTtTestData& data)
{
  bool newTestId;

  newTestId = (data.value("Id_PK") != pvTestData.value("Id_PK"));
  pvTestData = data;
  pvTestDataAreSaved = false;
  if(newTestId){
    ///updateCurrentTestRow(data.value("Id_PK"));
    resetTestItemTableModels();
  }
  ///emit testDataChanged(pvTestData);
}
*/

/**
void mdtTtTest::setTestDataValue(const QString& fieldName, const QVariant& value)
{
  bool newTestId = false;

  if(fieldName == "Id_PK"){
    newTestId = (value != pvTestData.value("Id_PK"));
  }
  pvTestData.setValue(fieldName, value);
  pvTestDataAreSaved = false;
  if(newTestId){
    resetTestItemTableModels();
  }
  ///emit testDataChanged(pvTestData);
}
*/

/**
QSqlRecord mdtTtTest::testData() const
{
  return pvTestViewTableModel->record(pvCurrentTestRow);
}
*/

QVariant mdtTtTest::createTest(const QVariant& testModelId)
{
  QSqlQuery query(database());
  QVariant testId;
  mdtSqlRecord testData;

  // Setup test data
  if(!testData.addAllFields("Test_tbl", database())){
    pvLastError = testData.lastError();
    return QVariant();
  }
  testData.setValue("TestModel_Id_FK", testModelId);
  testData.setValue("Date", QDate::currentDate());
  // Create test
  if(!beginTransaction()){
    return QVariant();
  }
  if(!addRecord(testData, "Test_tbl", query)){
    rollbackTransaction();
    return QVariant();
  }
  testId = query.lastInsertId();
  if(!addTestItems(testId, testModelId)){
    rollbackTransaction();
    return QVariant();
  }
  if(!commitTransaction()){
    return QVariant();
  }

  return testId;
}

bool mdtTtTest::saveTest(const mdtSqlRecord& testRecord, const QList< mdtSqlRecord >& testItemRecords)
{
  int i;

  if(!beginTransaction()){
    return false;
  }
  // Save Test_tbl part
  if(!updateRecord("Test_tbl", testRecord, "Id_PK", testRecord.value("Id_PK"))){
    rollbackTransaction();
    return false;
  }
  // Save test items
  for(i = 0; i < testItemRecords.size(); ++i){
    if(!updateRecord("TestItem_tbl", testItemRecords.at(i), "Id_PK", testItemRecords.at(i).value("Id_PK"))){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTest::removeTest(const QVariant& testId)
{
  if(!beginTransaction()){
    return false;
  }
  if(!removeTestItems(testId)){
    rollbackTransaction();
    return false;
  }
  if(!removeData("Test_tbl", "Id_PK", testId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

// void mdtTtTest::setCurrentTestItemData(const QString & fieldName, const QVariant & data)
// {
//   /**
//   QVariant testItemId;
//   int testItemViewCol;
//   int testItemTableRow, testItemTableCol;
//   QModelIndex index;
//   */
// 
//   /**
//   if(pvTestItemViewTableModel->rowCount() < 1){
//     return;
//   }
//   */
//   Q_ASSERT(pvCurrentTestItemRow >= 0);
//   pvTestViewController->setData(pvCurrentTestItemRow, fieldName, data, false);
//   /**
//   // Get column index of fieldName
//   testItemViewCol = pvTestItemViewTableModel->fieldIndex(fieldName);
//   Q_ASSERT(testItemViewCol >= 0);
//   testItemTableCol = pvTestItemTableModel->fieldIndex(fieldName);
//   Q_ASSERT(testItemTableCol >= 0);
//   // Get corresponding row in TestItem_view
//   testItemId = pvTestItemViewTableModel->record(pvCurrentTestItemRow).value("Id_PK");
//   Q_ASSERT(!testItemId.isNull());
//   testItemTableRow = getTestItemTableModelIndexRow(testItemId);
//   Q_ASSERT(testItemTableRow >= 0);
//   // Set value
//   index = pvTestItemViewTableModel->index(pvCurrentTestItemRow, testItemViewCol);
//   pvTestItemViewTableModel->setData(index, data);
//   index = pvTestItemTableModel->index(testItemTableRow, testItemTableCol);
//   pvTestItemTableModel->setData(index, data);
//   */
// }

// QVariant mdtTtTest::currentTestItemData(const QString & fieldName) const
// {
//   /**
//   int testItemViewCol;
//   QModelIndex index;
// 
//   if(pvTestItemViewTableModel->rowCount() < 1){
//     return QVariant();
//   }
//   */
//   Q_ASSERT(pvCurrentTestItemRow >= 0);
//   return pvTestItemViewController->data(pvCurrentTestItemRow, fieldName);
//   /**
//   testItemViewCol = pvTestItemViewTableModel->fieldIndex(fieldName);
//   Q_ASSERT(testItemViewCol >= 0);
//   index = pvTestItemViewTableModel->index(pvCurrentTestItemRow, testItemViewCol);
// 
//   return pvTestItemViewTableModel->data(index);
//   */
// }

// void mdtTtTest::setMeasuredValue(const mdtValue & value, const QVariant & instrumentRangeMin, const QVariant & instrumentRangeMax)
// {
//   double resultValue;
//   QVariant expectedValue;
//   QVariant val;
//   double limitValueMin, limitValueMax;
//   double failValueMin, failValueMax;
//   QString result;
// 
//   // Set date
//   setCurrentTestItemData("Date", QDateTime::currentDateTime());
//   // Check about validity
//   if(!value.isValid()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Measure/instrument error");
//     return;
//   }
//   // Get expected value
//   expectedValue = currentTestItemData("ExpectedValue");
//   if(expectedValue.isNull()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Expected value not set");
//     return;
//   }
//   // Get limit values
//   val = currentTestItemData("LimitValueMin");
//   if(val.isNull()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Limit value min not set");
//     return;
//   }
//   limitValueMin = val.toDouble();
//   val = currentTestItemData("LimitValueMax");
//   if(val.isNull()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Limit value max not set");
//     return;
//   }
//   limitValueMax = val.toDouble();
//   // Get fail values
//   val = currentTestItemData("FailValueMin");
//   if(val.isNull()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Fail value min not set");
//     return;
//   }
//   failValueMin = val.toDouble();
//   val = currentTestItemData("FailValueMax");
//   if(val.isNull()){
//     setCurrentTestItemData("Result", "Fail");
//     setCurrentTestItemData("Remark", "Fail value min not set");
//     return;
//   }
//   failValueMax = val.toDouble();
//   // Check about -OL and +OL
//   if(value.isMinusOl()){
//     resultValue = -std::numeric_limits<double>::infinity();
//   }else if(value.isPlusOl()){
//     resultValue = std::numeric_limits<double>::infinity();
//   }else{
//     resultValue = value.valueDouble();
//   }
//   // Check and set result
//   if(isInOkRange(resultValue, limitValueMin, limitValueMax)){
//     result = "Ok";
//   }else if(isInLimitRange(resultValue, limitValueMin, limitValueMax, failValueMin, failValueMax)){
//     result = "Limit";
//   }else{
//     result = "Fail";
//   }
//   // Set results
//   ///setCurrentTestItemData("Date", QDateTime::currentDateTime());
//   setCurrentTestItemData("MeasuredValue", value.valueDouble());
//   setCurrentTestItemData("InstrumentRangeMin", instrumentRangeMin);
//   setCurrentTestItemData("InstrumentRangeMax", instrumentRangeMax);
//   setCurrentTestItemData("ResultValue", resultValue);
//   setCurrentTestItemData("Result", result);
// }

bool mdtTtTest::isInRange(double x, double min, double max)
{
  return ( ( (x > min)||(qAbs(x - min) < std::numeric_limits<double>::min()) ) &&
           ( (x < max)||(qAbs(x - max) < std::numeric_limits<double>::min()) ) );
}

bool mdtTtTest::isInOkRange(double x, double limitMin, double limitMax)
{
  return ((x > limitMin)&&(x < limitMax));
}

bool mdtTtTest::isInLimitRange(double x, double limitMin, double limitMax, double failMin, double failMax)
{
  return ( ( (x > failMin) && ((x < limitMin)||(qAbs(x - limitMin) < std::numeric_limits<double>::min())) )
          || ( ((x > limitMax)||(qAbs(x - limitMax) < std::numeric_limits<double>::min())) && (x < failMax) ) );
}

// bool mdtTtTest::hasMoreTestItem() const
// {
//   ///Q_ASSERT(pvTestItemViewTableModel);
//   Q_ASSERT(pvTestItemViewController);
// 
//   /**
//   if(pvCurrentTestItemRow < (pvTestItemViewTableModel->rowCount() - 1)){
//     return true;
//   }
//   if(pvTestItemViewTableModel->canFetchMore()){
//     pvTestItemViewTableModel->fetchMore();
//     return true;
//   }
//   */
//   if(pvCurrentTestItemRow < (pvTestItemViewController->rowCount(true) - 1)){
//     return true;
//   }
//   return false;
// }

void mdtTtTest::resetTestItemCursor()
{
  pvCurrentTestItemRow = -1;
}

// QVariant mdtTtTest::nextTestItem()
// {
//   /**
//   Q_ASSERT(pvTestItemViewTableModel);
// 
//   QModelIndex index;
//   int col;
// 
//   col = pvTestItemViewTableModel->fieldIndex("Id_PK");
//   Q_ASSERT(col >= 0);
//   */
//   
//   ++pvCurrentTestItemRow;
// 
//   return pvTestItemViewController->data(pvCurrentTestItemRow, "Id_PK");
//   
//   /**
//   index = pvTestItemViewTableModel->index(pvCurrentTestItemRow, col);
//   
//   /// \todo Provisoire !
//   col = pvTestItemViewTableModel->fieldIndex("SequenceNumber");
//   QModelIndex index2;
//   index2 = pvTestItemViewTableModel->index(pvCurrentTestItemRow, col);
//   qDebug() << "mdtTtTest - current seq #: " << pvTestItemViewTableModel->data(index2);
//   
//   
// 
//   return pvTestItemViewTableModel->data(index);
//   */
// }

// mdtTtTestItemNodeSetupData mdtTtTest::getSetupData(const QVariant & testModelItemId, bool & ok)
// {
//   QString sql;
//   QList<QSqlRecord> dataList;
//   mdtTtTestItemNodeSetupData tiSetupData;
//   mdtTtTestNodeUnitSetupData tnuSetupData;
//   QVector<int> nodeIdList;
//   int i, k;
//   int id;
// 
//   // Get data from db
//   ///sql = "SELECT * FROM TestItemNodeUnitSetup_view WHERE Id_PK = " + testItemId.toString();
//   sql = "SELECT * FROM TestItemNodeUnitSetup_view WHERE TestModelItem_Id_FK = " + testModelItemId.toString();
//   ///dataList = getData(sql, &ok);
//   dataList = getDataList<QSqlRecord>(sql, ok);
//   if(!ok){
//     return tiSetupData;
//   }
//   // Build a list of nodes
//   for(i = 0; i < dataList.size(); ++i){
//     Q_ASSERT(!dataList.at(i).value("TestNode_Id_FK").isNull());
//     id = dataList.at(i).value("TestNode_Id_FK").toInt();
//     if(!nodeIdList.contains(id)){
//       nodeIdList.append(id);
//     }
//   }
//   ///qDebug() << "Nodes: " << nodeIdList;
//   // For each test node, build test node setup data
//   for(i = 0; i < nodeIdList.size(); ++i){
//     qDebug() << "Buil setup data for node " << nodeIdList.at(i) << " ...";
//     mdtTtTestNodeSetupData tnSetupData;
//     // Add route data
//     /**
//     if(!addRouteDataToTestNodeSetupData(tnSetupData, testModelItemId, nodeIdList.at(i))){
//       ok = false;
//       return tiSetupData;
//     }
//     */
//     // Add other setup data
//     /**
//     if(!addNodeUnitSetupDataToTestNodeSetupData(tnSetupData, testModelItemId, nodeIdList.at(i))){
//       ok = false;
//       return tiSetupData;
//     }
//     */
//     // Set node identification
//     /// \todo Can find better way...
//     for(k = 0; k < dataList.size(); ++k){
//       id = dataList.at(k).value("TestNode_Id_FK").toInt();
//       if(nodeIdList.at(i) == id){
//         ///qDebug() << "Updating node identification for node " << id << ": " << dataList.at(k).value("NodeIdentification");
//         if(tnSetupData.nodeIdentification().isEmpty()){
//           tnSetupData.setNodeIdentification(dataList.at(k).value("NodeIdentification").toString());
//         }
//       }
//     }
// 
//     /**
//     // Add all unit setup data that are part of current node
//     for(k = 0; k < dataList.size(); ++k){
//       id = dataList.at(k).value("TestNode_Id_FK").toInt();
//       if(nodeIdList.at(i) == id){
//         tnuSetupData = dataList.at(k);
//         if(tnSetupData.nodeIdentification().isEmpty()){
//           tnSetupData.setNodeIdentification(tnuSetupData.value("NodeIdentification").toString());
//         }
//         tnSetupData.addUnitSetup(tnuSetupData);
//       }
//     }
//     */
//     // Add node setup data to test item setup data
//     tiSetupData.addNodeSetupData(tnSetupData);
//     ///qDebug() << "Data built for test node " << tnSetupData.nodeIdentification();
//   }
// 
//   return tiSetupData;
// }

mdtTtTestItemNodeSetupData mdtTtTest::getSetupData(const QVariant& testModelItemId, bool& ok)
{
  QList<QVariant> stepNumberList;
  QVariant stepNumber;
  QList<mdtTtTestNodeSetupData> nodeSetupDataList;
  mdtTtTestItemNodeSetupData tiSetupData;
  int i;

  // Get setps for given test model item
  stepNumberList = getStepNumberList(testModelItemId, ok);
  if(!ok){
    return tiSetupData;
  }
  // For each step, add related test node setups
  for(i = 0; i < stepNumberList.size(); ++i){
    stepNumber = stepNumberList.at(i);
    nodeSetupDataList = getTestNodeSetupDataList(testModelItemId, stepNumber, ok);
    if(!ok){
      return tiSetupData;
    }
    tiSetupData.addNodeSetupData(stepNumber.toInt(), nodeSetupDataList);
  }

  return tiSetupData;
}

mdtTtTestNodeSetupData mdtTtTest::getTestNodeSetupData(const QVariant& testModelItemId, const QString & nodeIdentification, bool& ok)
{
  QString sql;
  QList<QVariant> idList;
  mdtTtTestNodeSetupData nodeSetupData;

  // Get node setup data from database
  sql = "SELECT VehicleType_Id_FK_PK FROM TestNode_tbl";
  sql += " WHERE NodeIdentification = " + nodeIdentification;
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return nodeSetupData;
  }
  if(idList.isEmpty()){
    QString msg = tr("Could not find a node with identification") + " '" + nodeIdentification + "' ";
    msg += tr("for test item ID") + " " + testModelItemId.toString();
    pvLastError.setError(msg , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    ok = false;
    return nodeSetupData;
  }
  Q_ASSERT(idList.size() == 1);

  return getTestNodeSetupDataByTnPk(testModelItemId, QVariant(), idList.at(0), ok);
}

QList<mdtClLinkData> mdtTtTest::getLinkDataListForRoute(const QVariant& connectionIdA, const QVariant& connectionIdB,
                                                        const QList< mdtTtTestNodeUnitSetupData >& testNodeUnitSetupDataList,
                                                        mdtClPathGraph& graph, bool& ok)
{
  mdtTtTestNode tn(0, database());
  QList<mdtClLinkData> linkDataList;

  linkDataList = tn.getLinkDataListForPath(connectionIdA, connectionIdB, testNodeUnitSetupDataList, graph, ok);
  if(!ok){
    pvLastError = tn.lastError();
  }

  return linkDataList;
}

QList<mdtClLinkData> mdtTtTest::getLinkDataListForRoute(const mdtTtTestModelItemRouteData& routeData, mdtClPathGraph& graph, bool& ok)
{
  return getLinkDataListForRoute(routeData.testConnectionId(), routeData.measureConnectionId(), routeData.setupDataList(), graph, ok);
}

QList<mdtClLinkData> mdtTtTest::getLinkDataListForRoute(const QVariant& testNodeId, 
                                                        const QString & schemaPositionA, const QString & connectorA, const QString & contactA,
                                                        const QString & schemaPositionB, const QString & connectorB, const QString & contactB,
                                                        const QStringList & relaysToEnable,
                                                        mdtClPathGraph & graph, bool & ok)
{
  QString sql;
  QList<QSqlRecord> dataList;
  QVariant connectionIdA, connectionIdB;
  QList<mdtTtTestNodeUnitSetupData> testNodeUnitSetupDataList;

  // Get connections ID for A
  connectionIdA = getConnectionId(testNodeId, schemaPositionA, connectorA, contactA, ok);
  if(!ok){
    return QList<mdtClLinkData>();
  }
  Q_ASSERT(!connectionIdA.isNull());
  // Get connections ID for B
  connectionIdB = getConnectionId(testNodeId, schemaPositionB, connectorB, contactB, ok);
  if(!ok){
    return QList<mdtClLinkData>();
  }
  Q_ASSERT(!connectionIdB.isNull());
  // Build test node unit setup data for given unit schema positions
  testNodeUnitSetupDataList = buildUnitSetupDataListBySchemaPostionList(testNodeId, relaysToEnable, ok);
  if(!ok){
    return QList<mdtClLinkData>();
  }

  return getLinkDataListForRoute(connectionIdA, connectionIdB, testNodeUnitSetupDataList, graph, ok);
}

double mdtTtTest::linkPathResistance(const QList< mdtClLinkData >& linkDataList) const
{
  int i;
  double r = 0.0;

  for(i = 0; i < linkDataList.size(); ++i){
    mdtClLinkData linkData = linkDataList.at(i);
    qDebug() << "Link R: " << linkData.value("Value");
    qDebug() << "Start CNX R: " << linkData.startConnectionData().value("Resistance").toDouble();
    qDebug() << "End CNX R: " << linkData.endConnectionData().value("Resistance").toDouble();
    
    r += linkData.value("Value").toDouble();
    r += linkData.startConnectionData().value("Resistance").toDouble() / 2.0;
    r += linkData.endConnectionData().value("Resistance").toDouble() / 2.0;
  }
  qDebug() << "r: " << r;

  return r;
}

QVariant mdtTtTest::getConnectionId(const QVariant & testNodeId, const QString & schemaPosition, const QString & connector, const QString & contact, bool & ok)
{
  QString sql;
  QList<QVariant> dataList;
  QVariant connectionId;

  sql = "SELECT UCNX.Id_PK FROM UnitConnection_tbl UCNX";
  sql += " LEFT JOIN UnitConnector_tbl UCNR ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK";
  sql += " JOIN Unit_tbl U ON U.Id_PK = UCNX.Unit_Id_FK";
  sql += " JOIN VehicleType_Unit_tbl VTU ON VTU.Unit_Id_FK = UCNX.Unit_Id_FK";
  sql += " WHERE U.SchemaPosition = '" + schemaPosition + "'";
  if(!connector.isEmpty()){
    sql += " AND UCNR.Name = '" + connector + "'";
  }
  sql += " AND UCNX.UnitContactName = '" + contact + "'";
  sql += " AND VTU.VehicleType_Id_FK = " + testNodeId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Could not find ID of requested connection.") + sql, mdtError::Error);
    pvLastError.setSystemError(0, tr("SQL query: ") + sql);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    ok = false;
    return QVariant();
  }
  if(dataList.size() > 1){
    pvLastError.setError(tr("Criteria for connection returned more than 1 connection.") + sql, mdtError::Error);
    pvLastError.setSystemError(0, tr("SQL query: ") + sql);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    ok = false;
    return QVariant();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

// bool mdtTtTest::addRouteDataToTestNodeSetupData(mdtTtTestNodeSetupData & tnSetupData, const QVariant& testModelItemId, const QVariant& testNodeId)
// {
//   QString sql;
//   QList<QSqlRecord> routeDataList;
//   bool ok;
//   int i;
// 
//   ///qDebug() << "-> Building routes for model item " << testModelItemId << " and node " << testNodeId << " ...";
//   
//   // Get route data
//   sql = "SELECT * FROM TestModelItemRoute_view";
//   sql += " WHERE TestModelItem_Id_FK = " + testModelItemId.toString();
//   sql += " AND TestNode_Id_FK = " + testNodeId.toString();
//   routeDataList = getDataList<QSqlRecord>(sql, ok);
//   if(!ok){
//     return false;
//   }
//   
//   ///qDebug() << "-> data list: " << routeDataList;
//   
//   // Add node unit setups for each route
//   for(i = 0; i < routeDataList.size(); ++i){
//     mdtTtTestModelItemRouteData routeData(routeDataList.at(i));
//     if(!addNodeUnitSetupDataToRouteData(routeData)){
//       return false;
//     }
//     tnSetupData.addRouteData(routeData);
//   }
// 
//   return true;
// }


/**
QVariant mdtTtTest::valueToDouble(const mdtValue & value)
{
  if(!value.isValid()){
    return QVariant();
  }
  if(value.isMinusOl()){
    return -1e300;
  }else if(value.isPlusOl()){
    return 1e300;
  }else{
    return value.valueDouble();
  }
}

mdtValue mdtTtTest::doubleToValue(const QVariant & dblVal)
{
  bool ok;
  double x;
  mdtValue val;

  x = dblVal.toDouble(&ok);
  if(!ok){
    return val;
  }
  if(x < -1.999999999999e299){
    val.setValue(x, true, false);
  }else if(x > 1.999999999999e299){
    val.setValue(x, false, true);
  }else{
    val.setValue(x, false, false);
  }

  return val;
}
*/

/**
bool mdtTtTest::setMeasuredValue(const QVariant & testItemId, const mdtValue & value)
{
  QModelIndex index;
  QVariant data;

  if(!testItemSqlModelOk()){
    return false;
  }
  index = indexOfTestItem(testItemId, pvColIdxOfMeasuredValue);
  if(!index.isValid()){
    return false;
  }
  /// \todo Map +-OL to a value
  data = value.valueDouble();
  // Store value
  return setTestItemSqlModelData(index, data);
}
*/

QList<QVariant> mdtTtTest::getStepNumberList(const QVariant& testModelItemId, bool & ok)
{
  QString sql;

  sql = "SELECT DISTINCT StepNumber FROM TestNodeUnitSetup_tbl";
  sql += " WHERE TestModelItem_Id_FK = " + testModelItemId.toString();

  return getDataList<QVariant>(sql, ok);
}

mdtTtTestNodeSetupData mdtTtTest::getTestNodeSetupDataByTnPk(const QVariant& testModelItemId, const QVariant & stepNumber, const QVariant& testNodeId, bool& ok)
{
  QString sql;
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  mdtTtTestNodeSetupData nodeSetupData;

  // Get node setup data from database
  sql = "SELECT * FROM TestNode_tbl";
  sql += " WHERE VehicleType_Id_FK_PK = " + testNodeId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return nodeSetupData;
  }
  if(dataList.isEmpty()){
    QString msg = tr("Could not find a node with ID (PK)") + " '" + testNodeId.toString() + "' ";
    msg += tr("for test item ID") + " " + testModelItemId.toString();
    pvLastError.setError(msg , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    ok = false;
    return nodeSetupData;
  }
  Q_ASSERT(dataList.size() == 1);
  // Set node setup data values
  data = dataList.at(0);
  nodeSetupData.setId(data.value("VehicleType_Id_FK_PK"));
  nodeSetupData.setDeviceIdentification(data.value("DeviceIdentification"));
  nodeSetupData.setNodeIdentification(data.value("NodeIdentification").toString());
  // Add route data
  if(!addRouteDataToTestNodeSetupData(nodeSetupData, testModelItemId, stepNumber, nodeSetupData.id())){
    ok = false;
    return nodeSetupData;
  }
  // Add other setup data
  if(!addNodeUnitSetupDataToTestNodeSetupData(nodeSetupData, testModelItemId, stepNumber, nodeSetupData.id())){
    ok = false;
    return nodeSetupData;
  }

  return nodeSetupData;
}

QList< mdtTtTestNodeUnitSetupData > mdtTtTest::buildUnitSetupDataListBySchemaPostionList(const QVariant& testNodeId, const QStringList& schemaPositionList, bool& ok)
{
  mdtTtTestNodeUnitSetupData setupData;
  QList<mdtTtTestNodeUnitSetupData> setupDataList;
  QString sql;
  QList<QVariant> unitIdList;
  int i;
  int lastIndex;

  // Setup setup data
  if(!setupData.setup(database())){
    ok = false;
    pvLastError = setupData.lastError();
    return setupDataList;
  }
  // Get ID of each test node unit schema postion
  sql = "SELECT U.Id_PK FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND U.SchemaPosition IN(";
  lastIndex = schemaPositionList.size() - 1;
  for(i = 0; i < lastIndex; ++i){
    sql += "'" + schemaPositionList.at(i) + "',";
  }
  sql += "'" + schemaPositionList.at(lastIndex) + "')";
  unitIdList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return setupDataList;
  }
  // Build unit setup data list
  for(i = 0; i < unitIdList.size(); ++i){
    setupData.clearValues();
    setupData.setValue("TestNodeUnit_Id_FK", unitIdList.at(i));
    setupData.setValue("State", true);
    setupDataList.append(setupData);
  }
  ok = true;

  return setupDataList;
}

QList<mdtTtTestNodeSetupData> mdtTtTest::getTestNodeSetupDataList(const QVariant& testModelItemId, const QVariant& stepNumber, bool& ok)
{
  QString sql;
  QList<QVariant> idList;
  QList<mdtTtTestNodeSetupData> nodeSetupDataList;
  int i;

  sql = "SELECT DISTINCT TN.VehicleType_Id_FK_PK FROM TestNode_tbl TN";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.TestNode_Id_FK = TN.VehicleType_Id_FK_PK";
  sql += " JOIN TestNodeUnitSetup_tbl TNUS ON TNUS.TestNodeUnit_Id_FK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNUS.TestModelItem_Id_FK = " + testModelItemId.toString();
  if(!stepNumber.isNull()){
    sql += " AND TNUS.StepNumber = " + stepNumber.toString();
  }
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return nodeSetupDataList;
  }
  for(i = 0; i < idList.size(); ++i){
    mdtTtTestNodeSetupData nodeSetupData = getTestNodeSetupDataByTnPk(testModelItemId, stepNumber, idList.at(i), ok);
    if(!ok){
      return nodeSetupDataList;
    }
    // Add node setup data to list
    nodeSetupDataList.append(nodeSetupData);
  }

  return nodeSetupDataList;
}

bool mdtTtTest::addRouteDataToTestNodeSetupData(mdtTtTestNodeSetupData & tnSetupData, const QVariant& testModelItemId, const QVariant& stepNumber, const QVariant& testNodeId)
{
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  sql = "SELECT DISTINCT TMIR.*";
  sql += " FROM TestModelItemRoute_tbl TMIR";
  sql += " JOIN TestNodeUnitSetup_tbl TNUS ON TNUS.TestModelItemRoute_Id_FK = TMIR.Id_PK";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK";
  sql += " WHERE TMIR.TestModelItem_Id_FK = " + testModelItemId.toString();
  if(!stepNumber.isNull()){
    sql += " AND TNUS.StepNumber = " + stepNumber.toString();
  }
  sql += " AND TNU.TestNode_Id_FK = " + testNodeId.toString();
  // Get data and build route data for each record.
  //  Note: we not include test connectiona and DUT connection
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtTtTestModelItemRouteData routeData(dataList.at(i));
    if(!addNodeUnitSetupDataToRouteData(routeData)){
      return false;
    }
    tnSetupData.addRouteData(routeData);
  }

  return true;
}

bool mdtTtTest::addNodeUnitSetupDataToRouteData(mdtTtTestModelItemRouteData & routeData)
{
  Q_ASSERT(!routeData.id().isNull());

  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  sql = "SELECT TNUS.* , TNU.IoPosition, TNU.CalibrationOffset, TNU.Type_Code_FK, U.SchemaPosition";
  sql += " FROM TestNodeUnitSetup_tbl TNUS";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK";
  sql += " JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TestModelItemRoute_Id_FK = " + routeData.id().toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtTtTestNodeUnitSetupData tnuSetupData(dataList.at(i));
    routeData.addSetupData(tnuSetupData);
  }

  return true;
}

bool mdtTtTest::addNodeUnitSetupDataToTestNodeSetupData(mdtTtTestNodeSetupData & tnSetupData, const QVariant& testModelItemId, const QVariant & stepNumber, const QVariant& testNodeId)
{
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  sql = "SELECT TNUS.* , TNU.IoPosition, TNU.CalibrationOffset, TNU.Type_Code_FK, U.SchemaPosition";
  sql += " FROM TestNodeUnitSetup_tbl TNUS";
  sql += " JOIN TestNodeUnit_tbl TNU ON TNU.Unit_Id_FK_PK = TNUS.TestNodeUnit_Id_FK";
  sql += " JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNUS.TestModelItemRoute_Id_FK IS NULL";
  sql += " AND TNUS.TestModelItem_Id_FK = " + testModelItemId.toString();
  if(!stepNumber.isNull()){
    sql += " AND TNUS.StepNumber = " + stepNumber.toString();
  }
  sql += " AND TNU.TestNode_Id_FK = " + testNodeId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtTtTestNodeUnitSetupData tnuSetupData(dataList.at(i));
    tnSetupData.addUnitSetup(tnuSetupData);
  }

  return true;
}

/**
void mdtTtTest::setCurrentTestIndexRow(int row)
{
  pvTestTableRelation->setParentCurrentIndex(row);
  pvTestItemTableRelation->setParentCurrentIndex(row);
  pvTestItemViewRelation->setParentCurrentIndex(row);
  pvCurrentTestRow = row;
  resetTestItemCursor();
  emit testDataChanged(testData());
}
*/

bool mdtTtTest::addTestItems(const QVariant & testId, const QVariant & testModelId)
{
  QString sql;
  QList<QVariant> modelItemsDataList;
  mdtSqlRecord testItemData;
  bool ok;
  int i;

  // Get current test ID
  if(testId.isNull()){
    pvLastError.setError(tr("Could not add test items because current test id (Id_PK) is Null."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Get items for given test model
  sql = "SELECT Id_PK FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testModelId.toString();
  modelItemsDataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  // Setup test item data
  if(!testItemData.addAllFields("TestItem_tbl", database())){
    pvLastError = testItemData.lastError();
    return false;
  }
  // Add a new test item for each model item
  for(i = 0; i < modelItemsDataList.size(); ++i){
    testItemData.clearValues();
    testItemData.setValue("Test_Id_FK", testId);
    testItemData.setValue("TestModelItem_Id_FK", modelItemsDataList.at(i));
    if(!addRecord(testItemData, "TestItem_tbl")){
      return false;
    }
  }

  return true;
}

bool mdtTtTest::removeTestItems(const QVariant & testId)
{
  if(testId.isNull()){
    pvLastError.setError(tr("Could not remove test items because test id (Id_PK) is Null."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Remove test items
  if(!removeData("TestItem_tbl", "Test_Id_FK", testId)){
    return false;
  }

  return true;
}

/**
int mdtTtTest::getTestItemTableModelIndexRow(const QVariant& testItemId)
{
  int row, col;
  QModelIndex index;

  if(testItemId.isNull()){
    return -1;
  }
  if(pvTestItemTableModel->rowCount() < 1){
    return -1;
  }
  // Find column of Id_PK
  col = pvTestItemTableModel->fieldIndex("Id_PK");
  if(col < 0){
    mdtError e(tr("Could not find field Id_PK in TestItem_tbl"), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTtTest");
    e.commit();
    return -1;
  }
  // Search row
  row = 0;
  while(true){
    index = pvTestItemTableModel->index(row, col);
    if(pvTestItemTableModel->data(index) == testItemId){
      return row;
    }
    if(row == (pvTestItemTableModel->rowCount()-1)){
      if(!pvTestItemTableModel->canFetchMore()){
        return -1;
      }
      pvTestItemTableModel->fetchMore();
    }
    ++row;
  }

  return -1;
}
*/
