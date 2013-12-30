/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtTtTest::mdtTtTest(QSqlDatabase db)
 : mdtClBase(db)
{
}

QList<QVariant> mdtTtTest::getListOfTestItemIdListByTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> testItemIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT Id_PK FROM TestItem_tbl WHERE Test_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of test items", mdtError::Error);
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

QList<QVariant> mdtTtTest::getListOfUsedNodeIdListByTestId(const QVariant & testId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant nodeId;
  QList<QVariant> nodeIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT TestNode_Id_FK FROM TestItemNode_view WHERE Test_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of used nodes", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
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

QList<QVariant> mdtTtTest::getListOfUsedNodeUnitIdListByTestItemId(const QVariant & testItemId, const QVariant & type)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant nodeUnitId;
  QList<QVariant> nodeUnitIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT Unit_Id_FK_PK FROM TestItemNodeUnit_view WHERE Id_PK = " + testItemId.toString();
  if(!type.isNull()){
    sql += " AND Type_Code_FK = '" + type.toString() + "' ";
  }
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of used node units", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
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

QList<QVariant> mdtTtTest::getListOfUnusedNodeUnitIdListByTestId(const QVariant & testId, const QVariant & type)
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
  sql += " SELECT Unit_Id_FK_PK FROM TestItemNodeUnit_view WHERE Test_Id_FK = " + testId.toString();
  sql += ")";
  qDebug() << "SQL: " << sql;
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of unused node units", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
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

bool mdtTtTest::addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestItem_tbl (Test_Id_FK, TestLinkBusA_Id_FK, TestLinkBusB_Id_FK, ExpectedValue) "\
        "VALUES (:Test_Id_FK, :TestLinkBusA_Id_FK, :TestLinkBusB_Id_FK, :ExpectedValue)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Test_Id_FK", testId);
  query.bindValue(":TestLinkBusA_Id_FK", testLinkBusAId);
  query.bindValue(":TestLinkBusB_Id_FK", testLinkBusBId);
  query.bindValue(":ExpectedValue", expectedValue);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTest::removeTestItem(const QVariant & testItemId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestItem_tbl WHERE Id_PK = " + testItemId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test item deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTest::removeTestItems(const QModelIndexList & indexListOfSelectedRows)
{
  int row;

  for(row = 0; row < indexListOfSelectedRows.size(); ++row){
    if(!removeTestItem(indexListOfSelectedRows.at(row).data())){
      return false;
    }
  }

  return true;
}

bool mdtTtTest::addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestNodeUnitSetup_tbl (TestItem_Id_FK, TestNodeUnit_Id_FK, State) "\
        "VALUES (:TestItem_Id_FK, :TestNodeUnit_Id_FK, :State)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test node setupinsertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestItem_Id_FK", testItemId);
  query.bindValue(":TestNodeUnit_Id_FK", testNodeUnitId);
  query.bindValue(":State", state);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node setup insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTest::generateTestNodeUnitSetupForTestItem(const QVariant & testItemId)
{
  Q_ASSERT(!testItemId.isNull());

  QList<QVariant> usedTestNodeUnitIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  ///QVariant testItemId;
  QVariant nodeUnitId;
  int i;

  // Get list of used channel relays
  usedTestNodeUnitIdList = getListOfUsedNodeUnitIdListByTestItemId(testItemId, "CHANELREL");
  if(usedTestNodeUnitIdList.isEmpty()){
    return false;
  }
  // Add setup for used node units
  for(i = 0; i < usedTestNodeUnitIdList.size(); ++i){
    nodeUnitId = usedTestNodeUnitIdList.at(i);
    if(nodeUnitId.isNull()){
      pvLastError.setError("Try to add a setup for a NULL test node ID", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
      pvLastError.commit();
      return false;
    }
    if(!addTestNodeUnitSetup(testItemId, nodeUnitId, true)){
      return false;
    }
  }

  return true;
}

bool mdtTtTest::generateTestNodeUnitSetupForTest(const QVariant & testId)
{
  QList<QVariant> usedTestNodeUnitIdList;
  ///QList<QVariant> unusedTestNodeUnitIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant testItemId;
  QVariant nodeUnitId;
  int i;

  // Get list of used channel relays
  /**
  usedTestNodeUnitIdList = getListOfUsedNodeUnitIdListByTestId(testId, "CHANELREL");
  if(usedTestNodeUnitIdList.isEmpty()){
    return false;
  }
  */
/**
  // Get list of unused channel relays
  unusedTestNodeUnitIdList = getListOfUnusedNodeUnitIdListByTestId(testId, "CHANELREL");
  if(unusedTestNodeUnitIdList.isEmpty()){
    return false;
  }
  qDebug() << "Used REL: " << usedTestNodeUnitIdList;
  qDebug() << "Unused REL: " << unusedTestNodeUnitIdList;
  */
  // Get test items for given test ID
  ///sql = "SELECT Id_PK, Unit_Id_FK_PK FROM TestItemNodeUnit_view WHERE Test_Id_FK = " + testId.toString();
  sql = "SELECT Id_PK FROM TestItem_tbl WHERE Test_Id_FK = " + testId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of test items", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }
  while(query.next()){
    // Get test item data
    testItemId = query.value(0);
    if(!generateTestNodeUnitSetupForTestItem(testItemId)){
      return false;
    }
    /**
    nodeUnitId = query.value(1);
    if((testItemId.isNull())||(nodeUnitId.isNull())){
      return false;
    }
    // Add setup for used node unit
    if(!addTestNodeUnitSetup(testItemId, nodeUnitId, true)){
      return false;
    }
    */
    /**
    // Add setup for used node units
    for(i = 0; i < usedTestNodeUnitIdList.size(); ++i){
      nodeUnitId = usedTestNodeUnitIdList.at(i);
      if(!addTestNodeUnitSetup(testItemId, nodeUnitId, true)){
        return false;
      }
    }
    */
    // Add a setup for each unused node unit
    /**
    for(i = 0; i < unusedTestNodeUnitIdList.size(); ++i){
      nodeUnitId = unusedTestNodeUnitIdList.at(i);
      // Add setup for unused node unit
      if(!addTestNodeUnitSetup(testItemId, nodeUnitId, false)){
        return false;
      }
    }
    */
  }

  return true;
}

bool mdtTtTest::removeTestNodeUnitSetup(const QVariant & tnusId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestNodeUnitSetup_tbl WHERE Id_PK = " + tnusId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node unit setup deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTest::removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows)
{
  int row;

  for(row = 0; row < indexListOfSelectedRows.size(); ++row){
    if(!removeTestNodeUnitSetup(indexListOfSelectedRows.at(row).data())){
      return false;
    }
  }

  return true;
}
