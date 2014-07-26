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
#include "mdtSqlRelation.h"
#include <QSqlTableModel>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QVector>
#include <QDate>

#include <QDebug>

mdtTtTest::mdtTtTest(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db),
   pvTestViewTableModel(new QSqlTableModel(0, db)),
   pvTestTableModel(new QSqlTableModel(0, db)),
   pvTestTableRelation(new mdtSqlRelation),
   pvTestItemViewTableModel(new QSqlTableModel(0, db)),
   pvTestItemTableModel(new QSqlTableModel(0, db)),
   pvTestItemViewRelation(new mdtSqlRelation),
   pvTestItemTableRelation(new mdtSqlRelation)
{
  pvCurrentTestRow = -1;
  pvCurrentTestItemRow = -1;
  ///pvTestDataAreSaved = false;
}

bool mdtTtTest::init()
{
  QSqlError sqlError;

  // Setup test view table model
  pvTestViewTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestViewTableModel->setTable("Test_view");
  if(!pvTestViewTableModel->select()){
    sqlError = pvTestViewTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'Test_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Setup test table model
  pvTestTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestTableModel->setTable("Test_tbl");
  pvTestTableModel->setFilter("Id_PK = -1");
  if(!pvTestTableModel->select()){
    sqlError = pvTestTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'Test_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  pvCurrentTestRow = -1;
  // Setup relation between test view and test models
  pvTestTableRelation->setParentModel(pvTestViewTableModel.get());
  pvTestTableRelation->setChildModel(pvTestTableModel.get());
  if(!pvTestTableRelation->addRelation("Id_PK", "Id_PK", false)){
    pvLastError.setError(tr("Cannot add Test_view <-> Test_tbl relation"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  
  /**
  // Setup test model data
  if(!pvTestModelData.setup(database())){
    pvLastError = pvTestData.lastError();
    return false;
  }
  */
  // Setup test data
  /**
  pvTestDataAreSaved = false;
  if(!pvTestData.setup(database(), true)){
    pvLastError = pvTestData.lastError();
    return false;
  }
  */
  // Setup test item view table model
  pvTestItemViewTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestItemViewTableModel->setTable("TestItem_view");
  pvTestItemViewTableModel->setFilter("Test_Id_FK = -1");
  pvTestItemViewTableModel->setSort(pvTestItemViewTableModel->fieldIndex("SequenceNumber"), Qt::AscendingOrder);
  if(!pvTestItemViewTableModel->select()){
    sqlError = pvTestItemViewTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestItem_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Setup test item table model
  pvTestItemTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTestItemTableModel->setTable("TestItem_tbl");
  pvTestItemTableModel->setFilter("Test_Id_FK = -1");
  if(!pvTestItemTableModel->select()){
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestItem_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Setup relation between test and test item models
  pvTestItemTableRelation->setParentModel(pvTestViewTableModel.get());
  pvTestItemTableRelation->setChildModel(pvTestItemTableModel.get());
  if(!pvTestItemTableRelation->addRelation("Id_PK", "Test_Id_FK", false)){
    pvLastError.setError(tr("Cannot add Test_view <-> TestItem_tbl relation"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Setup relation between test and test item view models
  pvTestItemViewRelation->setParentModel(pvTestViewTableModel.get());
  pvTestItemViewRelation->setChildModel(pvTestItemViewTableModel.get());
  if(!pvTestItemViewRelation->addRelation("Id_PK", "Test_Id_FK", true)){
    pvLastError.setError(tr("Cannot add Test_view <-> TestItem_view relation"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  setCurrentTestIndexRow(-1);
  ///resetTestItemTableModels();

  return true;
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

QSqlRecord mdtTtTest::testData() const
{
  return pvTestViewTableModel->record(pvCurrentTestRow);
}

void mdtTtTest::setCurrentTest(const QVariant & testId)
{
  
  qDebug() << "mdtTtTest::setCurrentTest() - testId: " << testId;
  
  QString filter;

  if(testId.isNull()){
    filter = "Id_PK = -1";
  }else{
    filter = "Id_PK = " + testId.toString();
  }
  pvTestViewTableModel->setFilter(filter);
  if(pvTestViewTableModel->rowCount() > 0){
    Q_ASSERT(pvTestViewTableModel->rowCount() == 1);
    setCurrentTestIndexRow(0);
  }else{
    setCurrentTestIndexRow(-1);
  }
  
  /**
  QModelIndex index;
  QVariant data;
  int row, col;

  // Find column of Id_PK
  col = pvTestViewTableModel->fieldIndex("Id_PK");
  qDebug() << "mdtTtTest::setCurrentTest() - col: " << col;
  if(col < 0){
    setCurrentTestIndexRow(-1);
    return false;
  }
  /// \todo Cache only if neened (while not found, .....)
  // Cache all data to model
  if(pvTestViewTableModel->rowCount() > 0){
    while(pvTestViewTableModel->canFetchMore()){
      pvTestViewTableModel->fetchMore();
    }
  }
  // Find row for given test ID
  for(row = 0; row < pvTestViewTableModel->rowCount(); ++row){
    index = pvTestViewTableModel->index(row, col);
    if(index.isValid()){
      data = pvTestViewTableModel->data(index);
      if(data == testId){
        // Ok, found
        setCurrentTestIndexRow(row);
        return true;
      }
    }
  }
  // Not found
  setCurrentTestIndexRow(-1);
  */

  ///return true;
}

/**
bool mdtTtTest::setCurrentTest(const QVariant & testId)
{
  mdtTtTestModel tm(0, database());
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  // Get test data
  sql = "SELECT * FROM Test_tbl WHERE Id_PK = " + testId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Could not find data in 'Test_tbl' for Id_PK = '") + testId.toString() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModel");
    pvLastError.commit();
    return false;
  }
  Q_ASSERT(dataList.size() == 1);
  pvTestData = dataList.at(0);
  // Get test model data
  pvTestData.setModelData(tm.getTestModelData(pvTestData.value("TestModel_Id_FK"), ok));
  if(!ok){
    pvTestData.clearValues();
    return false;
  }
  pvTestDataAreSaved = true;
  resetTestItemTableModels();
  ///emit testDataChanged(pvTestData);

  return true;
}
*/

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
    data.setModelData(tm.getTestModelData(data.value("TestModel_Id_FK"), *ok));
    if(!*ok){
      data.clearValues();
      return data;
    }
  }

  return data;
}

bool mdtTtTest::createTest(const QVariant & testModelId)
{
  QSqlQuery query(database());
  QVariant testId;
  mdtSqlRecord testData;

  // Setup test data
  if(!testData.addAllFields("Test_tbl", database())){
    pvLastError = testData.lastError();
    return false;
  }
  testData.setValue("TestModel_Id_FK", testModelId);
  testData.setValue("Date", QDate::currentDate());
  // Create test
  if(!beginTransaction()){
    return false;
  }
  if(!addRecord(testData, "Test_tbl", query)){
    rollbackTransaction();
    return false;
  }
  testId = query.lastInsertId();
  if(!addTestItems(testId, testModelId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }
  setCurrentTest(testId);

  return true;
}

bool mdtTtTest::saveCurrentTest()
{
  QVariant testId;
  QSqlRecord record;
  bool needToSave;
  int col;
  QModelIndex index;

  testId = testData().value("Id_PK");
  qDebug() << "->-> Save test - ID: " << testId;
  qDebug() << "->-> Save test - Test_view rec: " << pvTestViewTableModel->record(pvCurrentTestRow);
  qDebug() << "->-> Save test - Test_tbl  rec: " << pvTestTableModel->record(0);
  if(testId.isNull()){
    return true;
  }
  if(pvTestTableModel->rowCount() < 1){
    return true;
  }
  // Save data in Test_tbl if needed
  needToSave = false;
  record = pvTestTableModel->record(0);
  for(col = 0; col < pvTestTableModel->columnCount(); ++col){
    index = pvTestTableModel->index(0, col);
    if(pvTestTableModel->isDirty(index)){
      needToSave = true;
      break;
    }
  }
  qDebug() << "->-> needToSave: " << needToSave;
  if(!beginTransaction()){
    return false;
  }
  if(needToSave){
    if(!pvTestTableModel->submitAll()){
      rollbackTransaction();
      QSqlError sqlError;
      sqlError = pvTestTableModel->lastError();
      pvLastError.setError("Cannot save data in Test_tbl", mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
      pvLastError.commit();
      return false;
    }
  }
  // Save data in TestItem_tbl
  qDebug() << "->-> Items count: " << pvTestItemTableModel->rowCount();
  if(!pvTestItemTableModel->submitAll()){
    rollbackTransaction();
    QSqlError sqlError;
    sqlError = pvTestItemTableModel->lastError();
    pvLastError.setError("Cannot save data in TestItem_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }
  setCurrentTest(testId);

  return true;
}

bool mdtTtTest::removeCurrentTest()
{
  QVariant testId;

  testId = testData().value("Id_PK");
  if(testId.isNull()){
    return true;
  }
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
  setCurrentTest(QVariant());

  return true;
}

/**
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
  pvTestData = data;
  pvTestData.setValue("Id_PK", testId);
  if(!addTestItems(data.value("TestModel_Id_FK"))){
    rollbackTransaction();
    return QVariant();
  }
  if(!commitTransaction()){
    return QVariant();
  }
  pvTestDataAreSaved = true;
  resetTestItemTableModels();
  emit testDataChanged(pvTestData);

  return testId;
}
*/

/**
bool mdtTtTest::updateTest(const QVariant & testId, const mdtTtTestData & data)
{
  bool newTestModel;

  newTestModel = (pvTestData.value("TestModel_Id_FK") != data.value("TestModel_Id_FK"));
  pvTestData = data;
  if(!beginTransaction()){
    return false;
  }
  // If test model has changed, we have to update items
  if(newTestModel){
    if(!removeTestItems()){
      rollbackTransaction();
      return false;
    }
    if(!addTestItems(pvTestData.value("TestModel_Id_FK"))){
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
  pvTestDataAreSaved = true;

  return true;
}
*/

bool mdtTtTest::testIsEmpty() const
{
  QSqlRecord record;
  int i;
  int row;

  // Check test data in Test_tbl
  qDebug() << "++ Test is empty - row count: " << pvTestTableModel->rowCount();
  if(pvTestTableModel->rowCount() < 1){
    return true;
  }
  Q_ASSERT(pvTestTableModel->rowCount() == 1);
  record = pvTestTableModel->record(0);
  for(i = 0; i < record.count(); ++i){
    qDebug() << "++ Test is empty - Test_tbl - Field " << record.fieldName(i) << " : " << record.value(i);
    if((record.fieldName(i) != "Id_PK")&&(record.fieldName(i) != "TestModel_Id_FK")&&(record.fieldName(i) != "Date")){
      if(!record.value(i).isNull()){
        return false;
      }
    }
  }
  // Check in TestItem_tbl
  for(row = 0; row < pvTestItemTableModel->rowCount(); ++row){
    record = pvTestItemTableModel->record(row);
    for(i = 0; i < record.count(); ++i){
      qDebug() << "++ Test is empty - TestItem_tbl - Field " << record.fieldName(i) << " : " << record.value(i);
      if((record.fieldName(i) != "Id_PK")&&(record.fieldName(i) != "Test_Id_FK")&&(record.fieldName(i) != "TestModelItem_Id_FK")){
        if(!record.value(i).isNull()){
          return false;
        }
      }
    }
  }

  return true;
}

bool mdtTtTest::testIsSaved()
{
  int row, col;
  QModelIndex index;

  /**
  if(pvTestData.value("Id_PK").isNull()){
    return true;
  }
  if(!pvTestDataAreSaved){
    return false;
  }
  */
  // Check test data in Test_tbl
  if(pvTestTableModel->rowCount() < 1){
    return true;
  }
  Q_ASSERT(pvTestTableModel->rowCount() == 1);
  for(col = 0; col < pvTestTableModel->columnCount(); ++col){
    // Test_tbl model if filtered on current Id_PK, it contains only 1 row
    index = pvTestTableModel->index(0, col);
    qDebug() << "-- Checking in Test_tbl, data: " << pvTestTableModel->data(index);
    if(pvTestTableModel->isDirty(index)){
      return false;
    }
  }
  // Check test items
  for(row = 0; row < pvTestItemTableModel->rowCount(); ++row){
    for(col = 0; col < pvTestItemTableModel->columnCount(); ++col){
      index = pvTestItemTableModel->index(row, col);
      if(pvTestItemTableModel->isDirty(index)){
        return false;
      }
    }
  }

  return true;
}

void mdtTtTest::setCurrentTestItemData(const QString & fieldName, const QVariant & data)
{
  QVariant testItemId;
  int testItemViewCol;
  int testItemTableRow, testItemTableCol;
  QModelIndex index;

  if(pvTestItemViewTableModel->rowCount() < 1){
    return;
  }
  Q_ASSERT(pvCurrentTestItemRow >= 0);
  // Get column index of fieldName
  testItemViewCol = pvTestItemViewTableModel->fieldIndex(fieldName);
  Q_ASSERT(testItemViewCol >= 0);
  testItemTableCol = pvTestItemTableModel->fieldIndex(fieldName);
  Q_ASSERT(testItemTableCol >= 0);
  // Get corresponding row in TestItem_view
  testItemId = pvTestItemViewTableModel->record(pvCurrentTestItemRow).value("Id_PK");
  Q_ASSERT(!testItemId.isNull());
  testItemTableRow = getTestItemTableModelIndexRow(testItemId);
  Q_ASSERT(testItemTableRow >= 0);
  // Set value
  index = pvTestItemViewTableModel->index(pvCurrentTestItemRow, testItemViewCol);
  pvTestItemViewTableModel->setData(index, data);
  index = pvTestItemTableModel->index(testItemTableRow, testItemTableCol);
  pvTestItemTableModel->setData(index, data);
}

bool mdtTtTest::hasMoreTestItem() const
{
  Q_ASSERT(pvTestItemViewTableModel);

  if(pvCurrentTestItemRow < (pvTestItemViewTableModel->rowCount() - 1)){
    return true;
  }
  if(pvTestItemViewTableModel->canFetchMore()){
    pvTestItemViewTableModel->fetchMore();
    return true;
  }
  return false;
}

void mdtTtTest::resetTestItemCursor()
{
  pvCurrentTestItemRow = -1;
}

QVariant mdtTtTest::nextTestItem()
{
  Q_ASSERT(pvTestItemViewTableModel);

  QModelIndex index;
  int col;

  col = pvTestItemViewTableModel->fieldIndex("Id_PK");
  Q_ASSERT(col >= 0);
  
  ++pvCurrentTestItemRow;
  
  index = pvTestItemViewTableModel->index(pvCurrentTestItemRow, col);
  
  /// \todo Provisoire !
  col = pvTestItemViewTableModel->fieldIndex("SequenceNumber");
  QModelIndex index2;
  index2 = pvTestItemViewTableModel->index(pvCurrentTestItemRow, col);
  qDebug() << "mdtTtTest - current seq #: " << pvTestItemViewTableModel->data(index2);
  
  

  return pvTestItemViewTableModel->data(index);
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
bool mdtTtTest::setTestItemSqlModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  QStringList missingFields;
  QString errorText;
  int i;

  pvTestItemViewTableModel = model;
  ///connect(pvTestItemViewTableModel, SIGNAL(destroyed(QObject*)), this, SLOT(onSqlModelDestroyed(QObject*)));
  // Get index of fields that we need
  pvColIdxOfTestItemId = pvTestItemViewTableModel->fieldIndex("Id_PK");
  if(pvColIdxOfTestItemId < 0){
    missingFields << "Id_PK";
  }
  pvColIdxOfExpectedValue = pvTestItemViewTableModel->fieldIndex("ExpectedValue");
  if(pvColIdxOfExpectedValue < 0){
    missingFields << "ExpectedValue";
  }
  pvColIdxOfMeasuredValue = pvTestItemViewTableModel->fieldIndex("MeasuredValue");
  if(pvColIdxOfMeasuredValue < 0){
    missingFields << "MeasuredValue";
  }
  pvColIdxOfResult = pvTestItemViewTableModel->fieldIndex("Result");
  if(pvColIdxOfResult < 0){
    missingFields << "Result";
  }
  // Check that all column indexes where found
  if(!missingFields.isEmpty()){
    errorText = QObject::tr("Some fields could not be found in");
    errorText += " '" + pvTestItemViewTableModel->tableName() + "'. " + QObject::tr("Missing fields") + " : ";
    for(i = 0; i < missingFields.size(); ++i){
      errorText += missingFields.at(i);
      if(i < (missingFields.size() - 1)){
        errorText += ", ";
      }
    }
    pvLastError.setError(errorText, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    pvTestItemViewTableModel = 0;
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

/**
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
  for(row = 0; row < pvTestItemViewTableModel->rowCount(); ++row){
    // Get test item ID
    testItemId = getTestItemSqlModelData(row, pvColIdxOfTestItemId);
    if(testItemId.isNull()){
      rollbackTransaction();
      return false;
    }
    // MeasuredValue
    index = pvTestItemViewTableModel->index(row, pvColIdxOfMeasuredValue);
    qDebug() << "Checking MeasuredValue, index " << index;
    if(pvTestItemViewTableModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemViewTableModel->data(index);
      if(!editItem(testItemId, "MeasuredValue", data)){
        rollbackTransaction();
        return false;
      }
    }
    // Result
    index = pvTestItemViewTableModel->index(row, pvColIdxOfResult);
    qDebug() << "Checking Result, index " << index;
    if(pvTestItemViewTableModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemViewTableModel->data(index);
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
*/

/**
void mdtTtTest::onSqlModelDestroyed(QObject *obj)
{
  Q_ASSERT(obj != 0);

  qDebug() << "mdtTtTest::onSqlModelDestroyed() - obj: " << obj;
  if(obj == pvTestItemViewTableModel){
    pvTestItemViewTableModel = 0;
  }
}
*/




void mdtTtTest::setCurrentTestIndexRow(int row)
{
  qDebug() << "mdtTtTest::setCurrentTestIndexRow() - row: " << row;
  
  pvTestTableRelation->setParentCurrentIndex(row);
  pvTestItemTableRelation->setParentCurrentIndex(row);
  pvTestItemViewRelation->setParentCurrentIndex(row);
  pvCurrentTestRow = row;
  qDebug() << " * Current rec in Test_view: " << pvTestViewTableModel->record(pvCurrentTestRow);
  qDebug() << " * Records in Test_tbl:  " << pvTestTableModel->rowCount();
  qDebug() << " * Current rec in Test_tbl:  " << pvTestTableModel->record(0);
  qDebug() << " * Records in TestItem_view: " << pvTestItemViewTableModel->rowCount();
  qDebug() << " * Records in TestItem_tbl: " << pvTestItemTableModel->rowCount();
  ///pvTestItemTableRelation->setParentCurrentIndex(0);
  qDebug() << "mdtTtTest::setCurrentTestIndexRow() - emit testDataChanged() - data: " << testData();
  resetTestItemCursor();
  emit testDataChanged(testData());
}

/**
void mdtTtTest::updateCurrentTestRow(const QVariant & testId)
{
  QModelIndex index;
  QVariant data;
  int col;

  // Find column of Id_PK
  col = pvTestTableModel->fieldIndex("Id_PK");
  if(col < 0){
    pvCurrentTestRow = -1;
    return;
  }
  // Cache all data to model
  if(pvTestTableModel->rowCount() > 0){
    while(pvTestTableModel->canFetchMore()){
      pvTestTableModel->fetchMore();
    }
  }
  // Find row for given test ID
  for(pvCurrentTestRow = 0; pvCurrentTestRow < pvTestTableModel->rowCount(); ++pvCurrentTestRow){
    index = pvTestTableModel->index(pvCurrentTestRow, col);
    if(index.isValid()){
      data = pvTestTableModel->data(index);
      if(data == testId){
        // Ok, found
        return;
      }
    }
  }
  // Not found
  pvCurrentTestRow = -1;
}
*/

bool mdtTtTest::addTestItems(const QVariant & testId, const QVariant & testModelId)
{
  QString sql;
  QList<QVariant> modelItemsDataList;
  mdtSqlRecord testItemData;
  ///QVariant testId;
  bool ok;
  int i;

  // Get current test ID
  ///testId = pvTestData.value("Id_PK");
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
  ///resetTestItemTableModels();

  return true;
}

bool mdtTtTest::removeTestItems(const QVariant & testId)
{
  if(testId.isNull()){
    pvLastError.setError(tr("Could not remove test items because current test id (Id_PK) is Null."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Remove test items
  if(!removeData("TestItem_tbl", "Test_Id_FK", testId)){
    return false;
  }
  ///resetTestItemTableModels();

  return true;
}

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








/**
void mdtTtTest::resetTestItemTableModels()
{
  Q_ASSERT(pvTestItemViewTableModel);
  Q_ASSERT(pvTestItemTableModel);

  QString filter;
  QVariant testId;

  // Set new filter
  ///testId = pvTestData.value("Id_PK");
  testData().value("Id_PK");
  if(testId.isNull()){
    filter = "Test_Id_FK = -1";
  }else{
    filter = "Test_Id_FK = " + testId.toString();
  }
  pvTestItemViewTableModel->setFilter(filter);
  pvTestItemTableModel->setFilter(filter);
  // Cache all data to models
  if(pvTestItemViewTableModel->rowCount() > 0){
    while(pvTestItemViewTableModel->canFetchMore()){
      pvTestItemViewTableModel->fetchMore();
    }
  }
  if(pvTestItemTableModel->rowCount() > 0){
    while(pvTestItemTableModel->canFetchMore()){
      pvTestItemTableModel->fetchMore();
    }
  }
  // Reset row cursor
  resetTestItemCursor();
}
*/


/**
bool mdtTtTest::testItemSqlModelOk()
{
  if(pvTestItemViewTableModel == 0){
    pvLastError.setError("SQL model for test item was not set.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  return true;
}
*/

/**
QModelIndex mdtTtTest::indexOfTestItem(const QVariant & testItemId, int column)
{
  Q_ASSERT(pvTestItemViewTableModel != 0);

  QModelIndex index;
  QVariant data;
  int row;
  bool found = false;

  // Find row for given test item ID
  for(row = 0; row < pvTestItemViewTableModel->rowCount(); ++row){
    index = pvTestItemViewTableModel->index(row, pvColIdxOfTestItemId);
    if(index.isValid()){
      data = pvTestItemViewTableModel->data(index);
      if(data == testItemId){
        found = true;
        break;
      }
    }
  }
  if(!found){
    pvLastError.setError("Could not find row of test item ID " + testItemId.toString() + " in " + pvTestItemViewTableModel->tableName(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }
  // Get requested index
  index = pvTestItemViewTableModel->index(row, column);
  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemViewTableModel->lastError();
    pvLastError.setError("A invalid index was returned for row " + QString::number(row) + ", column " + QString::number(column), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }

  return index;
}
*/

/**
bool mdtTtTest::setTestItemSqlModelData(const QModelIndex & index, const QVariant & data)
{
  Q_ASSERT(pvTestItemViewTableModel != 0);

  if(!pvTestItemViewTableModel->setData(index, data)){
    QSqlError sqlError;
    sqlError = pvTestItemViewTableModel->lastError();
    pvLastError.setError("Could not set data in " + pvTestItemViewTableModel->tableName(), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}
*/

/**
QVariant mdtTtTest::getTestItemSqlModelData(const QModelIndex & index)
{
  Q_ASSERT(pvTestItemViewTableModel != 0);

  QVariant data;

  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemViewTableModel->lastError();
    pvLastError.setError("A invalid index was returned for row " + QString::number(index.row()) + ", column " + QString::number(index.column()), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return data;
  }
  // Get data and generate a error if it is invalid
  data = pvTestItemViewTableModel->data(index);
  if(!data.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemViewTableModel->lastError();
    pvLastError.setError("Invalid data was returned for row " + QString::number(index.row()) + ", column " + QString::number(index.column()), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }

  return data;
}
*/

/**
QVariant mdtTtTest::getTestItemSqlModelData(int row, int column)
{
  Q_ASSERT(pvTestItemViewTableModel != 0);

  QModelIndex index;

  index = pvTestItemViewTableModel->index(row, column);

  return getTestItemSqlModelData(index);
}
*/