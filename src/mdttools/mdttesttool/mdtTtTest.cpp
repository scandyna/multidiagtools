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
#include <QSqlTableModel>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QVector>

#include <QDebug>

mdtTtTest::mdtTtTest(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db) , pvTestItemTableModel(new QSqlTableModel(0, db))
{
  pvCurrentTestItemRow = -1;
  
  ///pvTestItemTableModel = 0;
  pvColIdxOfTestItemId = -1;
  pvColIdxOfExpectedValue = -1;
  pvColIdxOfMeasuredValue = -1;
  pvColIdxOfResult = -1;
}

bool mdtTtTest::setupTestItemModel()
{
  Q_ASSERT(pvTestItemTableModel);

  QSqlError sqlError;

  pvTestItemTableModel->setTable("TestItem_view");
  pvTestItemTableModel->setFilter("Test_Id_FK = -1");
  pvTestItemTableModel->setSort(pvTestItemTableModel->fieldIndex("SequenceNumber"), Qt::AscendingOrder);
  if(!pvTestItemTableModel->select()){
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestItem_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  pvCurrentTestItemRow = -1;

  return true;
}

mdtTtTestData mdtTtTest::getTestData(const QVariant & testId, bool includeModelData, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QList<QSqlRecord> dataList;
  mdtTtTestData data;

  sql = "SELECT * FROM Test_tbl WHERE Id_PK = " + testId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Could not find data in 'Test_tbl' for Id_PK = '") + testId.toString() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    *ok = false;
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  if(includeModelData){
    mdtTtTestModel tm(0, database());
    data.setModelData(tm.getTestModelData(data.value("TestModel_Id_FK"), ok));
    if(!*ok){
      data.clearValues();
      return data;
    }
  }

  return data;
}

QVariant mdtTtTest::addTest(const mdtTtTestData & data)
{
  QVariant testId;
  QSqlQuery query(database());

  if(!beginTransaction()){
    return testId;
  }
  if(!addRecord(data, "Test_tbl", query)){
    rollbackTransaction();
    return testId;
  }
  testId = query.lastInsertId();
  if(!addTestItems(testId, data.value("TestModel_Id_FK"))){
    rollbackTransaction();
    return QVariant();
  }
  if(!commitTransaction()){
    return QVariant();
  }

  return testId;
}

bool mdtTtTest::updateTest(const QVariant & testId, const mdtTtTestData & data)
{
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  // Get currently stored test model ID
  sql = "SELECT TestModel_Id_FK FROM Test_tbl WHERE Id_PK = " + testId.toString();
  dataList = getData(sql, &ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(dataList.size() == 1);
  if(!beginTransaction()){
    return false;
  }
  // If test model has changed, we have to update items
  if(dataList.at(0).value(0) != data.value("TestModel_Id_FK")){
    if(!removeTestItems(testId)){
      rollbackTransaction();
      return false;
    }
    if(!addTestItems(testId, data.value("TestModel_Id_FK"))){
      rollbackTransaction();
      return false;
    }
  }
  if(!updateRecord("Test_tbl", data, "Id_PK", testId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTest::hasMoreTestItem() const
{
  Q_ASSERT(pvTestItemTableModel);

  return (pvCurrentTestItemRow < pvTestItemTableModel->rowCount());
}

void mdtTtTest::resetTestItemCursor()
{
  pvCurrentTestItemRow = 0;
}

QVariant mdtTtTest::nextTestItem()
{
  Q_ASSERT(pvTestItemTableModel);

  QModelIndex index;
  int col;

  col = pvTestItemTableModel->fieldIndex("Id_PK");
  Q_ASSERT(col >= 0);
  index = pvTestItemTableModel->index(pvCurrentTestItemRow, col);
  
  /// \todo Provisoire !
  col = pvTestItemTableModel->fieldIndex("SequenceNumber");
  QModelIndex index2;
  index2 = pvTestItemTableModel->index(pvCurrentTestItemRow, col);
  qDebug() << "mdtTtTest - current seq #: " << pvTestItemTableModel->data(index2);
  
  ++pvCurrentTestItemRow;

  return pvTestItemTableModel->data(index);
}

mdtTtTestItemNodeSetupData mdtTtTest::getSetupData(const QVariant & testItemId, bool & ok)
{
  QString sql;
  QList<QSqlRecord> dataList;
  mdtTtTestItemNodeSetupData tiSetupData;
  mdtTtTestNodeUnitSetupData tnuSetupData;
  QVector<int> nodeIdList;
  int i, k;
  int id;

  // Get data from db
  sql = "SELECT * FROM TestItemNodeUnitSetup_view WHERE Id_PK = " + testItemId.toString();
  dataList = getData(sql, &ok);
  if(!ok){
    return tiSetupData;
  }
  // Build a list of nodes
  for(i = 0; i < dataList.size(); ++i){
    Q_ASSERT(!dataList.at(i).value("TestNode_Id_FK").isNull());
    id = dataList.at(i).value("TestNode_Id_FK").toInt();
    if(!nodeIdList.contains(id)){
      nodeIdList.append(id);
    }
  }
  qDebug() << "Nodes: " << nodeIdList;
  // For each test node, build test node setup data
  for(i = 0; i < nodeIdList.size(); ++i){
    mdtTtTestNodeSetupData tnSetupData;
    // Add all unit setup data that are part of current node
    for(k = 0; k < dataList.size(); ++k){
      id = dataList.at(k).value("TestNode_Id_FK").toInt();
      if(nodeIdList.at(i) == id){
        tnuSetupData = dataList.at(k);
        if(tnSetupData.nodeIdentification().isEmpty()){
          tnSetupData.setNodeIdentification(tnuSetupData.value("NodeIdentification").toString());
        }
        tnSetupData.addUnitSetup(tnuSetupData);
      }
    }
    // Add node setup data to test item setup data
    tiSetupData.addNodeSetupData(tnSetupData);
  }

  return tiSetupData;
}

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


/**
bool mdtTtTest::setTestItemSqlModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  QStringList missingFields;
  QString errorText;
  int i;

  pvTestItemTableModel = model;
  ///connect(pvTestItemTableModel, SIGNAL(destroyed(QObject*)), this, SLOT(onSqlModelDestroyed(QObject*)));
  // Get index of fields that we need
  pvColIdxOfTestItemId = pvTestItemTableModel->fieldIndex("Id_PK");
  if(pvColIdxOfTestItemId < 0){
    missingFields << "Id_PK";
  }
  pvColIdxOfExpectedValue = pvTestItemTableModel->fieldIndex("ExpectedValue");
  if(pvColIdxOfExpectedValue < 0){
    missingFields << "ExpectedValue";
  }
  pvColIdxOfMeasuredValue = pvTestItemTableModel->fieldIndex("MeasuredValue");
  if(pvColIdxOfMeasuredValue < 0){
    missingFields << "MeasuredValue";
  }
  pvColIdxOfResult = pvTestItemTableModel->fieldIndex("Result");
  if(pvColIdxOfResult < 0){
    missingFields << "Result";
  }
  // Check that all column indexes where found
  if(!missingFields.isEmpty()){
    errorText = QObject::tr("Some fields could not be found in");
    errorText += " '" + pvTestItemTableModel->tableName() + "'. " + QObject::tr("Missing fields") + " : ";
    for(i = 0; i < missingFields.size(); ++i){
      errorText += missingFields.at(i);
      if(i < (missingFields.size() - 1)){
        errorText += ", ";
      }
    }
    pvLastError.setError(errorText, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    pvTestItemTableModel = 0;
    return false;
  }

  return true;
}
*/

QList<QVariant> mdtTtTest::getTestItemIdListForTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> testItemIdList;

  sql = "SELECT Id_PK FROM TestItem_view WHERE Test_Id_FK = " + testId.toString();
  sql += " ORDER BY SequenceNumber ASC";
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test items", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return testItemIdList;
  }
  while(query.next()){
    testItemIdList.append(query.value(0));
  }

  return testItemIdList;
}

QList<QVariant> mdtTtTest::getHardwareNodeIdListForTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> nodeIdList;

  sql = "SELECT DISTINCT NodeId FROM TestItemNodeUnitSetup_view WHERE Test_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test nodeId list", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return nodeIdList;
  }
  while(query.next()){
    nodeIdList.append(query.value(0));
  }

  return nodeIdList;
}

QList<QVariant> mdtTtTest::getHardwareNodeIdListForTestItemId(const QVariant & testItemId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> nodeIdList;

  sql = "SELECT DISTINCT NodeId FROM TestItemNodeUnitSetup_view WHERE Id_PK = " + testItemId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test nodeId list", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return nodeIdList;
  }
  while(query.next()){
    nodeIdList.append(query.value(0));
  }

  return nodeIdList;
}

