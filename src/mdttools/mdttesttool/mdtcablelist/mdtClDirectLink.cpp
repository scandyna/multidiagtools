/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtClDirectLink.h"
#include "mdtSqlSchemaTable.h"
#include "mdtClUnit.h"
#include "mdtClPathGraph.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

#include <QDebug>

mdtClDirectLink::mdtClDirectLink(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
  createDirectLinkTable();
}

mdtClDirectLink::~mdtClDirectLink() 
{
  removeDirectLinkTable();
}

QList<QVariant> mdtClDirectLink::getUnitConnectionIdListPartOfUnit(const QList<QVariant> & unitConnectionIdList, const QVariant & unitId, bool *ok) 
{
  Q_ASSERT(ok != 0);

  QList<QVariant> idList;
  QList<QSqlRecord> dataList;
  QString sql;
  int i;

  if(unitConnectionIdList.isEmpty()){
    *ok = true;
    return idList;
  }
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + unitId.toString();
  Q_ASSERT(unitConnectionIdList.size() > 0);
  sql += " AND (Id_PK = " + unitConnectionIdList.at(0).toString();
  for(i = 1; i < unitConnectionIdList.size(); ++i){
    sql += " OR Id_PK = " + unitConnectionIdList.at(i).toString();
  }
  sql += ")";
  dataList = getData(sql, ok);
  if(!*ok){
    return idList;
  }
  for(i = 0; i < dataList.size(); ++i){
    idList.append(dataList.at(i).value(0));
  }

  return idList;
}

QList<QVariant> mdtClDirectLink::getUnitConnectionIdListPartOfUnitConnector(const QList<QVariant> & unitConnectionIdList, const QVariant & unitConnectorId, bool *ok) 
{
  Q_ASSERT(ok != 0);

  QList<QVariant> idList;
  QList<QSqlRecord> dataList;
  QString sql;
  int i;

  if(unitConnectionIdList.isEmpty()){
    *ok = true;
    return idList;
  }
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + unitConnectorId.toString();
  Q_ASSERT(unitConnectionIdList.size() > 0);
  sql += " AND (Id_PK = " + unitConnectionIdList.at(0).toString();
  for(i = 1; i < unitConnectionIdList.size(); ++i){
    sql += " OR Id_PK = " + unitConnectionIdList.at(i).toString();
  }
  sql += ")";
  dataList = getData(sql, ok);
  if(!*ok){
    return idList;
  }
  for(i = 0; i < dataList.size(); ++i){
    idList.append(dataList.at(i).value(0));
  }

  return idList;
}

