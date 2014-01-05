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
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QList>

#include <QDebug>

mdtTtTest::mdtTtTest(QSqlDatabase db)
 : mdtClBase(db)
{
  pvTestItemSqlModel = 0;
  pvColIdxOfTestItemId = -1;
  pvColIdxOfExpectedValue = -1;
  pvColIdxOfMeasuredValue = -1;
  pvColIdxOfResult = -1;
}

bool mdtTtTest::setTestItemSqlModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  QStringList missingFields;
  QString errorText;
  int i;

  /**
  if(pvTestItemSqlModel != 0){
    disconnect(pvTestItemSqlModel, SIGNAL(destroyed(QObject*)), this, SLOT(onSqlModelDestroyed(QObject*)));
  }
  */
  pvTestItemSqlModel = model;
  ///connect(pvTestItemSqlModel, SIGNAL(destroyed(QObject*)), this, SLOT(onSqlModelDestroyed(QObject*)));
  // Get index of fields that we need
  pvColIdxOfTestItemId = pvTestItemSqlModel->fieldIndex("Id_PK");
  if(pvColIdxOfTestItemId < 0){
    missingFields << "Id_PK";
  }
  pvColIdxOfExpectedValue = pvTestItemSqlModel->fieldIndex("ExpectedValue");
  if(pvColIdxOfExpectedValue < 0){
    missingFields << "ExpectedValue";
  }
  pvColIdxOfMeasuredValue = pvTestItemSqlModel->fieldIndex("MeasuredValue");
  if(pvColIdxOfMeasuredValue < 0){
    missingFields << "MeasuredValue";
  }
  pvColIdxOfResult = pvTestItemSqlModel->fieldIndex("Result");
  if(pvColIdxOfResult < 0){
    missingFields << "Result";
  }
  // Check that all column indexes where found
  if(!missingFields.isEmpty()){
    errorText = QObject::tr("Some fields could not be found in");
    errorText += " '" + pvTestItemSqlModel->tableName() + "'. " + QObject::tr("Missing fields") + " : ";
    for(i = 0; i < missingFields.size(); ++i){
      errorText += missingFields.at(i);
      if(i < (missingFields.size() - 1)){
        errorText += ", ";
      }
    }
    pvLastError.setError(errorText, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    pvTestItemSqlModel = 0;
    return false;
  }

  return true;
}

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

QList<mdtTtTestNodeUnitSetupData> mdtTtTest::getNodeUnitSetupList(const QVariant & testItemId)
{
  QList<mdtTtTestNodeUnitSetupData> dataList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  /// \todo Test node ID PK missing !
  // Note: in this view, Id_PK is the ID of test item
  sql = "SELECT "\
        " Id_PK AS TestItemId,"\
        " TestModelItem_Id_FK,"\
        " SchemaPosition,"\
        " State,"\
        " Value,"\
        " NodeId ";
  sql += " FROM TestItemNodeUnitSetup_view WHERE Id_PK = " + testItemId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test node unit setup.", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return dataList;
  }
  while(query.next()){
    mdtTtTestNodeUnitSetupData data;
    data.setId(QVariant()); /// \todo Implement !
    data.setTestItemId(query.value(0));
    data.setTestModelItemId(query.value(1));
    data.setSchemaPosition(query.value(2));
    data.setState(query.value(3));
    data.setValue(query.value(4));
    data.setNodeId(query.value(5));
    dataList.append(data);
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
  mdtTtTestModel t(database());
  QList<QVariant> testItemIdList;
  int i;

  // Get base test items ID list
  testItemIdList = t.getListOfTestItemIdListByTestId(testId);
  if(testItemIdList.isEmpty()){
    pvLastError = t.lastError();
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
  for(row = 0; row < pvTestItemSqlModel->rowCount(); ++row){
    // Get test item ID
    testItemId = getTestItemSqlModelData(row, pvColIdxOfTestItemId);
    if(testItemId.isNull()){
      rollbackTransaction();
      return false;
    }
    // MeasuredValue
    index = pvTestItemSqlModel->index(row, pvColIdxOfMeasuredValue);
    qDebug() << "Checking MeasuredValue, index " << index;
    if(pvTestItemSqlModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemSqlModel->data(index);
      if(!editItem(testItemId, "MeasuredValue", data)){
        rollbackTransaction();
        return false;
      }
    }
    // Result
    index = pvTestItemSqlModel->index(row, pvColIdxOfResult);
    qDebug() << "Checking Result, index " << index;
    if(pvTestItemSqlModel->isDirty(index)){
      qDebug() << "-> Dirty !";
      data = pvTestItemSqlModel->data(index);
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
  if(obj == pvTestItemSqlModel){
    pvTestItemSqlModel = 0;
  }
}
*/

bool mdtTtTest::testItemSqlModelOk()
{
  if(pvTestItemSqlModel == 0){
    pvLastError.setError("SQL model for test item was not set.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  return true;
}

QModelIndex mdtTtTest::indexOfTestItem(const QVariant & testItemId, int column)
{
  Q_ASSERT(pvTestItemSqlModel != 0);

  QModelIndex index;
  QVariant data;
  int row;
  bool found = false;

  // Find row for given test item ID
  for(row = 0; row < pvTestItemSqlModel->rowCount(); ++row){
    index = pvTestItemSqlModel->index(row, pvColIdxOfTestItemId);
    if(index.isValid()){
      data = pvTestItemSqlModel->data(index);
      if(data == testItemId){
        found = true;
        break;
      }
    }
  }
  if(!found){
    pvLastError.setError("Could not find row of test item ID " + testItemId.toString() + " in " + pvTestItemSqlModel->tableName(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
  }
  // Get requested index
  index = pvTestItemSqlModel->index(row, column);
  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemSqlModel->lastError();
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
  Q_ASSERT(pvTestItemSqlModel != 0);

  if(!pvTestItemSqlModel->setData(index, data)){
    QSqlError sqlError;
    sqlError = pvTestItemSqlModel->lastError();
    pvLastError.setError("Could not set data in " + pvTestItemSqlModel->tableName(), mdtError::Error);
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
  Q_ASSERT(pvTestItemSqlModel != 0);

  QVariant data;

  // Check if index is valid
  if(!index.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemSqlModel->lastError();
    pvLastError.setError("A invalid index was returned for row " + QString::number(index.row()) + ", column " + QString::number(index.column()), mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return data;
  }
  // Get data and generate a error if it is invalid
  data = pvTestItemSqlModel->data(index);
  if(!data.isValid()){
    QSqlError sqlError;
    sqlError = pvTestItemSqlModel->lastError();
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
  Q_ASSERT(pvTestItemSqlModel != 0);

  QModelIndex index;

  index = pvTestItemSqlModel->index(row, column);

  return getTestItemSqlModelData(index);
}
