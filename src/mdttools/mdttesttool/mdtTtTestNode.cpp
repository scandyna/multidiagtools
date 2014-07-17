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
#include "mdtTtTestNode.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtClPathGraph.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

//#include <QDebug>

mdtTtTestNode::mdtTtTestNode(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

QString mdtTtTestNode::sqlForUnitSelection(const QVariant & VehicleTypeId) 
{
  QString sql;

  sql = "SELECT"\
        " Unit_tbl.Id_PK AS Unit_Id,"\
        " Unit_tbl.SchemaPosition "\
        "FROM Unit_tbl"\
        " JOIN VehicleType_Unit_tbl"\
        "  ON VehicleType_Unit_tbl.Unit_Id_FK = Unit_tbl.Id_PK ";
  sql += " WHERE VehicleType_Unit_tbl.VehicleType_Id_FK = " + VehicleTypeId.toString();
  sql += " AND Unit_Id NOT IN ("\
         "  SELECT Unit_Id_FK_PK FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + VehicleTypeId.toString() + ")";

  return sql;
}

QString mdtTtTestNode::sqlForUnitSelectionByUnitIdList(const QList<QVariant> & unitIdList)
{
  Q_ASSERT(unitIdList.size() > 0);

  QString sql;
  int i;

  sql = "SELECT"\
        " Unit_tbl.Id_PK AS Unit_Id,"\
        " Unit_tbl.SchemaPosition "\
        "FROM Unit_tbl";
  sql += " WHERE Id_PK = " + unitIdList.at(0).toString();
  for(i = 1; i < unitIdList.size(); ++i){
    sql += " OR Id_PK = " + unitIdList.at(i).toString();
  }

  return sql;
}

QString mdtTtTestNode::sqlForUnitConnectionSelection(const QVariant & unitId)
{
  QString sql;

  sql = "SELECT"\
        " UnitConnection_tbl.Id_PK AS UnitConnection_Id,"\
        " UnitConnection_tbl.UnitContactName,"\
        " UnitConnector_tbl.Name AS ConnectorName,"\
        " UnitConnection_tbl.FunctionEN,"\
        " UnitConnection_tbl.FunctionFR,"\
        " UnitConnection_tbl.FunctionDE,"\
        " UnitConnection_tbl.FunctionIT "\
        "FROM UnitConnection_tbl"\
        " LEFT JOIN UnitConnector_tbl"\
        "  ON UnitConnector_tbl.Id_PK = UnitConnection_tbl.UnitConnector_Id_FK";
  sql += " WHERE UnitConnection_tbl.Unit_Id_FK = " + unitId.toString();

  return sql;
}

QSqlRecord mdtTtTestNode::getTestNodeData(const QVariant & testNodeId, bool & ok)
{
  QString sql;
  QList<QSqlRecord> dataList;

  sql = "SELECT\n"\
        " TN.VehicleType_Id_FK_PK,\n"\
        " TN.NodeIdentification,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + testNodeId.toString();
  dataList = getData(sql, &ok);
  if(!ok){
    return QSqlRecord();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

QList<QVariant> mdtTtTestNode::getIdListOfUnitConnectionsLinkedToUnitConnectionId(const QVariant & unitConnectionId)
{
  Q_ASSERT(!unitConnectionId.isNull());

  mdtClPathGraph graph(database());
  QList<QVariant> unitConnectionIdList;

  // Load link list and find linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    return unitConnectionIdList;
  }
  unitConnectionIdList = graph.getLinkedConnectionIdList(unitConnectionId);
  if(unitConnectionIdList.isEmpty()){
    pvLastError = graph.lastError();
    return unitConnectionIdList;
  }

  return unitConnectionIdList;
}

QList<QVariant> mdtTtTestNode::getIdListOfUnitIdForUnitConnectionIdList(const QList<QVariant> & unitConnectionIdList)
{
  QList<QVariant> unitIdList;
  QString sql;
  int i;
  QSqlQuery query(database());
  QSqlError sqlError;

  if(unitConnectionIdList.isEmpty()){
    return unitIdList;
  }
  Q_ASSERT(unitConnectionIdList.size() > 0);
  // Get unit ID for each unit connection ID
  sql = "SELECT Unit_Id_FK FROM UnitConnection_tbl WHERE Id_PK = " + unitConnectionIdList.at(0).toString();
  for(i = 1; i < unitConnectionIdList.size(); ++i){
    sql += " OR Id_PK = " + unitConnectionIdList.at(i).toString();
  }
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get units list for given unit connection ID list.", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return unitIdList;
  }
  // Get unit IDs
  while(query.next()){
    unitIdList.append(query.value(0));
  }

  return unitIdList;
}

QList<QVariant> mdtTtTestNode::getIdListOfUnitConnectionsPartOfUnit(const QVariant & unitId)
{
  QList<QVariant> unitConnectionIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connections from given unit", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return unitConnectionIdList;
  }
  while(query.next()){
    unitConnectionIdList.append(query.value(0));
  }

  return unitConnectionIdList;
}

QList<QVariant> mdtTtTestNode::getChannelTestConnectionIdList(const QVariant & testNodeId, const QVariant & busName)
{
  QList<QVariant> testConnectionIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  sql = "SELECT TestConnection_Id_FK FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString() + " AND Bus = '" + busName.toString() + "'";
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get channel test connections", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return testConnectionIdList;
  }
  while(query.next()){
    testConnectionIdList.append(query.value(0));
  }

  return testConnectionIdList;
}

bool mdtTtTestNode::addMissingConnections(const QVariant & testNodeId)
{
  QList<QSqlRecord> dataList;
  mdtTtTestNodeUnit tnu(0, database());
  QString sql;
  bool ok;
  int i;

  // Get ID list of node units
  sql = "SELECT Unit_Id_FK_PK FROM TestNodeUnit_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  dataList = getData(sql, &ok);
  if(!ok){
    return false;
  }
  // Update connections for each unit
  if(!beginTransaction()){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    if(!tnu.addConnections(dataList.at(i).value(0), QVariant(), false)){
      pvLastError = tnu.lastError();
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}
