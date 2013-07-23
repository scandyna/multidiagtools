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
#include "mdtGraphVertex.h"
#include "mdtGraphVertexAdjacent.h"
#include "mdtGraphEdgeData.h"

mdtGraphVertex::mdtGraphVertex(bool autoDeleteEdgeData)
{
  pvColor = White;
  pvData = 0;
  pvAutoDeleteEdgeData = autoDeleteEdgeData;
}

mdtGraphVertex::~mdtGraphVertex()
{
  int i;

  if(pvAutoDeleteEdgeData){
    for(i = 0; i < pvAdjacencyList.size(); ++i){
      Q_ASSERT(pvAdjacencyList.at(i) != 0);
      Q_ASSERT(pvAdjacencyList.at(i)->edgeData() != 0);
      delete pvAdjacencyList.at(i)->edgeData();
    }
  }
  qDeleteAll(pvAdjacencyList);
}

void mdtGraphVertex::setData(mdtGraphVertexData *data)
{
  Q_ASSERT(data != 0);

  pvData = data;
}

mdtGraphVertexData *mdtGraphVertex::data()
{
  return pvData;
}

QVariant mdtGraphVertex::key() const
{
  if(pvData == 0){
    return QVariant();
  }
  return pvData->key();
}

bool mdtGraphVertex::addAdjacent(mdtGraphVertex *v, mdtGraphEdgeData *d)
{
  Q_ASSERT(v != 0);
  Q_ASSERT(d != 0);

  if(containsAdjacent(v->key())){
    return false;
  }
  pvAdjacencyList.append(new mdtGraphVertexAdjacent(v, d));

  return true;
}

bool mdtGraphVertex::removeAdjacent(const QVariant &key)
{
  int i;

  // Search vertex that contains key in adjacency list
  for(i = 0; i < pvAdjacencyList.size(); ++i){
    Q_ASSERT(pvAdjacencyList.at(i) != 0);
    Q_ASSERT(pvAdjacencyList.at(i)->vertex() != 0);
    if(pvAdjacencyList.at(i)->vertex()->key() == key){
      Q_ASSERT(pvAdjacencyList.at(i)->edgeData() != 0);
      if(pvAutoDeleteEdgeData){
        delete pvAdjacencyList.at(i)->edgeData();
      }
      delete pvAdjacencyList.at(i);
      pvAdjacencyList.removeAt(i);
      return true;
    }
  }

  return false;
}

bool mdtGraphVertex::containsAdjacent(const QVariant &key)
{
  int i;

  // Search vertex that contains key in adjacency list
  for(i = 0; i < pvAdjacencyList.size(); ++i){
    Q_ASSERT(pvAdjacencyList.at(i) != 0);
    Q_ASSERT(pvAdjacencyList.at(i)->vertex() != 0);
    if(pvAdjacencyList.at(i)->vertex()->key() == key){
      return true;
    }
  }

  return false;
}

bool mdtGraphVertex::hasAdjacent() const
{
  return (!pvAdjacencyList.isEmpty());
}

int mdtGraphVertex::adjacentCount() const
{
  return pvAdjacencyList.size();
}

const QList<mdtGraphVertexAdjacent*> mdtGraphVertex::adjacencyList() const
{
  return pvAdjacencyList;
}

void mdtGraphVertex::setColor(mdtGraphVertex::color_t color)
{
  pvColor = color;
}

mdtGraphVertex::color_t mdtGraphVertex::currentColor() const
{
  return pvColor;
}