///QList<mdtTtTestNodeUnitSetupData> mdtTtTest::getNodeUnitSetupList(const QVariant & testItemId, const QVariant & hardwareNodeId)
QList<QSqlRecord> mdtTtTest::getNodeUnitSetupList(const QVariant & testItemId, const QVariant & hardwareNodeId)
{
  ///QList<mdtTtTestNodeUnitSetupData> dataList;
  QList<QSqlRecord> dataList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  /// \todo Test node ID PK missing !
  // Note: in this view, Id_PK is the ID of test item
  sql = "SELECT "\
        " Id_PK AS TestItemId,"\
        " TestModelItem_Id_FK,"\
        " SchemaPosition,"\
        " IoPosition,"\
        " State,"\
        " Value,"\
        " NodeId ";
  sql += " FROM TestItemNodeUnitSetup_view ";
  sql += " WHERE Id_PK = " + testItemId.toString() + " AND NodeId = " + hardwareNodeId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test node unit setup.", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return dataList;
  }
  while(query.next()){
    ///mdtTtTestNodeUnitSetupData data(database());
    ///qDebug() << "mdtTtTest: rec: " << query.record();
    ///mdtTtTestNodeUnitSetupData data = query.record();
    dataList.append(query.record());
    /// \todo TestItemNodeUnitSetup_tbl.Id_PK is missing in view
    ///data.setValue("??", ??);
    ///data.setValue("");
    ///data.setId(QVariant()); /// \todo Implement !
    /**
    data.setTestItemId(query.value(0));
    data.setTestModelItemId(query.value(1));
    data.setSchemaPosition(query.value(2));
    data.setState(query.value(3));
    data.setValue(query.value(4));
    data.setNodeId(query.value(5));
    dataList.append(data);
    */
  }

  return dataList;
}


