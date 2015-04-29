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
#include "mdtTtTestNodeRoute.h"
#include "mdtSqlRecord.h"
#include <QList>
#include <QSqlRecord>
#include <QSqlQuery>

#include <QDebug>

mdtTtTestNodeRoute::mdtTtTestNodeRoute(QObject* parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtTtTestNodeRoute::mdtTtTestNodeRoute(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

mdtTtTestNodeRouteData mdtTtTestNodeRoute::buildRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, mdtClPathGraph & graph, bool & ok)
{
  mdtTtTestNodeRouteData routeData;
  QList<QVariant> pathConnectionIdList;
  QVariant relayId;
  int i;

  // Build path
  pathConnectionIdList = graph.getShortestPath(testNodeConnectionAId, testNodeConnectionBId, ok);
  if(!ok){
    pvLastError = graph.lastError();
    return routeData;
  }
  if(pathConnectionIdList.size() < 2){
    QString msg = tr("Could not find a route from") + " ";
    msg += getSchemaPosAndContactName(testNodeConnectionAId) + " ";
    msg += tr("to") + " ";
    msg += getSchemaPosAndContactName(testNodeConnectionBId);
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeRoute");
    pvLastError.commit();
    ok = false;
    return routeData;
  }
  /*
   * Build relays list
   * mdtTtTestNode::addRelaysToGraph() has set test node uint ID for each added relay.
   */
  for(i = 1; i < pathConnectionIdList.size(); ++i){
    Q_ASSERT(!pathConnectionIdList.at(i-1).isNull());
    Q_ASSERT(!pathConnectionIdList.at(i).isNull());
    relayId = graph.getUserData(pathConnectionIdList.at(i-1), pathConnectionIdList.at(i));
    // Path contains other units than relays, we only have interrest of relays here
    if(!relayId.isNull()){
      addRelayToRoute(relayId, routeData);
    }
  }
  // Set test node and connection IDs to route data
  routeData.setTestNodeId(testNodeId);
  routeData.setConnectionAId(testNodeConnectionAId);
  routeData.setConnectionBId(testNodeConnectionBId);

  return routeData;
}

bool mdtTtTestNodeRoute::addRoute(const mdtTtTestNodeRouteData & data)
{
  QSqlQuery query(database());
  mdtSqlRecord routeRecord;
  mdtSqlRecord unitRecord;
  QVariant routeId;

  // Setup records
  if(!routeRecord.addAllFields("TestNodeRoute_tbl", database())){
    pvLastError = routeRecord.lastError();
    return false;
  }
  if(!unitRecord.addAllFields("TestNodeRouteUnit_tbl", database())){
    pvLastError = unitRecord.lastError();
    return false;
  }
  if(!beginTransaction()){
    return false;
  }
  // Add TestNodeRoute_tbl part
  routeRecord.setValue("TestNode_Id_FK", data.testNodeId());
  routeRecord.setValue("TestNodeUnitConnectionA_Id_FK", data.connectionAId());
  routeRecord.setValue("TestNodeUnitConnectionB_Id_FK", data.connectionBId());
  if(!data.resistance().isNull()){
    routeRecord.setValue("Resistance", data.resistance().value());
  }
  routeRecord.setValue("CalibrationDate", data.calibrationDate());
  if(!addRecord(routeRecord, "TestNodeRoute_tbl", query)){
    rollbackTransaction();
    return false;
  }
  routeId = query.lastInsertId();
  // Add relays to TestNodeRouteUnit_tbl
  for(const auto & relay : data.relaysToEnableVector()){
    unitRecord.clearValues();
    unitRecord.setValue("TestNodeRoute_Id_FK", routeId);
    unitRecord.setValue("TestNodeUnit_Id_FK", relay.id);
    if(!addRecord(unitRecord, "TestNodeRouteUnit_tbl")){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

mdtTtTestNodeRouteData mdtTtTestNodeRoute::getRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, bool & ok)
{
  mdtTtTestNodeRouteData routeData;
  QList<QSqlRecord> dataList;
  QList<QVariant> idList;
  QString sql;

  // Get TestNodeRoute_tbl part
  sql  = "SELECT * FROM TestNodeRoute_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND TestNodeUnitConnectionA_Id_FK = " + testNodeConnectionAId.toString();
  sql += " AND TestNodeUnitConnectionB_Id_FK = " + testNodeConnectionBId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if( (!ok) || (dataList.isEmpty())){
    return routeData;
  }
  Q_ASSERT(dataList.size() == 1);
  routeData.setId(dataList.at(0).value("Id_PK"));
  routeData.setTestNodeId(dataList.at(0).value("TestNode_Id_FK"));
  routeData.setConnectionAId(dataList.at(0).value("TestNodeUnitConnectionA_Id_FK"));
  routeData.setConnectionBId(dataList.at(0).value("TestNodeUnitConnectionB_Id_FK"));
  if(!dataList.at(0).value("Resistance").isNull()){
    routeData.setResistance(dataList.at(0).value("Resistance").toDouble());
  }
  routeData.setCalibrationDate(dataList.at(0).value("CalibrationDate").toDateTime());
  // Get relays from TestNodeRouteUnit_tbl
  sql = "SELECT TestNodeUnit_Id_FK FROM TestNodeRouteUnit_tbl WHERE TestNodeRoute_Id_FK = " + routeData.id().toString();
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    routeData.clear();
    return routeData;
  }
  for(const auto & id : idList){
    addRelayToRoute(id, routeData);
  }

  return routeData;
}

bool mdtTtTestNodeRoute::setRouteResistance(const QVariant & routeId, const mdtValueDouble & resistance, const QDateTime d)
{

}

bool mdtTtTestNodeRoute::removeRoute(const QVariant & routeId)
{
  if(!beginTransaction()){
    return false;
  }
  if(!removeRoutePv(routeId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeRoute::removeRoutes(const mdtSqlTableSelection & s)
{
  if(!beginTransaction()){
    return false;
  }
  for(const auto & id : s.dataList("Id_PK")){
    if(!removeRoutePv(id)){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeRoute::removeRoutePv(const QVariant & routeId)
{
  if(!removeData("TestNodeRouteUnit_tbl", "TestNodeRoute_Id_FK", routeId)){
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeRoute");
    return false;
  }
  if(!removeData("TestNodeRoute_tbl", "Id_PK", routeId)){
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeRoute");
    return false;
  }
  return true;
}

void mdtTtTestNodeRoute::addRelayToRoute(const QVariant & relayId, mdtTtTestNodeRouteData & routeData)
{
  Q_ASSERT(!relayId.isNull());

  QList<QVariant> dataList;
  QString sql;
  QString schemaPosition;
  bool ok;

  sql = "SELECT SchemaPosition FROM Unit_tbl WHERE Id_PK = " + relayId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if( (ok) && (dataList.size() == 1) ){
    schemaPosition = dataList.at(0).toString();
  }else{
    schemaPosition = "<\?\?\?>";
  }
  routeData.addRelayToEnable(relayId, schemaPosition);
}

// bool mdtTtTestNodeRoute::addRelayToDatabase(const mdtTtTestNodeRouteRelay & relay)
// {
// }

QString mdtTtTestNodeRoute::getSchemaPosAndContactName(const QVariant& connectionId)
{
  QString str;
  QList<QSqlRecord> dataList;
  QSqlRecord rec;
  QString sql;
  bool ok;

  sql = "SELECT U.SchemaPosition, UCNX.UnitContactName FROM UnitConnection_tbl UCNX";
  sql += " JOIN Unit_tbl U ON U.Id_PK = UCNX.Unit_Id_FK";
  sql += " WHERE UCNX.Id_PK = " + connectionId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if((!ok) || (dataList.isEmpty())){
    str = "<\?\?\?\?>";
    return str;
  }
  Q_ASSERT(dataList.size() == 1);
  rec = dataList.at(0);
  str = rec.value(0).toString() + ";" + rec.value(1).toString();

  return str;
}
