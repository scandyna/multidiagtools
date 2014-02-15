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
#include "mdtClVehicleTypeLinkData.h"
#include "mdtError.h"
#include <QSqlQuery>
#include <QSqlError>

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

bool mdtClLink::removeLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId)
{
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  // Remove vehicle type links
  if(!removeData("VehicleType_Link_tbl", "UnitConnectionStart_Id_FK", unitConnectionStartId, "UnitConnectionEnd_Id_FK", unitConnectionEndId)){
    rollbackTransaction();
    return false;
  }
  // Remove vehicle type links
  if(!removeData("Link_tbl", "UnitConnectionStart_Id_FK", unitConnectionStartId, "UnitConnectionEnd_Id_FK", unitConnectionEndId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
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

