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
  QString nameStartPart, nameEndPart;
  QString name;
  int i;

  // Get start and end name part
  nameStartPart = getSchemaPosAndContactName(testNodeConnectionAId);
  nameEndPart = getSchemaPosAndContactName(testNodeConnectionBId);
  // Build path
  pathConnectionIdList = graph.getShortestPath(testNodeConnectionAId, testNodeConnectionBId, ok);
  if(!ok){
    pvLastError = graph.lastError();
    return routeData;
  }
  if(pathConnectionIdList.size() < 2){
    QString msg = tr("Could not find a route from") + " ";
    msg += nameStartPart + " ";
    msg += tr("to") + " ";
    msg += nameEndPart;
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
      if(!addRelayToRoute(relayId, routeData)){
        routeData.clear();
        ok = false;
        return routeData;
      }
    }
  }
  // Set test node and connection IDs to route data
  routeData.setTestNodeId(testNodeId);
  routeData.setConnectionAId(testNodeConnectionAId);
  routeData.setConnectionBId(testNodeConnectionBId);
  // Build and set route name
  name = nameStartPart + ",";
  for(const auto & relay : routeData.relaysToEnableVector()){
    name += relay.schemaPosition.toString() + ",";
  }
  name += nameEndPart;
  routeData.setName(name);

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
  if(!data.name().isNull()){
    routeRecord.setValue("Name", data.name());
  }
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

bool mdtTtTestNodeRoute::routeExist(const QVariant & testNodeId, const QString & routeName, bool & ok)
{
  QList<QVariant> dataList;
  QString sql;

  sql = "SELECT COUNT(*) FROM TestNodeRoute_tbl";
  sql += " WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND Name = '" + routeName + "'";
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(dataList.size() == 1);

  return (dataList.at(0).toInt(&ok) > 0);
}

mdtTtTestNodeRouteData mdtTtTestNodeRoute::getRoute(const QVariant & testNodeRouteId, bool & ok)
{
  mdtTtTestNodeRouteData routeData;
  QList<QSqlRecord> dataList;
  QList<QVariant> idList;
  QString sql;

  // Get TestNodeRoute_tbl part
  sql  = "SELECT * FROM TestNodeRoute_tbl WHERE Id_PK = " + testNodeRouteId.toString();
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
  routeData.setName(dataList.at(0).value("Name"));
  // Get relays from TestNodeRouteUnit_tbl
  sql = "SELECT TestNodeUnit_Id_FK FROM TestNodeRouteUnit_tbl WHERE TestNodeRoute_Id_FK = " + routeData.id().toString();
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    routeData.clear();
    return routeData;
  }
  for(const auto & id : idList){
    if(!addRelayToRoute(id, routeData)){
      routeData.clear();
      ok = false;
      return routeData;
    }
  }

  return routeData;
}

// mdtTtTestNodeRouteData mdtTtTestNodeRoute::getRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, bool & ok)
// {
//   mdtTtTestNodeRouteData routeData;
//   QList<QSqlRecord> dataList;
//   QList<QVariant> idList;
//   QString sql;
// 
//   // Get TestNodeRoute_tbl part
//   sql  = "SELECT * FROM TestNodeRoute_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
//   sql += " AND TestNodeUnitConnectionA_Id_FK = " + testNodeConnectionAId.toString();
//   sql += " AND TestNodeUnitConnectionB_Id_FK = " + testNodeConnectionBId.toString();
//   dataList = getDataList<QSqlRecord>(sql, ok);
//   if( (!ok) || (dataList.isEmpty())){
//     return routeData;
//   }
//   Q_ASSERT(dataList.size() == 1);
//   routeData.setId(dataList.at(0).value("Id_PK"));
//   routeData.setTestNodeId(dataList.at(0).value("TestNode_Id_FK"));
//   routeData.setConnectionAId(dataList.at(0).value("TestNodeUnitConnectionA_Id_FK"));
//   routeData.setConnectionBId(dataList.at(0).value("TestNodeUnitConnectionB_Id_FK"));
//   if(!dataList.at(0).value("Resistance").isNull()){
//     routeData.setResistance(dataList.at(0).value("Resistance").toDouble());
//   }
//   routeData.setCalibrationDate(dataList.at(0).value("CalibrationDate").toDateTime());
//   // Get relays from TestNodeRouteUnit_tbl
//   sql = "SELECT TestNodeUnit_Id_FK FROM TestNodeRouteUnit_tbl WHERE TestNodeRoute_Id_FK = " + routeData.id().toString();
//   idList = getDataList<QVariant>(sql, ok);
//   if(!ok){
//     routeData.clear();
//     return routeData;
//   }
//   for(const auto & id : idList){
//     if(!addRelayToRoute(id, routeData)){
//       routeData.clear();
//       ok = false;
//       return routeData;
//     }
//   }
// 
//   return routeData;
// }

