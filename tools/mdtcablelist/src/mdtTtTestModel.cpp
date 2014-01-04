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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtTtTestModel::mdtTtTestModel(QSqlDatabase db)
 : mdtClBase(db)
{
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

QVariant mdtTtTestModel::getNextSequenceNumber(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant lastNumber;

  sql = "SELECT SequenceNumber FROM TestModelItem_tbl WHERE TestModel_Id_FK = " + testId.toString() + " ORDER BY SequenceNumber ASC";
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

bool mdtTtTestModel::generateTestNodeUnitSetup(const QVariant & testId)
{
  mdtTtTestModelItem ti(database());
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
    if(!ti.generateTestNodeUnitSetup(testItemId)){
      return false;
    }
  }

  return true;
}