bool mdtTtTest::setTestModel(const QVariant & testResultId, const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Remove previous result items
  if(!removeData("TestItem_tbl", "Test_Id_FK", testResultId)){
    return false;
  }
  // Prepare query for edition
  sql = "UPDATE Test_tbl SET "\
        " TestModel_Id_FK = :TestModel_Id_FK "\
        " WHERE Id_PK = " + testResultId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test result edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestModel_Id_FK", testId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test result edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add items
  if(!addItemsByTestId(testResultId, testId)){
    return false;
  }

  return true;
}

bool mdtTtTest::addItem(const QVariant & testResultId, const QVariant & testItemId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestItem_tbl (Test_Id_FK, TestModelItem_Id_FK) "\
        "VALUES (:Test_Id_FK, :TestModelItem_Id_FK)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test result item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Test_Id_FK", testResultId);
  query.bindValue(":TestModelItem_Id_FK", testItemId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test result item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTest::addItemsByTestId(const QVariant & testResultId, const QVariant & testId)
{
  mdtTtTestModel tm(this, database());
  QList<QVariant> testItemIdList;
  int i;

  // Get base test items ID list
  testItemIdList = tm.getListOfTestItemIdListByTestId(testId);
  if(testItemIdList.isEmpty()){
    pvLastError = tm.lastError();
    return false;
  }
  // Add test result items
  for(i = 0; i < testItemIdList.size(); ++i){
    if(!addItem(testResultId, testItemIdList.at(i))){
      return false;
    }
  }

  return true;
}

bool mdtTtTest::editItem(const QVariant & testItemId, const QString & fieldName, const QVariant & data)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE TestItem_tbl SET ";
  sql += fieldName + " = :data";
        " WHERE Id_PK = " + testItemId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test item edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":data", data);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

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

bool mdtTtTest::submitTestItemSqlModelData()
{
  int row;
  QModelIndex index;
  QVariant data;
  QVariant testItemId;

  if(!testItemSqlModelOk()){
    return false;
  }
  if(!beginTransaction()){
    return false;
  }
  // Check each row and interesting fields and store values of them that are dirty
  for(row = 0; row < pvTestItemTableModel->rowCount(); ++row){
    // Get test item ID
    testItemId = getTestItemSqlModelData(row, pvColIdxOfTestItemId);
    if(testItemId.isNull()){
      rollbackTransaction();
      return false;
    }
    // MeasuredValue
    index = pvTestItemTableModel->index(row, pvColIdxOfMeasuredValue);
    qDebug() << "Checking MeasuredValue, index " << index;
    if(pvTestItemTableModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemTableModel->data(index);
      if(!editItem(testItemId, "MeasuredValue", data)){
        rollbackTransaction();
        return false;
      }
    }
    // Result
    index = pvTestItemTableModel->index(row, pvColIdxOfResult);
    qDebug() << "Checking Result, index " << index;
    if(pvTestItemTableModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemTableModel->data(index);
      if(!editItem(testItemId, "Result", data)){
        rollbackTransaction();
        return false;
      }
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

/**
void mdtTtTest::onSqlModelDestroyed(QObject *obj)
{
  Q_ASSERT(obj != 0);

  qDebug() << "mdtTtTest::onSqlModelDestroyed() - obj: " << obj;
  if(obj == pvTestItemTableModel){
    pvTestItemTableModel = 0;
  }
}
*/


bool mdtTtTest::addTestItems(const QVariant & testId, const QVariant & testModelId)
{
  QString sql;
  QList<QSqlRecord> modelItemsDataList;
  mdtSqlRecord testItemData;
  bool ok;
  int i;

  // Get items for given test model
  sql = "SELECT Id_PK FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testModelId.toString();
  modelItemsDataList = getData(sql, &ok);
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
    testItemData.setValue("TestModelItem_Id_FK", modelItemsDataList.at(i).value(0));
    if(!addRecord(testItemData, "TestItem_tbl")){
      return false;
    }
  }
  resetTestItemTableModel(testId);

  return true;
}

bool mdtTtTest::removeTestItems(const QVariant & testId)
{
  if(!removeData("TestItem_tbl", "Test_Id_FK", testId)){
    return false;
  }
  resetTestItemTableModel(testId);

  return true;
}

void mdtTtTest::resetTestItemTableModel(const QVariant & testId)
{
  Q_ASSERT(pvTestItemTableModel);

  QString filter;

  // Set new filter
  if(testId.isNull()){
    filter = "Test_Id_FK = -1";
  }else{
    filter = "Test_Id_FK = " + testId.toString();
  }
  pvTestItemTableModel->setFilter(filter);
  // Cache all data to model
  if(pvTestItemTableModel->rowCount() > 0){
    while(pvTestItemTableModel->canFetchMore()){
      pvTestItemTableModel->fetchMore();
    }
  }
  // Reset row cursor
  pvCurrentTestItemRow = 0;
}


bool mdtTtTest::testItemSqlModelOk()
{
  if(pvTestItemTableModel == 0){
    pvLastError.setError("SQL model for test item was not set.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  return true;
}

QModelIndex mdtTtTest::indexOfTestItem(const QVariant & testItemId, int column)
{
  Q_ASSERT(pvTestItemTableModel != 0);

  QModelIndex index;
  QVariant data;
  int row;
  bool found = false;

  // Find row for given test item ID
  for(row = 0; row < pvTestItemTableModel->rowCount(); ++row){
    index = pvTestItemTableModel->index(row, pvColIdxOfTestItemId);
    if(index.isValid()){
      data = pvTestItemTableModel->data(index);
      if(data == testItemId){
        found = true;
        break;
      }
    }
  }
  if(!found){
    pvLastError.setError("Could not find row of test item ID " + testItemId.toString() + " in " + pvTestItemTableModel->tableName(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }
  // Get requested index
  index = pvTestItemTableModel->index(row, column);
  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError("A invalid index was returned for row " + QString::number(row) + ", column " + QString::number(column), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }

  return index;
}

bool mdtTtTest::setTestItemSqlModelData(const QModelIndex & index, const QVariant & data)
{
  Q_ASSERT(pvTestItemTableModel != 0);

  if(!pvTestItemTableModel->setData(index, data)){
    QSqlError sqlError;
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError("Could not set data in " + pvTestItemTableModel->tableName(), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

QVariant mdtTtTest::getTestItemSqlModelData(const QModelIndex & index)
{
  Q_ASSERT(pvTestItemTableModel != 0);

  QVariant data;

  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError("A invalid index was returned for row " + QString::number(index.row()) + ", column " + QString::number(index.column()), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return data;
  }
  // Get data and generate a error if it is invalid
  data = pvTestItemTableModel->data(index);
  if(!data.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError("Invalid data was returned for row " + QString::number(index.row()) + ", column " + QString::number(index.column()), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }

  return data;
}

QVariant mdtTtTest::getTestItemSqlModelData(int row, int column)
{
  Q_ASSERT(pvTestItemTableModel != 0);

  QModelIndex index;

  index = pvTestItemTableModel->index(row, column);

  return getTestItemSqlModelData(index);
}
