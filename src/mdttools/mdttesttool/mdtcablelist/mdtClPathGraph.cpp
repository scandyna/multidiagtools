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
#include "mdtClPathGraph.h"
#include "mdtClPathGraphicsConnection.h"
#include "mdtClPathGraphicsLink.h"
#include "mdtClUnit.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QString>
#include <QModelIndex>
#include <QObject>

//#include <QDebug>

using namespace mdtClPathGraphPrivate;

/*
 * Visitor class implementation
 */

mdtClPathGraphVisitor::mdtClPathGraphVisitor()
{
  pvEdgeQueue = 0;
}

mdtClPathGraphVisitor::mdtClPathGraphVisitor(QQueue<mdtClPathGraphEdgeData> *edgeQueue)
{
  Q_ASSERT(edgeQueue != 0);

  pvEdgeQueue = edgeQueue;
}

void mdtClPathGraphVisitor::examine_edge(edge_t e, const graph_t &g)
{
  Q_ASSERT(pvEdgeQueue != 0);

  mdtClPathGraphEdgeData edgeData = g[e];
  pvEdgeQueue->enqueue(edgeData);
}


/*
 * mdtClPathGraph implementation
 */

mdtClPathGraph::mdtClPathGraph(QSqlDatabase db)
{
  pvDatabase = db;
  pvLinkListModel = new QSqlQueryModel;
  pvGraphicsScene = new QGraphicsScene;
}

mdtClPathGraph::~mdtClPathGraph()
{
  delete pvLinkListModel;
  delete pvGraphicsScene;
}

bool mdtClPathGraph::loadLinkList() 
{
  QString sql;
  QSqlError sqlError;
  int row;
  QModelIndex index;
  QVariant data;
  mdtClPathGraphEdgeData edgeData;
  vertex_t startVertex, endVertex;
  edge_t edge;
  int startConnectionId, endConnectionId;

  // Clear previous results
  pvGraph.clear();
  pvGraphVertices.clear();
  // Run query to get start and end connection IDs
  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, LinkDirection_Code_FK FROM LinkList_view";
  pvLinkListModel->setQuery(sql, pvDatabase);
  sqlError = pvLinkListModel->lastError();
  if(sqlError.isValid()){
    pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return false;
  }
  // Build the graph
  for(row = 0; row < pvLinkListModel->rowCount(); ++row){
    // Get start connection ID
    index = pvLinkListModel->index(row, 0);
    data = pvLinkListModel->data(index);
    if(data.isNull()){
      pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
      pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
      pvLastError.setSystemError(-1, QObject::tr("UnitConnectionStart_Id_FK returned a NULL value."));
      MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
      pvLastError.commit();
      return false;
    }
    startConnectionId = data.toInt();
    // Get start vertex or create one if not allready exists
    if(pvGraphVertices.contains(startConnectionId)){
      startVertex = pvGraphVertices.value(startConnectionId);
    }else{
      startVertex = boost::add_vertex(pvGraph);
      pvGraphVertices.insert(startConnectionId, startVertex);
    }
    // Set start ID to edge data
    edgeData.startConnectionId = data;
    // Get end connection ID
    index = pvLinkListModel->index(row, 1);
    data = pvLinkListModel->data(index);
    if(data.isNull()){
      pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
      pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
      pvLastError.setSystemError(-1, QObject::tr("UnitConnectionEnd_Id_FK returned a NULL value."));
      MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
      pvLastError.commit();
      return false;
    }
    endConnectionId = data.toInt();
    // Get end vertex or create one if not allready exists
    if(pvGraphVertices.contains(endConnectionId)){
      endVertex = pvGraphVertices.value(endConnectionId);
    }else{
      endVertex = boost::add_vertex(pvGraph);
      pvGraphVertices.insert(endConnectionId, endVertex);
    }
    // Set end connection ID to edge data
    edgeData.endConnectionId = data;
    // Add edge
    edgeData.isComplement = false;
    boost::add_edge(startVertex, endVertex, edgeData, pvGraph);
    // Add complement edge if link is bidirectinnal
    index = pvLinkListModel->index(row, 2);
    data = pvLinkListModel->data(index);
    if(data.toString() == "BID"){
      edgeData.isComplement = true;
      boost::add_edge(endVertex, startVertex, edgeData, pvGraph);
    }
    // Fetch more data if possible
    if((row > 0)&&(pvLinkListModel->canFetchMore())){
      pvLinkListModel->fetchMore();
    }
  }

  return true;
}

