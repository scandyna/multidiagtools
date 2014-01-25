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
#include "mdtClUnitVehicleType.h"
#include "mdtError.h"
#include <QString>
#include <QSqlQuery>

#include <QDebug>

mdtClUnitVehicleType::mdtClUnitVehicleType(QSqlDatabase db)
{
  pvDatabase = db;
}

mdtClUnitVehicleType::~mdtClUnitVehicleType()
{
}

QSqlQueryModel &mdtClUnitVehicleType::vehicleTypeNotAssignedToUnitModel(const QVariant & unitId)
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
  pvVehicleTypeNotAssignedToUnitModel.setQuery(sql, pvDatabase);

  return pvVehicleTypeNotAssignedToUnitModel;
}

bool mdtClUnitVehicleType::addUnitVehicleTypeAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList)
{
  QSqlQuery query(pvDatabase);
  QString sql;
  int i;

  clearLastError();

  // Prepare query for insertion
  sql = "INSERT INTO VehicleType_Unit_tbl "\
        "(Unit_Id_FK, VehicleType_Id_FK) "\
        "VALUES (:Unit_Id_FK, :VehicleType_Id_FK)";
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for unit <-> vehicle type assignment inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitVehicleType");
    e.commit();
    return false;
  }
  // Add entries for each vehicle type ID
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    Q_ASSERT(vehicleTypeIdList.at(i).column() == 0);
    query.bindValue(":Unit_Id_FK", unitId);
    query.bindValue(":VehicleType_Id_FK", vehicleTypeIdList.at(i).data());
    // Exec query
    if(!query.exec()){
      pvLastError = query.lastError();
      mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for unit <-> vehicle type assignment inertion", mdtError::Error);
      e.setSystemError(pvLastError.number(), pvLastError.text());
      MDT_ERROR_SET_SRC(e, "mdtClUnitVehicleType");
      e.commit();
      return false;
    }
  }

  return true;
}

bool mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant & unitId, const QModelIndexList & vehicleTypeIdList)
{
  int i;
  QString sql;

  if(vehicleTypeIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM VehicleType_Unit_tbl "\
        "WHERE Unit_Id_FK = " + unitId.toString();
  for(i = 0; i < vehicleTypeIdList.size(); ++i){
    if(i == 0){
      sql += " AND ( ";
    }else{
      sql += " OR ";
    }
    sql += "VehicleType_Id_FK = " + vehicleTypeIdList.at(i).data().toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(pvDatabase);
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for unit <-> vehicle type assignment deletion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitVehicleType");
    e.commit();
    return false;
  }

  return true;
}

QSqlError mdtClUnitVehicleType::lastError() const
{
  return pvLastError;
}

void mdtClUnitVehicleType::clearLastError()
{
  pvLastError.setDatabaseText(QString());
  pvLastError.setDriverText(QString());
  pvLastError.setNumber(-1);
  pvLastError.setType(QSqlError::NoError);
  Q_ASSERT(!pvLastError.isValid());
}
