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
#include "mdtTtDut01TestData.h"
#include "mdtTest.h"
#include "mdtTtBase.h"
#include <QList>
#include <QSqlRecord>
#include <QDebug>

bool mdtTtDut01TestData::populate()
{
  if(!createVehicleTypeAndUnit()){
    return false;
  }
  if(!createConnectorX1()){
    return false;
  }
  if(!createsLinks()){
    return false;
  }
  return true;
}

void mdtTtDut01TestData::clear()
{
  removeLinks();
  removeConnections();
  removeUnitAndVehicleType();
}

bool mdtTtDut01TestData::createVehicleTypeAndUnit()
{
  QSqlQuery query(pvDatabase);

  if(!beginTransaction()){
    return false;
  }
  // VehicleType_tbl part
  if(!query.exec("INSERT INTO VehicleType_tbl (Type, SubType, SeriesNumber) VALUES ('DUT 01 VT', 'DUT 01 VST', '1')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  pvVehicleTypeId = query.lastInsertId();
  Q_ASSERT(!pvVehicleTypeId.isNull());
  // Unit_tbl part
  if(!query.exec("INSERT INTO Unit_tbl (SchemaPosition) VALUES ('DUT 01')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  pvUnitId = query.lastInsertId();
  Q_ASSERT(!pvUnitId.isNull());
  // VehicleType_Unit_tbl
  if(!query.exec("INSERT INTO VehicleType_Unit_tbl (Unit_Id_FK, VehicleType_Id_FK) VALUES (" + pvUnitId.toString() + ", " + pvVehicleTypeId.toString() + ")")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtDut01TestData::removeUnitAndVehicleType()
{
  mdtTtBase tb(0, pvDatabase);

  QVERIFY(tb.removeData("VehicleType_Unit_tbl", "Unit_Id_FK", pvUnitId, "VehicleType_Id_FK", pvVehicleTypeId));
  QVERIFY(tb.removeData("Unit_tbl", "Id_PK", pvUnitId));
  QVERIFY(tb.removeData("VehicleType_tbl", "Id_PK", pvVehicleTypeId));
}

bool mdtTtDut01TestData::createConnectorX1()
{
  QSqlQuery query(pvDatabase);
  QVariant connectorId;

  if(!beginTransaction()){
    return false;
  }
  // UnitConnector_tbl part
  if(!query.exec("INSERT INTO UnitConnector_tbl (Unit_Id_FK, Name) VALUES (" + pvUnitId.toString() + ", 'X1')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  connectorId = query.lastInsertId();
  Q_ASSERT(!connectorId.isNull());
  // UnitConnection_tbl part
  if(!addUnitConnection(connectorId, "P", "A")){
    rollbackTransaction();
    return false;
  }
  if(!addUnitConnection(connectorId, "P", "B")){
    rollbackTransaction();
    return false;
  }
  if(!addUnitConnection(connectorId, "P", "C")){
    rollbackTransaction();
    return false;
  }
  if(!addUnitConnection(connectorId, "P", "D")){
    rollbackTransaction();
    return false;
  }
  if(!addUnitConnection(connectorId, "P", "E")){
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtDut01TestData::removeConnections()
{
  mdtTtBase tb(0, pvDatabase);

  QVERIFY(tb.removeData("UnitConnection_tbl", "Unit_Id_FK", pvUnitId));
  QVERIFY(tb.removeData("UnitConnector_tbl", "Unit_Id_FK", pvUnitId));
}

bool mdtTtDut01TestData::addUnitConnection(const QVariant& unitConnectorId, const QString & contactType, const QString & contactName)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  sql = "INSERT INTO UnitConnection_tbl (Unit_Id_FK, ConnectionType_Code_FK, UnitContactName)";
  sql += " VALUES (" + pvUnitId.toString() + ",'" + contactType + "','" + contactName + "')";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

bool mdtTtDut01TestData::createsLinks()
{
  if(!beginTransaction()){
    return false;
  }

  if(!addLink("W001", 0.0, "X1", "A", "X1", "B")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W001", 0.0, "X1", "C", "X1", "D")){
    rollbackTransaction();
    return false;
  }
  if(!addLink("W001", 0.0, "X1", "D", "X1", "E")){
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtDut01TestData::removeLinks()
{
  mdtTtBase tb(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  sql = "SELECT * FROM VehicleType_Link_tbl WHERE VehicleTypeStart_Id_FK = " + pvVehicleTypeId.toString() + " AND VehicleTypeEnd_Id_FK = " + pvVehicleTypeId.toString();
  dataList = tb.getDataList<QSqlRecord>(sql, ok);
  QVERIFY(ok);
  for(const auto & rec : dataList){
    removeLink(rec.value("UnitConnectionStart_Id_FK"), rec.value("UnitConnectionEnd_Id_FK"));
  }
}

bool mdtTtDut01TestData::addLink(const QString & identification, const QVariant & R, const QString& startConnectorName, const QString& startConnection, const QString& endConnectorName, const QString& endConnection)
{
  mdtTtBase tb(0, pvDatabase);
  QSqlQuery query(pvDatabase);
  QString sql;
  bool ok;
  QList<QVariant> dataList;
  QVariant startCnrId, endCnrId;
  QVariant startConnectionId, endConnectionId;

  // Get start connector ID
  sql = "SELECT Id_PK FROM UnitConnector_tbl WHERE Name = '" + startConnectorName + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  startCnrId = dataList.at(0);
  // Get end connector ID
  sql = "SELECT Id_PK FROM UnitConnector_tbl WHERE Name = '" + endConnectorName + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  endCnrId = dataList.at(0);
  // Get start connection ID
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + pvUnitId.toString() + " AND UnitContactName = '" + startConnection + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  startConnectionId = dataList.at(0);
  // Get start connection ID
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + pvUnitId.toString() + " AND UnitContactName = '" + endConnection + "'";
  dataList = tb.getDataList<QVariant>(sql, ok);
  Q_ASSERT(ok);
  Q_ASSERT(dataList.size() == 1);
  endConnectionId = dataList.at(0);
  // Add to Link_tbl
  sql = "INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, LinkType_Code_FK, LinkDirection_Code_FK, Identification, Resistance)";
  sql += " VALUES (" + startConnectionId.toString() + "," + endConnectionId.toString() + ",'CABLELINK','BID'";
  sql += ",'" + identification + "'," + R.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }
  // Add to VehicleType_Link_tbl
  sql = "INSERT INTO VehicleType_Link_tbl (VehicleTypeStart_Id_FK,VehicleTypeEnd_Id_FK,UnitConnectionStart_Id_FK,UnitConnectionEnd_Id_FK)";
  sql += " VALUES (" + pvVehicleTypeId.toString() + "," + pvVehicleTypeId.toString() + "," + startConnectionId.toString() + "," + endConnectionId.toString() + ")";
  if(!query.exec(sql)){
    debugSqlError(query);
    return false;
  }

  return true;
}

void mdtTtDut01TestData::removeLink(const QVariant& startConnectionId, const QVariant& endConnectionId)
{
  QSqlQuery query(pvDatabase);
  QString sql;

  // Remove from VehicleType_Link_tbl
  sql = "DELETE FROM VehicleType_Link_tbl WHERE UnitConnectionStart_Id_FK = " + startConnectionId.toString() + " AND UnitConnectionEnd_Id_FK = " + endConnectionId.toString();
  sql += " AND VehicleTypeStart_Id_FK = " + pvVehicleTypeId.toString() + " AND VehicleTypeEnd_Id_FK = " + pvVehicleTypeId.toString();
  QVERIFY(query.exec(sql));
  // Remove from Link_tbl
  sql = "DELETE FROM Link_tbl WHERE UnitConnectionStart_Id_FK = " + startConnectionId.toString() + " AND UnitConnectionEnd_Id_FK = " + endConnectionId.toString();
  QVERIFY(query.exec(sql));
}

bool mdtTtDut01TestData::beginTransaction()
{
  if(!pvDatabase.transaction()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtDut01TestData::commitTransaction()
{
  if(!pvDatabase.commit()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtDut01TestData::rollbackTransaction()
{
  if(!pvDatabase.rollback()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

void mdtTtDut01TestData::debugSqlError(const QSqlError& error)
{
    qDebug() << error;
}

void mdtTtDut01TestData::debugSqlError(const QSqlQuery& query)
{
  qDebug() << "Error on query '" << query.lastQuery() << "': " << query.lastError();
}
