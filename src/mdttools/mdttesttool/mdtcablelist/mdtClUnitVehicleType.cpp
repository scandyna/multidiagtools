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
#include "mdtClUnitVehicleType.h"
///#include "mdtClLink.h"
#include "mdtClVehicleTypeLink.h"
#include "mdtSqlTransaction.h"
#include "mdtError.h"
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

mdtClUnitVehicleType::mdtClUnitVehicleType(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClUnitVehicleType::mdtClUnitVehicleType(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

mdtClUnitVehicleType::~mdtClUnitVehicleType()
{
}

QString mdtClUnitVehicleType::sqlForVehicleTypeNotAssignedToUnit(const QVariant& unitId) const
{
  QString sql;

  // SQL query to get the list of vehicles that are not allready assigned
  sql = "SELECT Id_PK, Type , SubType , SeriesNumber "\
        "FROM VehicleType_tbl "\
        "WHERE Id_PK NOT IN ( "\
        " SELECT VehicleType_Id_FK "\
        " FROM VehicleType_Unit_tbl "\
        " WHERE Unit_Id_FK = " + unitId.toString() + " ) "\
        "ORDER BY Type ASC, SubType ASC, SeriesNumber ASC;";

  return sql;
}

bool mdtClUnitVehicleType::addUnitVehicleTypeAssignment(const QVariant & unitId, const QVariant & vehicleTypeId)
{
  QList<QVariant> idList;

  idList.append(vehicleTypeId);

  return addUnitVehicleTypeAssignments(unitId, idList);
}

bool mdtClUnitVehicleType::addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList)
{
  QSqlQuery query(database());
  QString sql;
  int i;

  // Prepare query for insertion
  sql = "INSERT INTO VehicleType_Unit_tbl "\
        "(Unit_Id_FK, VehicleType_Id_FK) "\
        "VALUES (:Unit_Id_FK, :VehicleType_Id_FK)";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot prepare query for unit <-> vehicle type assignment inertion"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitVehicleType");
    pvLastError.commit();
    return false;
  }
  // Add entries for each vehicle type ID
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    query.bindValue(":Unit_Id_FK", unitId);
    query.bindValue(":VehicleType_Id_FK", vehicleTypeIdList.at(i));
    // Exec query
    if(!query.exec()){
      QSqlError sqlError = query.lastError();
      pvLastError.setError(tr("Cannot execute query for unit <-> vehicle type assignment inertion"), mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitVehicleType");
      pvLastError.commit();
      return false;
    }
  }

  return true;
}

bool mdtClUnitVehicleType::addUnitVehicleTypeAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList)
{
  QList<QVariant> idList;
  int i;

  // Add entries for each vehicle type ID
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    Q_ASSERT(vehicleTypeIdList.at(i).column() == 0);
    idList.append(vehicleTypeIdList.at(i).data());
  }

  return addUnitVehicleTypeAssignments(unitId, idList);
}

bool mdtClUnitVehicleType::removeUnitVehicleAssignment(const QVariant& unitId, const QVariant& vehicleTypeId)
{
  QList<QVariant> idList;

  idList.append(vehicleTypeId);

  return removeUnitVehicleAssignments(unitId, idList);
}

bool mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant& unitId, const QList< QVariant >& vehicleTypeIdList)
{
  int i;
  QString sql;
  ///mdtClLink lnk(0, database());
  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());

  if(vehicleTypeIdList.size() < 1){
    return true;
  }
  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  // Remove vehicle type links
  if(!vtl.removeVehicleTypeLinks(unitId)){
    return false;
  }
//   if(!lnk.removeVehicleTypeLinkByUnitId(unitId, false)){
//     rollbackTransaction();
//     return false;
//   }
  // Generate SQL
  sql = "DELETE FROM VehicleType_Unit_tbl "\
        "WHERE Unit_Id_FK = " + unitId.toString();
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    if(i == 0){
      sql += " AND ( ";
    }else{
      sql += " OR ";
    }
    sql += "VehicleType_Id_FK = " + vehicleTypeIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot execute query for unit <-> vehicle type assignment deletion"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitVehicleType");
    pvLastError.commit();
    return false;
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

/**
bool mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList)
{
  QList<QVariant> idList;
  int i;

  // Add entries for each vehicle type ID
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    idList.append(vehicleTypeIdList.at(i).data());
  }

  return removeUnitVehicleAssignments(unitId, idList);
}
*/

bool mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant & unitId, const mdtSqlTableSelection & vehicleTypeIdListSelection)
{
  QList<QVariant> idList;

  // Add entries for each vehicle type ID
  idList = vehicleTypeIdListSelection.dataList("VehicleType_Id_FK");

  return removeUnitVehicleAssignments(unitId, idList);
}

QList<QSqlRecord> mdtClUnitVehicleType::getVehicleTypeDataList(const QVariant & unitId, bool & ok)
{
  QString sql = "SELECT * FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + unitId.toString();
  return getDataList<QSqlRecord>(sql, ok);
}
