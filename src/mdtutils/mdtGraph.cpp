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
#include "mdtGraph.h"
#include "mdtGraphVertex.h"
#include <QQueue>

mdtGraph::mdtGraph(bool autoDeleteData)
{
  pvAutoDeleteData = autoDeleteData;
  pvEdgeCount = 0;
}

mdtGraph::~mdtGraph()
{
  if(pvAutoDeleteData){
    deleteData();
  }
  qDeleteAll(pvVertexList);
}

void mdtGraph::setAutoDeleteData(bool autoDelete)
{
  pvAutoDeleteData = autoDelete;
}

bool mdtGraph::autoDeleteData() const
{
  return pvAutoDeleteData;
}

void mdtGraph::deleteData()
{
  int i;

  for(i = 0; i < pvVertexList.size(); ++i){
    Q_ASSERT(pvVertexList.at(i) != 0);
    Q_ASSERT(pvVertexList.at(i)->data() != 0);
    delete pvVertexList.at(i)->data();
  }
}

bool mdtGraph::insertVertex(mdtGraphVertexData *vertexData)
{
  Q_ASSERT(vertexData != 0);

  mdtGraphVertex *v;

  // Multiple key is not allowed
  if(containsVertex(vertexData->key())){
    return false;
  }
  // Create a new vertex, set his data and isert it to graph
  v = new mdtGraphVertex;
  if(v == 0){
    return false;
  }
  v->setData(vertexData);
  pvVertexList.append(v);

  return true;
}

bool mdtGraph::insertVertex(const QVariant &key, const QVariant &data)
{
  mdtGraphVertex *v;
  mdtGraphVertexData *d;

  // Multiple key is not allowed
  if(containsVertex(key)){
    return false;
  }
  // Create a data object and set key and data
  d = new mdtGraphVertexData;
  if(d == 0){
    return false;
  }
  d->setKey(key);
  d->setData(data);
  // Create a new vertex, set his data and isert it to graph
  v = new mdtGraphVertex;
  if(v == 0){
    delete d;
    return false;
  }
  v->setData(d);
  pvVertexList.append(v);

  return true;
}

bool mdtGraph::removeVertex(const QVariant &key)
{
  int i;
  int vToRemoveIndex = -1;
  mdtGraphVertex *v;

  // Search the vertex that contains data with given key
  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->key() == key){
      // If vertex contains other vertices in his adjacency list, we fail
      if(v->hasAdjacent()){
        return false;
      }
      // Remember index of vertex to remove
      vToRemoveIndex = i;
    }else{
      // If searched vertex is contained in the adjacency list of another vertex, we fail
      if(v->containsAdjacent(key)){
        return false;
      }
    }
  }
  // If vertex was not found, tell this
  if(vToRemoveIndex < 0){
    return false;
  }
  // Suppression du sommet
  // Remove vertex
  v = pvVertexList.at(vToRemoveIndex);
  pvVertexList.removeAt(vToRemoveIndex);
  if(pvAutoDeleteData){
    delete v->data();
  }
  delete v;

  return true;
}

bool mdtGraph::insertEdge(const QVariant &keyFrom, const QVariant &keyTo)
{
  int i;
  mdtGraphVertex *vFrom, *vTo;
  bool found;

  // Check that vertices referenced by both keys exists
  found = false;
  vFrom = 0;
  for(i = 0; i < pvVertexList.size(); ++i){
    vFrom = pvVertexList.at(i);
    Q_ASSERT(vFrom != 0);
    if(vFrom->key() == keyFrom){
      found = true;
      break;
    }
  }
  if(!found){
    return false;
  }
  found = false;
  vTo = 0;
  for(i = 0; i < pvVertexList.size(); ++i){
    vTo = pvVertexList.at(i);
    Q_ASSERT(vTo != 0);
    if(vTo->key() == keyTo){
      found = true;
      break;
    }
  }
  if(!found){
    return false;
  }
  Q_ASSERT(vFrom != 0);
  Q_ASSERT(vTo != 0);
  // Insert vertex referenced by keyTo into adjacency list of vertex referenced by keyFrom
  if(!vFrom->addAdjacent(vTo)){
    return false;
  }
  // Update edge count
  pvEdgeCount++;

  return true;
}

