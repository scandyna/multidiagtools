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
#include "mdtTtTestItem.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtTtTestItem::mdtTtTestItem(QSqlDatabase db)
 : mdtClBase(db)
{
}

QList<QVariant> mdtTtTestItem::getUsedNodeUnitIdList(const QVariant & testItemId, const QVariant & type)
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

QList<QVariant> mdtTtTestItem::getTestNodeUnitSetupIdList(const QVariant & testItemId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> tnusIdList;
  QVariant id;

  // Setup and run query to get data in unit link view
  sql = "SELECT Id_PK FROM TestItemNodeUnitSetup_view WHERE TestItem_Id_FK = " + testItemId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of node unit setup", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
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

bool mdtTtTestItem::setTestLink(const QVariant & testItemId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE TestItem_tbl "\
        "SET TestLinkBusA_Id_FK = :TestLinkBusA_Id_FK, "\
        "    TestLinkBusB_Id_FK = :TestLinkBusB_Id_FK "\
        "WHERE Id_PK = " + testItemId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestLinkBusA_Id_FK", testLinkBusAId);
  query.bindValue(":TestLinkBusB_Id_FK", testLinkBusBId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestItem::generateTestNodeUnitSetup(const QVariant & testItemId)
{
  Q_ASSERT(!testItemId.isNull());

  QList<QVariant> usedTestNodeUnitIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  ///QVariant testItemId;
  QVariant nodeUnitId;
  int i;

  qDebug() << "TI: begin transaction ...";
  if(!beginTransaction()){
    return false;
  }
  qDebug() << "TI: removing old setup ...";
  // Remove allready existing setups
  if(!removeTestNodeUnitSetupsForTestItemId(testItemId)){
    rollbackTransaction();
    return false;
  }
  qDebug() << "TI: get list of channel relays ...";
  // Get list of used channel relays
  usedTestNodeUnitIdList = getUsedNodeUnitIdList(testItemId, "CHANELREL");
  if(usedTestNodeUnitIdList.isEmpty()){
    rollbackTransaction();
    return false;
  }
  // Add setup for used node units
  for(i = 0; i < usedTestNodeUnitIdList.size(); ++i){
    nodeUnitId = usedTestNodeUnitIdList.at(i);
    qDebug() << "TI: adding setup for unit ID " << nodeUnitId << " ...";
    if(nodeUnitId.isNull()){
      pvLastError.setError("Try to add a setup for a NULL test node ID", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
      pvLastError.commit();
      rollbackTransaction();
      return false;
    }
    if(!addTestNodeUnitSetup(testItemId, nodeUnitId, true)){
      rollbackTransaction();
      return false;
    }
  }
  qDebug() << "TI: commit transaction ...";
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestItem::addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestItem::removeTestNodeUnitSetup(const QVariant & tnusId)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestItem::removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows)
{
  int row;

  for(row = 0; row < indexListOfSelectedRows.size(); ++row){
    if(!removeTestNodeUnitSetup(indexListOfSelectedRows.at(row).data())){
      return false;
    }
  }

  return true;
}

bool mdtTtTestItem::removeTestNodeUnitSetupsForTestItemId(const QVariant & testItemId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestNodeUnitSetup_tbl WHERE TestItem_Id_FK = " + testItemId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node unit setup deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestItem");
    pvLastError.commit();
    return false;
  }

  return true;
}