bool mdtClDirectLink::addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId) 
{
  QList<QSqlRecord> leftDataList;   // Data for start part in DirectLink_tbl
  QList<QSqlRecord> rightDataList;  // Data for end part in DirectLink_tbl
  QList<QSqlRecord> dataList;
  mdtSqlRecord record;
  QString sql;
  bool ok;
  int i;

  // Get left data part (that will be stored as start in DirectLink_tbl)
  dataList = getStartData(unitConnectionStartId, &ok);
  if(!ok){
    return false;
  }
  addDataList(dataList, leftDataList);
  dataList = getEndData(unitConnectionStartId, &ok);
  if(!ok){
    return false;
  }
  addDataList(dataList, leftDataList);
  // Get right data part (that will be stored as end in DirectLink_tbl)
  dataList = getStartData(unitConnectionEndId, &ok);
  if(!ok){
    return false;
  }
  addDataList(dataList, rightDataList);
  dataList = getEndData(unitConnectionEndId, &ok);
  if(!ok){
    return false;
  }
  addDataList(dataList, rightDataList);
  /*
   * Because this feature is very experimental, we go to error if left and right data part have not the same size
   */
  if(leftDataList.size() != rightDataList.size()){
    pvLastError.setError(tr("Failed to get Left/Right data part properly."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }
  // Add data - Note: transactions are handled outside this method
  for(i = 0; i < leftDataList.size(); ++i){
    record = getRecordForTable(leftDataList.at(i), rightDataList.at(i));
    if(record.isEmpty()){
      return false;
    }
    if(!addRecord(record, "DirectLink_tbl")){
      return false;
    }
  }

  return true;
}

/// \todo Add transactions
bool mdtClDirectLink::addLinksByUnitConnector(const QVariant unitConnectorIdS, const QVariant unitConnectorIdE, mdtClPathGraph *graph)
{
  Q_ASSERT(graph != 0);

  mdtClUnit unit(0, database());
  QList<QVariant> leftConnectionIdList;
  QList<QVariant> linkedConnectionIdList;
  QVariant connectorId;
  bool ok;
//   int i;
//   int k;

  // Get list of connections from connector S
  leftConnectionIdList = unit.getConnectionIdListPartOfConnectorId(unitConnectorIdS, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  // For each connection:
  //  - get linked connections
  //  - for each linked connection:
  //    * get connector ID
  //    * If connector ID is same as E, add a link
  for(auto & leftConnectionId : leftConnectionIdList){
    // Get linked connection
    if(graph->connectionExists(leftConnectionId)){
      linkedConnectionIdList = graph->getLinkedConnectionIdList(leftConnectionId, ok);
      if(!ok){
        pvLastError = graph->lastError();
        return false;
      }
      for(auto & linkedConnectionId : linkedConnectionIdList){
        // Get connector ID
        connectorId = unit.getConnectorIdOfConnectionId(linkedConnectionId, &ok);
        if(!ok){
          return false;
        }
        // Add link if connector ID is the same as E
        if(connectorId == unitConnectorIdE){
          if(!addLink(leftConnectionId, linkedConnectionId)){
            return false;
          }
        }
      }
    }
  }
/*
  for(i = 0; i < leftConnectionIdList.size(); ++i){
    linkedConnectionIdList = graph->getLinkedConnectionIdList(leftConnectionIdList.at(i), ok);
    if(!ok){
      pvLastError = graph->lastError();
      return false;
    }
    for(k = 0; k < linkedConnectionIdList.size(); ++k){
      connectorId = unit.getConnectorIdOfConnectionId(linkedConnectionIdList.at(k), &ok);
      if(!ok){
        return false;
      }
      if(connectorId == unitConnectorIdE){
        if(!addLink(leftConnectionIdList.at(i), linkedConnectionIdList.at(k))){
          return false;
        }
      }
    }
  }*/

  return true;
}

QList<QSqlRecord> mdtClDirectLink::getStartData(const QVariant & unitConnectionId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;

  sql = "SELECT UnitConnectionStart_Id_FK, StartVehicleType_Id_PK, StartVehicleType, StartVehicleSubType,"\
        " StartVehicleSerie, StartUnit_Id_PK, StartSchemaPosition, StartAlias, StartCabinet, StartCoordinate, "\
        " UnitConnectorStart_Id_FK, StartUnitConnectorName, StartUnitContactName, StartConnectionType_Code_FK, "\
        " StartFunctionEN, StartFunctionFR, StartFunctionDE, StartFunctionIT "\
        " FROM LinkList_view ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionId.toString();

  return getData(sql, ok);
}

QList<QSqlRecord> mdtClDirectLink::getEndData(const QVariant & unitConnectionId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;

  sql = "SELECT UnitConnectionEnd_Id_FK, EndVehicleType_Id_PK, EndVehicleType, EndVehicleSubType,"\
        " EndVehicleSerie, EndUnit_Id_PK, EndSchemaPosition, EndAlias, EndCabinet, EndCoordinate, "\
        " UnitConnectorEnd_Id_FK, EndUnitConnectorName, EndUnitContactName, EndConnectionType_Code_FK, "\
        " EndFunctionEN, EndFunctionFR, EndFunctionDE, EndFunctionIT "\
        " FROM LinkList_view ";
  sql += " WHERE UnitConnectionEnd_Id_FK = " + unitConnectionId.toString();

  return getData(sql, ok);
}

void mdtClDirectLink::addDataList(const QList<QSqlRecord> & src, QList<QSqlRecord> & dest)
{
  int i;

  for(i = 0; i < src.size(); ++i){
    if(!connectionExistInList(src.at(i), dest)){
      dest.append(src.at(i));
    }
  }
}

mdtSqlRecord mdtClDirectLink::getRecordForTable(const QSqlRecord & leftData, const QSqlRecord & rightData)
{
  Q_ASSERT(leftData.count() == rightData.count());

  mdtSqlRecord record;

  // Setup record with fields from DirectLink_tbl
  if(!record.addAllFields("DirectLink_tbl", database())){
    return record;
  }
  /*
   * leftData and rightData contain data from start and/or end connection in LinkList_view.
   * We know that leftData and rightData have the same order of fields (UnitConnection_Id_FK, VehicleType_Id_PK, ...),
   *  because they are get with a manual SQL query in getStartData() and getEndData().
   * So, we can index them simply for the copy.
   */
  record.setValue("UnitConnectionStart_Id_FK", leftData.value(0));
  record.setValue("StartVehicleType_Id_PK", leftData.value(1));
  record.setValue("StartVehicleType", leftData.value(2));
  record.setValue("StartVehicleSubType", leftData.value(3));
  record.setValue("StartVehicleSerie", leftData.value(4));
  record.setValue("StartUnit_Id_PK", leftData.value(5));
  record.setValue("StartSchemaPosition", leftData.value(6));
  record.setValue("StartAlias", leftData.value(7));
  record.setValue("StartCabinet", leftData.value(8));
  record.setValue("StartCoordinate", leftData.value(9));
  record.setValue("UnitConnectorStart_Id_FK", leftData.value(10));
  record.setValue("StartUnitConnectorName", leftData.value(11));
  record.setValue("StartUnitContactName", leftData.value(12));
  record.setValue("StartConnectionType_Code_FK", leftData.value(13));
  
  record.setValue("StartFunctionEN", leftData.value(14));
  record.setValue("StartFunctionFR", leftData.value(15));
  record.setValue("StartFunctionDE", leftData.value(16));
  record.setValue("StartFunctionIT", leftData.value(17));
  
  record.setValue("UnitConnectionEnd_Id_FK", rightData.value(0));
  record.setValue("EndVehicleType_Id_PK", rightData.value(1));
  record.setValue("EndVehicleType", rightData.value(2));
  record.setValue("EndVehicleSubType", rightData.value(3));
  record.setValue("EndVehicleSerie", rightData.value(4));
  record.setValue("EndUnit_Id_PK", rightData.value(5));
  record.setValue("EndSchemaPosition", rightData.value(6));
  record.setValue("EndAlias", rightData.value(7));
  record.setValue("EndCabinet", rightData.value(8));
  record.setValue("EndCoordinate", rightData.value(9));
  record.setValue("UnitConnectorEnd_Id_FK", rightData.value(10));
  record.setValue("EndUnitConnectorName", rightData.value(11));
  record.setValue("EndUnitContactName", rightData.value(12));
  record.setValue("EndConnectionType_Code_FK", rightData.value(13));

  record.setValue("EndFunctionEN", rightData.value(14));
  record.setValue("EndFunctionFR", rightData.value(15));
  record.setValue("EndFunctionDE", rightData.value(16));
  record.setValue("EndFunctionIT", rightData.value(17));
  
  
  return record;
}

bool mdtClDirectLink::connectionExistInList(const QSqlRecord & connection, const QList<QSqlRecord> & list)
{
  int i;

  for(i = 0; i < list.size(); ++i){
    if(connectionsAreTheSame(connection, list.at(i))){
      return true;
    }
  }

  return false;
}

bool mdtClDirectLink::connectionsAreTheSame(const QSqlRecord & A, const QSqlRecord & B)
{
  int connectionIdIndexA;
  int vhctIdIndexA;
  int connectionIdIndexB;
  int vhctIdIndexB;

  // Get unit connection indexe in A
  connectionIdIndexA = A.indexOf("UnitConnectionStart_Id_FK");
  if(connectionIdIndexA < 0){
    connectionIdIndexA = A.indexOf("UnitConnectionEnd_Id_FK");
  }
  if(connectionIdIndexA < 0){
    pvLastError.setError(tr("Cannot find field UnitConnection[Start|End]_Id_FK in A"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }
  // Get vehicle type index in A
  vhctIdIndexA = A.indexOf("StartVehicleType_Id_PK");
  if(vhctIdIndexA < 0){
    vhctIdIndexA = A.indexOf("EndVehicleType_Id_PK");
  }
  if(vhctIdIndexA < 0){
    pvLastError.setError(tr("Cannot find field [Start|End]VehicleType_Id_PK in A"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }
  // Get unit connection indexe in B
  connectionIdIndexB = B.indexOf("UnitConnectionStart_Id_FK");
  if(connectionIdIndexB < 0){
    connectionIdIndexB = B.indexOf("UnitConnectionEnd_Id_FK");
  }
  if(connectionIdIndexB < 0){
    pvLastError.setError(tr("Cannot find field UnitConnection[Start|End]_Id_FK in B"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }
  // Get vehicle type index in B
  vhctIdIndexB = B.indexOf("StartVehicleType_Id_PK");
  if(vhctIdIndexB < 0){
    vhctIdIndexB = B.indexOf("EndVehicleType_Id_PK");
  }
  if(vhctIdIndexB < 0){
    pvLastError.setError(tr("Cannot find field [Start|End]VehicleType_Id_PK in B"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }
  // Check if A and B are equal
  if((A.value(connectionIdIndexA) == B.value(connectionIdIndexB)) && (A.value(vhctIdIndexA) == B.value(vhctIdIndexB))){
    return true;
  }

  return false;
}

bool mdtClDirectLink::createDirectLinkTable() 
{
  mdtSqlSchemaTable st;
  QSqlQuery query(database());

  if(!removeDirectLinkTable()){
    return false;
  }
  if(!st.setupFromTable("LinkList_view", database())){
    pvLastError = st.lastError();
    return false;
  }
  st.setTableName("DirectLink_tbl", "UTF-8");
  st.setTableTemporary(true);
  if(!query.exec(st.sqlForCreateTable())){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot create table 'DirectLink_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClDirectLink::removeDirectLinkTable() 
{
  QSqlQuery query(database());

  if(!query.exec("DROP TABLE IF EXISTS 'DirectLink_tbl'")){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot remove table 'DirectLink_tbl'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClDirectLink");
    pvLastError.commit();
    return false;
  }

  return true;
}