bool mdtGraph::removeEdge(const QVariant &keyFrom, const QVariant &keyTo)
{
  int i;
  mdtGraphVertex *v;

  // Search vertex referenced by keyFrom
  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->key() == keyFrom){
      if(v->removeAdjacent(keyTo)){
        pvEdgeCount--;
        return true;
      }else{
        return false;
      }
    }
  }

  return false;
}

bool mdtGraph::containsVertex(const QVariant &key) const
{
  int i;

  for(i = 0; i < pvVertexList.size(); ++i){
    Q_ASSERT(pvVertexList.at(i) != 0);
    if(pvVertexList.at(i)->key() == key){
      return true;
    }
  }

  return false;
}

mdtGraphVertexData *mdtGraph::vertexData(const QVariant & key)
{
  int i;
  mdtGraphVertex *v;

  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->key() == key){
      return v->data();
    }
  }

  return 0;
}

const QList<mdtGraphVertexData*> mdtGraph::adjacencyDataList(const QVariant & key) const
{
  int i, j;
  mdtGraphVertex *v, *va;
  QList<mdtGraphVertex*> adjList;
  QList<mdtGraphVertexData*> list;

  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->key() == key){
      adjList = v->adjacencyList();
      for(j = 0; j < adjList.size(); ++j){
        va = adjList.at(j);
        Q_ASSERT(va != 0);
        Q_ASSERT(va->data() != 0);
        list.append(va->data());
      }
    }
  }

  return list;
}

int mdtGraph::vertexCount() const
{
  return pvVertexList.size();
}

int mdtGraph::edgeCount() const
{
  return pvEdgeCount;
}

QList<mdtGraphVertexData*> mdtGraph::bfs(const QVariant &keyStart)
{
  QQueue<mdtGraphVertex*> fifo;
  QList<mdtGraphVertex*> adjList;
  mdtGraphVertex *v, *av;
  QList<mdtGraphVertexData*> result;
  int i;

  // Init all vertices
  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->key() == keyStart){
      // We have the start vertex. Set it Gray and put it to fifo
      v->setColor(mdtGraphVertex::Gray);
      fifo.enqueue(v);
    }else{
      v->setColor(mdtGraphVertex::White);
    }
  }
  // Search ...
  while(!fifo.isEmpty()){
    v = fifo.dequeue();
    Q_ASSERT(v != 0);
    // Travel all adjacent vertices to current vertex
    adjList = v->adjacencyList();
    for(i = 0; i < adjList.size(); ++i){
      av = adjList.at(i);
      Q_ASSERT(av != 0);
      // If adjacent vertex is white, we enqueue it and set it gray
      if(av->currentColor() == mdtGraphVertex::White){
        av->setColor(mdtGraphVertex::Gray);
        fifo.enqueue(av);
      }
    }
    // We explored all adjacent vertices of current vertex, set it black
    v->setColor(mdtGraphVertex::Black);
    Q_ASSERT(v->data() != 0);
    result.append(v->data());
  }

  return result;
}

QString mdtGraph::dataListDump(const QList<mdtGraphVertexData*> list) const
{
  QString s = "\n";
  int i;

  for(i = 0; i < list.size(); ++i){
    Q_ASSERT(list.at(i) != 0);
    s += "(" + list.at(i)->key().toString() + ")-";
  }

  return s;
}

QString mdtGraph::graphDump() const
{
  QString s = "\n";
  int i, j;
  mdtGraphVertex *v, *av;
  QList<mdtGraphVertex*> adjList;

  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    s += "(" + v->key().toString() + ")\n Adjacent vertices: ";
    adjList = v->adjacencyList();
    for(j = 0; j < adjList.size(); ++j){
      av = adjList.at(j);
      s += "(" + av->key().toString() + ") ";
    }
    s += "\n";
  }

  return s;
}
