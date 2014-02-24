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
#include "mdtClLink.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClVehicleTypeLinkData.h"
#include "mdtError.h"
#include <boost/graph/graph_concepts.hpp>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

//#include <QDebug>

mdtClLink::mdtClLink(QObject* parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClLink::~mdtClLink()
{
}

bool mdtClLink::buildVehicleTypeLinkDataList(mdtClLinkData & linkData, const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList)
{
  int i;
  QVariant id;

  linkData.clearVehicleTypeLinkDataList();
  if(!checkVehicleTypeStartEndIdLists(vtStartIdList, vtEndIdList)){
    return false;
  }
  if(vtStartIdList.size() == vtEndIdList.size()){
    // Case of same size
    for(i = 0; i < vtStartIdList.size(); ++i){
      mdtClVehicleTypeLinkData vtLinkData;
      vtLinkData.setVehicleTypeStartId(vtStartIdList.at(i));
      vtLinkData.setVehicleTypeEndId(vtEndIdList.at(i));
      linkData.addVehicleTypeLinkData(vtLinkData);
    }
  }else{
    // Case of 1 start VT to many end VT
    if(vtStartIdList.size() == 1){
      id = vtStartIdList.at(0);
      for(i = 0; i < vtEndIdList.size(); ++i){
        mdtClVehicleTypeLinkData vtLinkData;
        vtLinkData.setVehicleTypeStartId(id);
        vtLinkData.setVehicleTypeEndId(vtEndIdList.at(i));
        linkData.addVehicleTypeLinkData(vtLinkData);
      }
    }else{
      // Case of 1 end VT to many start VT
      Q_ASSERT(vtEndIdList.size() == 1);
      id = vtEndIdList.at(0);
      for(i = 0; i < vtStartIdList.size(); ++i){
        mdtClVehicleTypeLinkData vtLinkData;
        vtLinkData.setVehicleTypeStartId(vtStartIdList.at(i));
        vtLinkData.setVehicleTypeEndId(id);
        linkData.addVehicleTypeLinkData(vtLinkData);
      }
    }
  }

  return true;
}

bool mdtClLink::addLink(const mdtClLinkData & linkData, bool handleTransaction)
{
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  // Add Link_tbl part
  if(!addRecord(linkData, "Link_tbl")){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  // Add vehicle type link part
  if(!addLinkToVehicleTypeList(linkData.vehicleTypeLinkDataList())){
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

bool mdtClLink::addLinks(const QList<mdtClLinkData> & linkDataList, bool handleTransaction)
{
  int i;

  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  for(i = 0; i < linkDataList.size(); ++i){
    if(!addLink(linkDataList.at(i), false)){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtClLink::linkExists(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, bool* ok)
{
  Q_ASSERT(ok != 0);

  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK FROM Link_tbl ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  dataList = getData(sql, ok);

  return (dataList.size() > 0);
}

mdtClLinkData mdtClLink::getLinkData(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, bool includeConnectionData, bool includeVehicleTypeLinkData, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtClLinkData linkData;
  QList<QSqlRecord> dataList;
  QString sql;

  // Get link data part
  sql = "SELECT * FROM Link_tbl";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return linkData;
  }
  if(dataList.isEmpty()){
    QString msg;
    msg = tr("No link exists from UnitConnectionStart_Id_FK ") + unitConnectionStartId.toString();
    msg += tr(" to UnitConnectionEnd_Id_FK ") + unitConnectionEndId.toString();
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    *ok = false;
    return linkData;
  }
  Q_ASSERT(dataList.size() == 1);
  linkData = dataList.at(0);
  // Get unit connection data part if required
  if(includeConnectionData){
    if(!getConnectionData(linkData, unitConnectionStartId, unitConnectionEndId)){
      *ok = false;
      return linkData;
    }
  }
  // Get vehicle type link data if required
  if(includeVehicleTypeLinkData){
    if(!getVehicleTypeLinkData(linkData, unitConnectionStartId, unitConnectionEndId)){
      *ok = false;
      return linkData;
    }
  }
  // Done
  *ok = true;

  return linkData;
}

bool mdtClLink::editLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, const mdtClLinkData & linkData)
{
  // We update many tables, so use manually a transaction
  if(!beginTransaction()){
    return false;
  }
  if(!removeLink(unitConnectionStartId, unitConnectionEndId, false)){
    rollbackTransaction();
    return false;
  }
  if(!addLink(linkData, false)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}


/** \todo Wrong way for delete: will delete links for all vehicle type assoc.
 * Check about a overloaded method that limits vehicle type assoc. , that only remove the link in Link_tbl when no entry exists in VehicleType_Link_tbl
 */
bool mdtClLink::removeLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, bool handleTransaction)
{
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  // Remove vehicle type links
  if(!removeData("VehicleType_Link_tbl", "UnitConnectionStart_Id_FK", unitConnectionStartId, "UnitConnectionEnd_Id_FK", unitConnectionEndId)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  // Remove links
  if(!removeData("Link_tbl", "UnitConnectionStart_Id_FK", unitConnectionStartId, "UnitConnectionEnd_Id_FK", unitConnectionEndId)){
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

bool mdtClLink::removeLinks(const QList<QModelIndexList> & indexListOfSelectedRowsByRows)
{
  int row;
  QModelIndexList indexes;

  for(row = 0; row < indexListOfSelectedRowsByRows.size(); ++row){
    indexes = indexListOfSelectedRowsByRows.at(row);
    Q_ASSERT(indexes.size() == 2);
    if(!removeLink(indexes.at(0).data(), indexes.at(1).data())){
      return false;
    }
  }

  return true;
}

QList<mdtClVehicleTypeLinkData> mdtClLink::getVehicleTypeLinkDataByUnitId(const QVariant & unitId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QSqlQuery query(database());
  QSqlRecord data;
  QList<QSqlRecord> dataList;
  QList<mdtClVehicleTypeLinkData> vtLinkDataList;
  int i;

  sql = "SELECT DISTINCT VTL.VehicleTypeStart_Id_FK, VTL.VehicleTypeEnd_Id_FK, VTL.UnitConnectionStart_Id_FK, VTL.UnitConnectionEnd_Id_FK ";
  sql += "FROM VehicleType_Link_tbl VTL";
  sql += " JOIN UnitConnection_tbl UCNX";
  sql += "  ON UCNX.Id_PK = VTL.UnitConnectionStart_Id_FK";
  sql += "  OR UCNX.Id_PK = VTL.UnitConnectionEnd_Id_FK";
  sql += " WHERE UCNX.Unit_Id_FK = " + unitId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return vtLinkDataList;
  }
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    mdtClVehicleTypeLinkData vtLinkData;
    vtLinkData.setVehicleTypeStartId(data.value("VehicleTypeStart_Id_FK"));
    vtLinkData.setVehicleTypeEndId(data.value("VehicleTypeEnd_Id_FK"));
    vtLinkData.setUnitConnectionStartId(data.value("UnitConnectionStart_Id_FK"));
    vtLinkData.setUnitConnectionEndId(data.value("UnitConnectionEnd_Id_FK"));
    vtLinkDataList.append(vtLinkData);
  }

  return vtLinkDataList;
}

bool mdtClLink::removeVehicleTypeLinkByUnitId(const QVariant & unitId, bool handleTransaction)
{
  QList<mdtClVehicleTypeLinkData> vtLinkDataList;
  bool ok;

  vtLinkDataList = getVehicleTypeLinkDataByUnitId(unitId, &ok);
  if(!ok){
    return false;
  }

  return removeVehicleTypeLinks(vtLinkDataList, handleTransaction);
}

bool mdtClLink::removeVehicleTypeLinks(const QList<mdtClVehicleTypeLinkData> & vtLinkList, bool handleTransaction)
{
  int i;
  QString sql;
  QSqlQuery query(database());
  mdtClVehicleTypeLinkData vtLinkData;

  if(vtLinkList.size() < 1){
    return true;
  }
  // Build SQL statement
  vtLinkData = vtLinkList.at(0);
  sql = "DELETE FROM VehicleType_Link_tbl ";
  sql += " WHERE (VehicleTypeStart_Id_FK = " + vtLinkData.vehicleTypeStartId().toString();
  sql += " AND VehicleTypeEnd_Id_FK = " + vtLinkData.vehicleTypeEndId().toString();
  sql += " AND UnitConnectionStart_Id_FK = " + vtLinkData.unitConnectionStartId().toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + vtLinkData.unitConnectionEndId().toString();
  sql += ") ";
  for(i = 1; i < vtLinkList.size(); ++i){
    vtLinkData = vtLinkList.at(i);
    sql += " OR (VehicleTypeStart_Id_FK = " + vtLinkData.vehicleTypeStartId().toString();
    sql += " AND  VehicleTypeEnd_Id_FK = " + vtLinkData.vehicleTypeEndId().toString();
    sql += " AND  UnitConnectionStart_Id_FK = " + vtLinkData.unitConnectionStartId().toString();
    sql += " AND  UnitConnectionEnd_Id_FK = " + vtLinkData.unitConnectionEndId().toString();
    sql += ") ";
  }
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  if(!query.exec(sql)){
    if(handleTransaction){
      rollbackTransaction();
    }
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot remove vehicle type link. SQL: ") + sql + tr("."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtClLink::checkVehicleTypeStartEndIdLists(const QList<QVariant> & vtStartIdList, const QList<QVariant> & vtEndIdList)
{
  // Check about empty list
  if(vtStartIdList.isEmpty()){
    pvLastError.setError(tr("No vehicle type was assigned to start connection."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  if(vtEndIdList.isEmpty()){
    pvLastError.setError(tr("No vehicle type was assigned to end connection."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  // If both lists have different size, check that one list has only one item
  if(vtStartIdList.size() != vtEndIdList.size()){
    if((vtStartIdList.size() > 1)&&(vtEndIdList.size() > 1)){
      pvLastError.setError(tr("Linking many vehicle types from start connection to many vehicle types at end connection is not possible."), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
      pvLastError.commit();
      return false;
    }
  }
  return true;
}

bool mdtClLink::addLinkToVehicleType(const mdtClVehicleTypeLinkData & data)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for insertion in VehicleType_Link table
  sql = "INSERT INTO VehicleType_Link_tbl (VehicleTypeStart_Id_FK, VehicleTypeEnd_Id_FK, UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK) "\
        "VALUES (:VehicleTypeStart_Id_FK, :VehicleTypeEnd_Id_FK, :UnitConnectionStart_Id_FK, :UnitConnectionEnd_Id_FK)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError(tr("Cannot prepare query for connection inertion in VehicleType_Link_tbl."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":VehicleTypeStart_Id_FK", data.vehicleTypeStartId());
  query.bindValue(":VehicleTypeEnd_Id_FK", data.vehicleTypeEndId());
  query.bindValue(":UnitConnectionStart_Id_FK", data.unitConnectionStartId());
  query.bindValue(":UnitConnectionEnd_Id_FK", data.unitConnectionEndId());
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError(tr("Cannot execute query for connection inertion in VehicleType_Link_tbl."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClLink::addLinkToVehicleTypeList(const QList<mdtClVehicleTypeLinkData> & dataList)
{
  int i;

  for(i = 0; i < dataList.size(); ++i){
    if(!addLinkToVehicleType(dataList.at(i))){
      return false;
    }
  }

  return true;
}

bool mdtClLink::getConnectionData(mdtClLinkData  & linkData, const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId)
{
  mdtClUnit unit(0, database());
  mdtClUnitConnectionData connectionData;
  bool ok;

  // Get start connection data
  connectionData = unit.getConnectionData(unitConnectionStartId, false, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  linkData.setStartConnectionData(connectionData);
  // Get end connection data
  connectionData = unit.getConnectionData(unitConnectionEndId, false, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  linkData.setEndConnectionData(connectionData);

  return true;
}


bool mdtClLink::getVehicleTypeLinkData(mdtClLinkData & linkData, const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId)
{
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString sql;
  bool ok;
  int i;

  linkData.clearVehicleTypeLinkDataList();
  sql = "SELECT * FROM VehicleType_Link_tbl";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  dataList = getData(sql, &ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    mdtClVehicleTypeLinkData vtlData;
    data = dataList.at(i);
    vtlData.setUnitConnectionStartId(data.value("UnitConnectionStart_Id_FK"));
    vtlData.setUnitConnectionEndId(data.value("UnitConnectionEnd_Id_FK"));
    vtlData.setVehicleTypeStartId(data.value("VehicleTypeStart_Id_FK"));
    vtlData.setVehicleTypeEndId(data.value("VehicleTypeEnd_Id_FK"));
    linkData.addVehicleTypeLinkData(vtlData);
  }

  return true;
}
