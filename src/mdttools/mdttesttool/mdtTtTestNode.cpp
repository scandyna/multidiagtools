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
#include "mdtTtTestNode.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtClLink.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDebug>

mdtTtTestNode::mdtTtTestNode(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtTtTestNode::mdtTtTestNode(QSqlDatabase db)
 : mdtTtBase(0, db)
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
        " TN.Alias,\n"\
        " TN.AddressString,\n"\
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

QString mdtTtTestNode::getTestNodeAlias(const QVariant & testNodeId, bool & ok)
{
  Q_ASSERT(!testNodeId.isNull());

  QString sql;
  QList<QVariant> dataList;

  sql = "SELECT TN.Alias\n"\
        "FROM TestNode_tbl TN\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n";
  sql += " WHERE TN.VehicleType_Id_FK_PK = " + testNodeId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QString();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).toString();
}

QVariant mdtTtTestNode::getTestNodeIdForAlias(const QString & alias)
{
  QString sql;
  QList<QVariant> dataList;
  bool ok;

  sql = "SELECT VehicleType_Id_FK_PK FROM TestNode_tbl WHERE Alias = '" + alias + "'";
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    pvLastError.setError(tr("Test node with alias '") + alias + tr("' was not found in TestNode_tbl"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return QVariant();
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0);
}

QList<QVariant> mdtTtTestNode::getIdListOfUnitConnectionsLinkedToUnitConnectionId(const QVariant & unitConnectionId)
{
  Q_ASSERT(!unitConnectionId.isNull());

  mdtClPathGraph graph(database());
  QList<QVariant> unitConnectionIdList;
  bool ok;

  // Load link list and find linked connections
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    return unitConnectionIdList;
  }
  unitConnectionIdList = graph.getLinkedConnectionIdList(unitConnectionId, ok);
  if(!ok){
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

std::vector<mdtTtTestNodeRouteRelay> mdtTtTestNode::getTestNodeRouteRelays(const QVariant & testNodeId, const QStringList & schemaPositionList)
{
  std::vector<mdtTtTestNodeRouteRelay> relays;
  QList<QSqlRecord> dataList;
  QString sql;
  bool ok;

  // Get data from DB
  sql = "SELECT U.Id_PK, U.SchemaPosition, TNU.IoPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (TNU.Type_Code_FK = 'BUSCPLREL' OR TNU.Type_Code_FK = 'CHANELREL')";
  sql += " AND U.SchemaPosition IN (";
  for(int i = 0; i < schemaPositionList.size(); ++i){
    if(i > 0){
      sql += ",";
    }
    sql += "'" + schemaPositionList.at(i) + "'";
  }
  sql += ")";
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return relays;
  }
  // Check that the expected amount of data was found
  if(dataList.size() < schemaPositionList.size()){
    QStringList missingRelays;
    QString missingRelaysStr;
    QVariant id;
    // Search missing relays
    for(const auto & sp : schemaPositionList){
      id = getTestNodeRouteRelayIdBySchemaPosition(testNodeId, sp, ok);
      if(!ok){
        return relays;
      }
      if(id.isNull()){
        missingRelays.append(sp);
      }
    }
    // Build missing relays string
    for(int i = 0; i < missingRelays.size(); ++i){
      if(i > 0){
        missingRelaysStr += ", ";
      }
      missingRelaysStr += missingRelays.at(i);
    }
    // Build error
    pvLastError.setError(tr("Some requested relays not exist in database."), mdtError::Error);
    pvLastError.setInformativeText(tr("Missing relays are: ") + missingRelaysStr);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return relays;
  }
  Q_ASSERT(dataList.size() == schemaPositionList.size());
  // Build relays list and return it
  for(const auto & record : dataList){
    relays.emplace_back(record.value(0).toInt(), record.value(1).toString(), record.value(2).toInt());
  }

  return relays;
}

QVariant mdtTtTestNode::getTestNodeRouteRelayIdBySchemaPosition(const QVariant & testNodeId, const QString & schemaPosition, bool & ok)
{
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT U.Id_PK, U.SchemaPosition FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK";
  sql += " WHERE TNU.TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (TNU.Type_Code_FK = 'BUSCPLREL' OR TNU.Type_Code_FK = 'CHANELREL')";
  sql += " AND U.SchemaPosition = '" + schemaPosition + "'";
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return QVariant();
  }
  if(dataList.isEmpty()){
    return QVariant();
  }

  return dataList.at(0).value(0);
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

bool mdtTtTestNode::setTestNodeUnitIoRange(const mdtSqlTableSelection& s, int startIoPosition)
{
  int i;
  mdtSqlRecord record;
  int ioPos;

  // Setup record
  if(!record.addAllFields("TestNodeUnit_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  if(!beginTransaction()){
    return false;
  }
  ioPos = startIoPosition;
  for(i = 0; i < s.rowCount(); ++i){
    Q_ASSERT(!s.data(i, "Unit_Id_FK_PK").isNull());
    record.setValue("IoPosition", ioPos);
    if(!updateRecord("TestNodeUnit_tbl", record, "Unit_Id_FK_PK", s.data(i, "Unit_Id_FK_PK"))){
      rollbackTransaction();
      return false;
    }
    ++ioPos;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestNode::addRelaysToGraph(const QVariant& testNodeId, mdtClPathGraph& graph, const QList< QVariant >& relaysToIgnoreIdList)
{
  /**
  struct relay_t{
    QVariant id;
    QVariant cnxA;
    QVariant cnxB;
  } relay;
  */
  QString sql;
  QList<QVariant> testNodeUnitIdList;
  ///QPair<QVariant, QVariant> connections;
  ///QList<relay_t> relaysList;
  bool ok;
  int i;

  // Clear previous added relays
  graph.removeAddedLinks();
  // Get coupling relays
  sql = "SELECT Unit_Id_FK_PK FROM TestNodeUnit_tbl";
  sql += " WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL' OR Type_Code_FK = 'CHANELREL')";
  testNodeUnitIdList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < relaysToIgnoreIdList.size(); ++i){
    Q_ASSERT(!relaysToIgnoreIdList.at(i).isNull());
    testNodeUnitIdList.removeAll(relaysToIgnoreIdList.at(i));
  }

  return addRelayListToGraphPv(testNodeUnitIdList, graph, 100);
  /**
  // Build relays list
  for(i = 0; i < testNodeUnitIdList.size(); ++i){
    // Get connections
    connections = getTwoRelayConnections(testNodeUnitIdList.at(i), graph, ok);
    if(!ok){
      return false;
    }
    relay.id = testNodeUnitIdList.at(i);
    relay.cnxA = connections.first;
    relay.cnxB = connections.second;
    relaysList.append(relay);
  }
  // Add relays to graph
  for(i = 0; i < relaysList.size(); ++i){
    relay = relaysList.at(i);
    graph.addLink(relay.cnxA, relay.cnxB, relay.id, true, 100);
  }

  return true;
  */
}

QList<mdtTtTestNodeUnitSetupData> mdtTtTestNode::getRelayPathSetupDataList(const QList<QVariant> & relayPathConnectionIdList, mdtClPathGraph & graph, bool & ok)
{
  mdtTtTestNodeUnitSetupData setupData;
  QList<mdtTtTestNodeUnitSetupData> setupDataList;
  QVariant testNodeUnitId;
  int i;

  // Setup data
  if(!setupData.setup(database())){
    ok = false;
    return setupDataList;
  }
  // Process..
  if(relayPathConnectionIdList.size() < 2){
    ok = false;
    pvLastError.setError(tr("Given relays path is not valid (contains less than 2 connections)."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return setupDataList;
  }
  for(i = 1; i < relayPathConnectionIdList.size(); ++i){
    Q_ASSERT(!relayPathConnectionIdList.at(i-1).isNull());
    Q_ASSERT(!relayPathConnectionIdList.at(i).isNull());
    testNodeUnitId = graph.getUserData(relayPathConnectionIdList.at(i-1), relayPathConnectionIdList.at(i));
    if(!testNodeUnitId.isNull()){
      setupData.clearValues();
      setupData.setValue("TestNodeUnit_Id_FK", testNodeUnitId);
      setupData.setValue("State", true);
      setupData.setValue("StepNumber", 0);
      setupDataList.append(setupData);
    }
  }

  return setupDataList;
}

bool mdtTtTestNode::makesShortCircuit(const QVariant& connectionIdA, const QVariant& connectionIdB, const std::vector<mdtTtTestNodeRouteRelay> & relays, mdtClPathGraph& graph, bool & ok)
{
  Q_ASSERT(!connectionIdA.isNull());
  Q_ASSERT(!connectionIdB.isNull());

  QList<QVariant> testNodeUnitIdList;

  // Clear previous results
  graph.removeAddedLinks();
  // Add relays of given node units to graph
  for(const auto & relay : relays){
    testNodeUnitIdList.append(relay.id);
  }
  if(!addRelayListToGraphPv(testNodeUnitIdList, graph, 2)){
    ok = false;
    return false;
  }
  ok = true;

  return graph.connectionsAreLinked(connectionIdA, connectionIdB);
}

bool mdtTtTestNode::ensureAbsenceOfShortCircuit(const QVariant & connectionIdA, const QVariant & connectionIdB, const QList<mdtTtTestNodeUnitSetupData> & testNodeUnitSetupDataList, mdtClPathGraph & graph, bool & ok)
{
  Q_ASSERT(!connectionIdA.isNull());
  Q_ASSERT(!connectionIdB.isNull());

  ///QList<QVariant> connectionIdList;
  int i;
  bool hasShort;
  /**
  struct relay_t{
    QVariant id;
    QVariant cnxA;
    QVariant cnxB;
  } relay;
  QList<relay_t> relaysList;
  QPair<QVariant, QVariant> connections;
  */
  ///QVariant testNodeUnitId;
  QList<QVariant> testNodeUnitIdList;

  // Clear previous results
  graph.removeAddedLinks();
  // Add relays of given node units to graph
  for(i = 0; i < testNodeUnitSetupDataList.size(); ++i){
    testNodeUnitIdList << testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK");
  }
  if(!addRelayListToGraphPv(testNodeUnitIdList, graph, 2)){
    return false;
  }
  /**
  for(i = 0; i < testNodeUnitSetupDataList.size(); ++i){
    testNodeUnitId = testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK");
    Q_ASSERT(!testNodeUnitId.isNull());
    if(!addRelayToGraphPv(testNodeUnitId, graph, 2)){
      return false;
    }
  }
  */
  /**
  // Build relays list with only given node units
  for(i = 0; i < testNodeUnitSetupDataList.size(); ++i){
    testNodeUnitId = testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK");
    Q_ASSERT(!testNodeUnitId.isNull());
    // Get connections
    connections = getTwoRelayConnections(testNodeUnitId, graph, ok);
    if(!ok){
      return false;
    }
    relay.id = testNodeUnitId;
    relay.cnxA = connections.first;
    relay.cnxB = connections.second;
    relaysList.append(relay);
  }
  // Add relays to graph
  for(i = 0; i < relaysList.size(); ++i){
    relay = relaysList.at(i);
    graph.addLink(relay.cnxA, relay.cnxB, relay.id, true, 2);
  }
  */
  // Check about shorts and reset graph
  hasShort = graph.connectionsAreLinked(connectionIdA, connectionIdB);
  graph.removeAddedLinks();
  if(hasShort){
    QString sql;
    QList<QSqlRecord> dataList;
    QSqlRecord data;
    QString u1, u2, relaysStr;
    QString msg;
    int lastIndex;
    // Infos about connectionIdA
    sql = "SELECT U.SchemaPosition, UCNX.UnitContactName FROM UnitConnection_tbl UCNX JOIN Unit_tbl U ON U.Id_PK = UCNX.Unit_Id_FK";
    sql += " WHERE UCNX.Id_PK = " + connectionIdA.toString();
    dataList = getDataList<QSqlRecord>(sql, ok);
    if(!ok){
      return false;
    }
    Q_ASSERT(dataList.size() == 1);
    data = dataList.at(0);
    u1 = tr("pos. ") + data.value("SchemaPosition").toString() + tr(" connection ") + data.value("UnitContactName").toString();
    // Infos about connectionIdB
    sql = "SELECT U.SchemaPosition, UCNX.UnitContactName FROM UnitConnection_tbl UCNX JOIN Unit_tbl U ON U.Id_PK = UCNX.Unit_Id_FK";
    sql += " WHERE UCNX.Id_PK = " + connectionIdB.toString();
    dataList = getDataList<QSqlRecord>(sql, ok);
    if(!ok){
      return false;
    }
    Q_ASSERT(dataList.size() == 1);
    data = dataList.at(0);
    u2 = tr("pos. ") + data.value("SchemaPosition").toString() + tr(" connection ") + data.value("UnitContactName").toString();
    // Infos about used relays
    sql = "SELECT SchemaPosition FROM Unit_tbl WHERE Id_PK IN (";
    lastIndex = testNodeUnitSetupDataList.size() - 1;
    for(i = 0; i < lastIndex; ++i){
      sql += testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK").toString() + ",";
    }
    sql += testNodeUnitSetupDataList.at(lastIndex).value("TestNodeUnit_Id_FK").toString() + ")";
    /**
    for(i = 0; i < (relaysList.size()-1); ++i){
      sql += relaysList.at(i).id.toString() + ",";
    }
    if(i < relaysList.size()){
      sql += relaysList.at(i).id.toString();
    }
    sql += ")";
    */
    dataList = getDataList<QSqlRecord>(sql, ok);
    if(!ok){
      return false;
    }
    for(i = 0; i < dataList.size(); ++i){
      relaysStr += dataList.at(i).value("SchemaPosition").toString() + " ";
    }
    // Form message
    msg = tr("A generated setup makes a internal short between measure connections.") + " ";
    msg += tr("Short is between ") + u1 + tr(" and ") + u2 + ". ";
    msg += tr("Route relays: ") + relaysStr;
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
    pvLastError.commit();
    return false;
  }

  return true;
}

QList< mdtClLinkData > mdtTtTestNode::getLinkDataListForPath(const QVariant& connectionIdA, const QVariant& connectionIdB,
                                                             const QList< mdtTtTestNodeUnitSetupData >& testNodeUnitSetupDataList,
                                                             mdtClPathGraph& graph, bool& ok)
{
  Q_ASSERT(!connectionIdA.isNull());
  Q_ASSERT(!connectionIdB.isNull());

  mdtClLink lnk(database());
  QList<QVariant> connectionIdList;
  ///QVariant startConnectionId;
  ///QVariant endConnectionId;
  mdtClLinkPkData linkPk;
  mdtClLinkData linkData;
  QList<mdtClLinkData> linkDataList;
  QList<QVariant> testNodeUnitIdList;
  int i;
  bool exists;

  // Clear previous results
  graph.removeAddedLinks();
  // Add relays of given node units to graph
  for(i = 0; i < testNodeUnitSetupDataList.size(); ++i){
    testNodeUnitIdList << testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK");
  }
  if(!addRelayListToGraphPv(testNodeUnitIdList, graph, 2)){
    ok = false;
    return linkDataList;
  }
  /**
  for(i = 0; i < testNodeUnitSetupDataList.size(); ++i){
    testNodeUnitId = testNodeUnitSetupDataList.at(i).value("TestNodeUnit_Id_FK");
    Q_ASSERT(!testNodeUnitId.isNull());
    if(!addRelayToGraphPv(testNodeUnitId, graph, 2)){
      ok = false;
      return linkDataList;
    }
  }
  */
  // Get shortest path
  connectionIdList = graph.getShortestPath(connectionIdA, connectionIdB, ok);
  if(!ok){
    pvLastError = graph.lastError();
    return linkDataList;
  }
  /*
   * Get link data from Link_tbl
   * If 2 connection not exists in table,
   *  they are "virtual" links representing relay contact,
   *  i.e. those added previously with addRelayToGraphPv().
   */
  for(i = 1; i < connectionIdList.size(); ++i){
    linkPk.connectionStartId = connectionIdList.at(i-1);
    linkPk.connectionEndId = connectionIdList.at(i);
    exists = lnk.linkExists(linkPk, ok);
//     startConnectionId = connectionIdList.at(i-1);
//     endConnectionId = connectionIdList.at(i);
//     exists = lnk.linkExists(startConnectionId, endConnectionId, ok);
    if(!ok){
      pvLastError = lnk.lastError();
      return linkDataList;
    }
    if(!exists){
      linkPk.connectionStartId = connectionIdList.at(i);
      linkPk.connectionEndId = connectionIdList.at(i-1);
      exists = lnk.linkExists(linkPk, ok);
//       startConnectionId = connectionIdList.at(i);
//       endConnectionId = connectionIdList.at(i-1);
//       exists = lnk.linkExists(startConnectionId, endConnectionId, ok);
      if(!ok){
        pvLastError = lnk.lastError();
        return linkDataList;
      }
    }
    if(exists){
      linkData = lnk.getLinkData(linkPk, ok);
//       linkData = lnk.getLinkData(startConnectionId, endConnectionId, true, false, ok);
      if(!ok){
        pvLastError = lnk.lastError();
        return linkDataList;
      }
      linkDataList.append(linkData);
    }
  }
  // Remove added relays from graph
  graph.removeAddedLinks();

  return linkDataList;
}

QPair<QVariant, QVariant> mdtTtTestNode::getTwoRelayConnections(const QVariant & testNodeUnitId, mdtClPathGraph & graph, bool & ok)
{
  Q_ASSERT(!testNodeUnitId.isNull());

  QString sql;
  QList<QVariant> connectionsList;
  int i, m;

  // Get all connections of given relay
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + testNodeUnitId.toString();
  connectionsList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QPair<QVariant, QVariant>();
  }
  // Check any combinaison of connections to find 2 non linked together connections
  for(i = 0; i < (connectionsList.size()-1); ++i){
    for(m = i+1; m < connectionsList.size(); ++m){
      if(!graph.connectionsAreLinked(connectionsList.at(i), connectionsList.at(m))){
        ok = true;
        return QPair<QVariant, QVariant>(connectionsList.at(i), connectionsList.at(m));
      }
    }
  }
  // Given relay has not 2 non linked connections
  QList<QVariant> dataList;
  QString msg;
  sql = "SELECT SchemaPosition FROM Unit_tbl WHERE Id_PK = " + testNodeUnitId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QPair<QVariant, QVariant>();
  }
  Q_ASSERT(dataList.size() == 1);
  ok = false;
  msg = tr("Could not find 2 connections for relay ") + dataList.at(0).toString();
  pvLastError.setError(msg, mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNode");
  pvLastError.commit();

  return QPair<QVariant, QVariant>();
}

bool mdtTtTestNode::addRelayListToGraphPv(const QList<QVariant> & testNodeUnitIdList, mdtClPathGraph & graph, int relayCost)
{
  int i;
  QPair<QVariant, QVariant> connections;
  QVector<QPair<QVariant, QVariant> > connectionsList;
  bool ok;

  /*
   * First version of this method added relays one after the other
   * with a helper method.
   * By doing so, problem occured with getTwoRelayConnections()
   * because a loop existed outside a relay, so the function could
   * not find 2 not linked connections of the relay.
   *
   * To fix this problem, we build relays connections list first,
   * then add them to graph.
   */
  // Build relays connections list
  for(i = 0; i < testNodeUnitIdList.size(); ++i){
    connections = getTwoRelayConnections(testNodeUnitIdList.at(i), graph, ok);
    if(!ok){
      return false;
    }
    connectionsList.append(connections);
  }
  // Add relays connections to graph
  Q_ASSERT(connectionsList.size() == testNodeUnitIdList.size());
  for(i = 0; i < connectionsList.size(); ++i){
    connections = connectionsList.at(i);
    graph.addLink(connections.first, connections.second, testNodeUnitIdList.at(i), true, relayCost);
  }
  /**
  // Build relays list with only given node units
  for(i = 0; i < testNodeUnitIdList.size(); ++i){
    if(!addRelayToGraphPv(testNodeUnitIdList.at(i), graph, relayCost)){
      return false;
    }
  }
  */

  return true;
}