QList<QVariant> mdtClPathGraph::getLinkedConnectionIdList(const QVariant & fromConnectionId)
{
  QList<QVariant> connectionIdList;
  mdtClPathGraphEdgeData edge;
  mdtClPathGraphVisitor visitor(&pvEdgeQueue);
  vertex_t vertex;

  // Check if we have requested connection ID in the graph
  if(!pvGraphVertices.contains(fromConnectionId.toInt())){
    pvLastError.setError(QObject::tr("Cannot get list of linked connections."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Start connection ID not found") + " (ID: " + fromConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return connectionIdList;
  }
  vertex = pvGraphVertices.value(fromConnectionId.toInt());
  // Clear previous results
  pvEdgeQueue.clear();
  // Proceed BFS
  breadth_first_search(pvGraph, vertex, boost::visitor(visitor));
  // Get connections
  while(!pvEdgeQueue.isEmpty()){
    edge = pvEdgeQueue.dequeue();
    Q_ASSERT(!edge.startConnectionId.isNull());
    Q_ASSERT(!edge.endConnectionId.isNull());
    if(!edge.isComplement){
      if((edge.startConnectionId != fromConnectionId)&&(!connectionIdList.contains(edge.startConnectionId))){
        connectionIdList.append(edge.startConnectionId);
      }
      if((edge.endConnectionId != fromConnectionId)&&(!connectionIdList.contains(edge.endConnectionId))){
        connectionIdList.append(edge.endConnectionId);
      }
    }
  }

  return connectionIdList;
}

QList<QVariant> mdtClPathGraph::getLinkedConnectorIdList(const QVariant & fromConnectorId, bool *ok, const QList<QVariant> & connectionIdListToIgnore)
{
  Q_ASSERT(ok != 0);

  mdtClUnit unit(0, pvDatabase);
  QVariant connectorId;
  QList<QVariant> connectorIdList;
  QList<QVariant> fromConnectionIdList;
  QList<QVariant> linkedConnectionIdList;
  int i;
  int k;

  // Get fromConnector's list of connections
  fromConnectionIdList = unit.getConnectionIdListPartOfConnectorId(fromConnectorId, ok);
  if(!*ok){
    pvLastError = unit.lastError();
    return connectorIdList;
  }
  // Remove connectionIdListToIgnore in fromConnectionIdList
  for(i = 0; i < fromConnectionIdList.size(); ++i){
    if(connectionIdListToIgnore.contains(fromConnectionIdList.at(i))){
      fromConnectionIdList.removeAt(i);
    }
  }
  // Add linked connectors for each connection
  for(i = 0; i < fromConnectionIdList.size(); ++i){
    // Get linked connections
    linkedConnectionIdList = getLinkedConnectionIdList(fromConnectionIdList.at(i));
    for(k = 0; k < linkedConnectionIdList.size(); ++k){
      // Get connector ID
      connectorId = unit.getConnectorIdOfConnectionId(linkedConnectionIdList.at(k), ok);
      if(!*ok){
        pvLastError = unit.lastError();
        return connectorIdList;
      }
      // Add connector ID to result
      if((!connectorId.isNull())&&(!connectorIdList.contains(connectorId))){
        connectorIdList.append(connectorId);
      }
    }
  }

  return connectorIdList;
}

bool mdtClPathGraph::drawPath(const QVariant & fromConnectionId)
{
  mdtClPathGraphEdgeData edge;
  mdtClPathGraphicsConnection *startConnection, *endConnection;
  mdtClPathGraphicsLink *link;
  int startConnectionId, endConnectionId;
  mdtClPathGraphVisitor visitor(&pvEdgeQueue);
  vertex_t vertex;

  // Check if we have requested connection ID in the graph
  if(!pvGraphVertices.contains(fromConnectionId.toInt())){
    pvLastError.setError(QObject::tr("Cannot draw requested path."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Start connection ID not found") + " (ID: " + fromConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return false;
  }
  vertex = pvGraphVertices.value(fromConnectionId.toInt());
  // Clear previous results
  pvEdgeQueue.clear();
  pvDrawnConnections.clear();
  pvGraphicsScene->clear();
  // Proceed BFS
  breadth_first_search(pvGraph, vertex, boost::visitor(visitor));
  // draw ...
  while(!pvEdgeQueue.isEmpty()){
    edge = pvEdgeQueue.dequeue();
    Q_ASSERT(!edge.startConnectionId.isNull());
    Q_ASSERT(!edge.endConnectionId.isNull());
    if(!edge.isComplement){
      // Try to get startConnection
      startConnectionId = edge.startConnectionId.toInt();
      startConnection = pvDrawnConnections.value(startConnectionId, 0);
      // Try to get end connection
      endConnectionId = edge.endConnectionId.toInt();
      endConnection = pvDrawnConnections.value(endConnectionId, 0);
      // Create missing connections
      if((startConnection == 0)&&(endConnection == 0)){
        startConnection = newConnectionItem(startConnectionId, 0, false);
        endConnection = newConnectionItem(endConnectionId, startConnection, false);
      }else if(startConnection == 0){
        Q_ASSERT(endConnection != 0);
        startConnection = newConnectionItem(startConnectionId, endConnection, false);
      }else{
        Q_ASSERT(startConnection != 0);
        endConnection = newConnectionItem(endConnectionId, startConnection, false);
      }
      Q_ASSERT(startConnection != 0);
      Q_ASSERT(endConnection != 0);
      // Draw link
      link = new mdtClPathGraphicsLink(startConnection, endConnection);
      pvGraphicsScene->addItem(link);
      // Set connections and link data
      setGraphicsItemsData(startConnection, endConnection, link, startConnectionId, endConnectionId);
    }
  }

  return true;
}

void mdtClPathGraph::attachView(QGraphicsView *view) 
{
  Q_ASSERT(view != 0);

  view->setScene(pvGraphicsScene);
}

QStringList mdtClPathGraph::lastErrorMessage() const
{
  QStringList errorList;

  errorList << pvLastError.text() << pvLastError.informativeText() << pvLastError.systemText();
  Q_ASSERT(errorList.size() == 3);
  return errorList;
}

mdtError mdtClPathGraph::lastError() const
{
  return pvLastError;
}

mdtClPathGraphicsConnection *mdtClPathGraph::newConnectionItem(int id, mdtClPathGraphicsConnection *itemForNextPos, bool reverse)
{
  mdtClPathGraphicsConnection *item;

  item = new mdtClPathGraphicsConnection;
  if(itemForNextPos != 0){
    item->setPos(itemForNextPos->nextPos(reverse));
  }else{
    item->setPos(0.0, 0.0);
  }
  pvGraphicsScene->addItem(item);
  pvDrawnConnections.insert(id, item);

  return item;
}

bool mdtClPathGraph::setGraphicsItemsData(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection, mdtClPathGraphicsLink *link, int startConnectionId, int endConnectionId)
{
  Q_ASSERT(startConnection != 0);
  Q_ASSERT(endConnection != 0);
  Q_ASSERT(link != 0);

  QString sql;
  QSqlError sqlError; 
  QVariant data;
  QSqlQuery query(pvDatabase);
  QSqlRecord record;
  QString str;

  sql = "SELECT StartVehicleType, StartVehicleSubType, StartCabinet, StartSchemaPosition, StartUnitConnectorName, StartUnitContactName, StartConnectionType_Code_FK, "\
        "EndVehicleType, EndVehicleSubType, EndCabinet, EndSchemaPosition, EndUnitConnectorName, EndUnitContactName, EndConnectionType_Code_FK, Identification, "\
        "LinkType_Code_FK "\
        "FROM LinkList_view ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + QString::number(startConnectionId);
  sql += " AND UnitConnectionEnd_Id_FK = " + QString::number(endConnectionId);
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError(QObject::tr("Cannot get connections informations from database."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    startConnection->setText("??");
    endConnection->setText("??");
    return false;
  }
  query.next();
  record = query.record();
  // Set start data
  str = record.value("StartVehicleType").toString() + "\n";
  str += record.value("StartVehicleSubType").toString() + "\n";
  str += record.value("StartCabinet").toString() + "\n";
  str += record.value("StartSchemaPosition").toString() + "\n";
  str += record.value("StartUnitConnectorName").toString() + ";";
  str += record.value("StartUnitContactName").toString();
  startConnection->setText(str);
  // Set start connection type
  str = record.value("StartConnectionType_Code_FK").toString();
  if(str == "P"){
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Pin);
  }else if(str == "S"){
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Socket);
  }else{
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Terminal);
  }
  // Set end data
  str = record.value("EndVehicleType").toString() + "\n";
  str += record.value("EndVehicleSubType").toString() + "\n";
  str += record.value("EndCabinet").toString() + "\n";
  str += record.value("EndSchemaPosition").toString() + "\n";
  str += record.value("EndUnitConnectorName").toString() + ";";
  str += record.value("EndUnitContactName").toString();
  endConnection->setText(str);
  // Set end connection type
  str = record.value("EndConnectionType_Code_FK").toString();
  if(str == "P"){
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Pin);
  }else if(str == "S"){
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Socket);
  }else{
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Terminal);
  }
  // Set link data
  str = record.value("Identification").toString();
  link->setText(str);
  str = record.value("LinkType_Code_FK").toString();
  if(str == "CONNECTION"){
    link->setLinkType(mdtClPathGraphicsLink::Connection);
  }else{
    link->setLinkType(mdtClPathGraphicsLink::CableLink);
  }

  return true;
}