mdtTtTestNodeRouteData mdtTtTestNodeRoute::getRoute(const QVariant & testNodeId, const QVariant & testNodeConnectionAId, const QVariant & testNodeConnectionBId, bool & ok)
{
  QList<QVariant> idList;
  QString sql;

  sql  = "SELECT Id_PK FROM TestNodeRoute_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND TestNodeUnitConnectionA_Id_FK = " + testNodeConnectionAId.toString();
  sql += " AND TestNodeUnitConnectionB_Id_FK = " + testNodeConnectionBId.toString();
  idList = getDataList<QVariant>(sql, ok);
  if( (!ok) || (idList.isEmpty())){
    return mdtTtTestNodeRouteData();
  }
  Q_ASSERT(idList.size() == 1);

  return getRoute(idList.at(0), ok);
}

QList<mdtTtTestNodeRouteData> mdtTtTestNodeRoute::getAllRoutes(const QVariant & testNodeId, bool & ok)
{
  QList<mdtTtTestNodeRouteData> routeList;
  QList<QVariant> idList;
  QString sql;

  sql  = "SELECT Id_PK FROM TestNodeRoute_tbl WHERE TestNode_Id_FK = " + testNodeId.toString();
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return routeList;
  }
  for(const auto & id : idList){
    mdtTtTestNodeRouteData route = getRoute(id, ok);
    if(!ok){
      routeList.clear();
      return routeList;
    }
    routeList.append(route);
  }

  return routeList;
}

QList<mdtTtTestNodeRouteData> mdtTtTestNodeRoute::getAllRoutesByAlias(const QString & testNodeAlias, bool& ok)
{
  QList<QVariant> idList;
  QString sql;

  sql  = "SELECT VehicleType_Id_FK_PK FROM TestNode_tbl WHERE Alias = '" + testNodeAlias + "'";
  idList = getDataList<QVariant>(sql, ok);
  if( (!ok) || (idList.size() != 1) ){
    return QList<mdtTtTestNodeRouteData>();
  }

  return getAllRoutes(idList.at(0), ok);
}

// QString mdtTtTestNodeRoute::getRouteName(const mdtTtTestNodeRouteData & route)
// {
//   QString name;
// 
//   name = getSchemaPosAndContactName(route.connectionAId()) + ",";
//   for(const auto & relay : route.relaysToEnableVector()){
//     name += relay.schemaPosition.toString() + ",";
//   }
//   name += getSchemaPosAndContactName(route.connectionBId());
// 
//   return name;
// }

bool mdtTtTestNodeRoute::setRouteName(mdtTtTestNodeRouteData & route)
{
  QString name;

  name = getSchemaPosAndContactName(route.connectionAId()) + ",";
  for(const auto & relay : route.relaysToEnableVector()){
    name += relay.schemaPosition.toString() + ",";
  }
  name += getSchemaPosAndContactName(route.connectionBId());
  route.setName(name);

  return true;
}

// QString mdtTtTestNodeRoute::getRouteString(const mdtTtTestNodeRouteData& route)
// {
//   return getSchemaPosAndContactName(route.connectionAId()) + " - " + getSchemaPosAndContactName(route.connectionBId());
// }

bool mdtTtTestNodeRoute::setRouteResistance(const QVariant & routeId, const mdtValueDouble & resistance, const QDateTime & d)
{
  mdtSqlRecord routeRecord;

  // Setup record
  if(!routeRecord.addAllFields("TestNodeRoute_tbl", database())){
    pvLastError = routeRecord.lastError();
    return false;
  }
  if(resistance.isNull()){
    routeRecord.setValue("Resistance", QVariant());
  }else{
    routeRecord.setValue("Resistance", resistance.value());
  }
  routeRecord.setValue("CalibrationDate", d);

  return updateRecord("TestNodeRoute_tbl", routeRecord, "Id_PK", routeId);
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

bool mdtTtTestNodeRoute::addRelayToRoute(const QVariant & relayId, mdtTtTestNodeRouteData & routeData)
{
  Q_ASSERT(!relayId.isNull());

  QList<QVariant> dataList;
  QString sql;
  QString schemaPosition;
  int ioPosition;
  bool ok;

  // Get schema position
  sql = "SELECT SchemaPosition FROM Unit_tbl WHERE Id_PK = " + relayId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  if(dataList.size() == 1){
    schemaPosition = dataList.at(0).toString();
  }else{
    schemaPosition = "<\?\?\?>";
  }
  // Get I/O position
  sql = "SELECT IoPosition FROM TestNodeUnit_tbl WHERE Unit_Id_FK_PK = " + relayId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  if(dataList.size() != 1){
    pvLastError.setError(tr("Getting I/O position from TestNodeUnit_tbl returned unexpected amout of data (expected: 1 record)."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeRoute");
    pvLastError.commit();
    return false;
  }
  if(dataList.at(0).isNull()){
    ioPosition = -1;
  }else{
    ioPosition = dataList.at(0).toInt();
  }
  // Add relay
  routeData.addRelayToEnable(relayId, schemaPosition, ioPosition);

  return true;
}

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
