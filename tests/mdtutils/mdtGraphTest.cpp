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
#include "mdtGraphTest.h"
#include "mdtGraph.h"
#include "mdtGraphVertex.h"
#include "mdtGraphVertexData.h"
#include "mdtGraphEdgeData.h"
#include "mdtApplication.h"
#include <QVariant>
#include <QString>
#include <QList>

void mdtGraphTest::vertexDataTest()
{
  mdtGraphVertexData d;

  // Check defaults
  QCOMPARE(d.key(), QVariant());
  QCOMPARE(d.data(), QVariant());
  // Check simple data access
  d.setData("Data 01");
  QCOMPARE(d.data(), QVariant("Data 01"));
  d.setData("Data 02");
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Check multiple data access
  d.setData("Data A", "FldA");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  d.setData("Data B", "FldB");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  QCOMPARE(d.data("FldB"), QVariant("Data B"));
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Unavailable field
  QCOMPARE(d.data("UnknownFld"), QVariant());
}

void mdtGraphTest::edgeDataTest ()
{
  mdtGraphEdgeData d;

  // Check defaults
  QCOMPARE(d.data(), QVariant());
  // Check simple data access
  d.setData("Data 01");
  QCOMPARE(d.data(), QVariant("Data 01"));
  d.setData("Data 02");
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Check multiple data access
  d.setData("Data A", "FldA");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  d.setData("Data B", "FldB");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  QCOMPARE(d.data("FldB"), QVariant("Data B"));
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Unavailable field
  QCOMPARE(d.data("UnknownFld"), QVariant());
}

void mdtGraphTest::vertexTest()
{
  mdtGraphVertex v, v2;
  mdtGraphVertexData *data1, *data2;

  // Defaults
  QCOMPARE(v.key(), QVariant());
  QVERIFY(v.data() == 0);
  QCOMPARE(v.currentColor(), mdtGraphVertex::White);
  QVERIFY(!v.hasAdjacent());
  QCOMPARE(v.adjacentCount(), 0);
  
  // Set some data
  data1 = new mdtGraphVertexData;
  data1->setKey("K1");
  data1->setData("D1");
  v.setData(data1);
  QCOMPARE(v.key(), QVariant("K1"));
  QVERIFY(v.data() != 0);
  QCOMPARE(v.data()->data(), QVariant("D1"));
  // Add a vertex
  QVERIFY(!v.containsAdjacent("K2"));
  data2 = new mdtGraphVertexData;
  data2->setKey("K2");
  data2->setData("D2");
  v2.setData(data2);
  QVERIFY(v.addAdjacent(&v2));
  QVERIFY(v.hasAdjacent());
  QVERIFY(v.containsAdjacent("K2"));
  QCOMPARE(v.adjacentCount(), 1);
  QVERIFY(!v.addAdjacent(&v2));
  QVERIFY(v.hasAdjacent());
  QVERIFY(v.containsAdjacent("K2"));
  QCOMPARE(v.adjacentCount(), 1);
  // Remove vertex
  QVERIFY(!v.removeAdjacent("KX"));
  QVERIFY(v.hasAdjacent());
  QCOMPARE(v.adjacentCount(), 1);
  QVERIFY(v.removeAdjacent("K2"));
  QVERIFY(!v.containsAdjacent("K2"));
  QVERIFY(!v.hasAdjacent());
  QCOMPARE(v.adjacentCount(), 0);


  delete data1;
  delete data2;
}

void mdtGraphTest::graphTest()
{
  mdtGraph g;
  mdtGraphVertexData *vd1;

  // Defaults
  QVERIFY(g.autoDeleteData());
  QCOMPARE(g.vertexCount(), 0);
  QCOMPARE(g.edgeCount(), 0);
  QVERIFY(!g.containsVertex("K1"));
  QVERIFY(g.vertexData("K1") == 0);
  // Insert data with mdtGraphVertexData API
  vd1 = new mdtGraphVertexData;
  QVERIFY(vd1 != 0);
  vd1->setKey("K1");
  vd1->setData("D1");
  QVERIFY(g.insertVertex(vd1));
  QCOMPARE(g.vertexCount(), 1);
  QVERIFY(g.containsVertex("K1"));
  QVERIFY(!g.insertVertex(vd1));
  QCOMPARE(g.vertexCount(), 1);
  QCOMPARE(g.edgeCount(), 0);
  QVERIFY(g.containsVertex("K1"));
  QVERIFY(g.vertexData("K1") != 0);
  QCOMPARE(g.vertexData("K1")->key(), QVariant("K1"));
  // Insert a vertex with QVariant API
  QVERIFY(g.insertVertex("K2", "D2"));
  QCOMPARE(g.vertexCount(), 2);
  QVERIFY(g.containsVertex("K1"));
  QVERIFY(g.containsVertex("K2"));
  QVERIFY(!g.insertVertex("K2", "D2"));
  QVERIFY(!g.insertVertex("K2", "D2A"));
  QCOMPARE(g.vertexCount(), 2);
  QCOMPARE(g.edgeCount(), 0);
  QVERIFY(g.containsVertex("K1"));
  QVERIFY(g.containsVertex("K2"));
  QVERIFY(g.vertexData("K2") != 0);
  QCOMPARE(g.vertexData("K2")->key(), QVariant("K2"));
  // Remove a vertex
  QVERIFY(g.removeVertex("K1"));
  QCOMPARE(g.vertexCount(), 1);
  QVERIFY(!g.containsVertex("K1"));
  QVERIFY(g.containsVertex("K2"));
  QVERIFY(!g.removeVertex("K1"));
  QCOMPARE(g.vertexCount(), 1);
  QCOMPARE(g.edgeCount(), 0);
  QVERIFY(!g.containsVertex("K1"));
  QVERIFY(g.containsVertex("K2"));
  // Insert a vertex with QVariant API
  QVERIFY(g.insertVertex("K3", "D3"));
  QCOMPARE(g.vertexCount(), 2);
  QVERIFY(g.containsVertex("K2"));
  QVERIFY(g.containsVertex("K3"));
  QCOMPARE(g.edgeCount(), 0);
  // Insert edge K2->K3
  QVERIFY(g.insertEdge("K2", "K3"));
  QCOMPARE(g.vertexCount(), 2);
  QCOMPARE(g.edgeCount(), 1);
  QVERIFY(!g.insertEdge("K2", "K3"));
  QCOMPARE(g.edgeCount(), 1);
  // Insert edge K3->K2
  QVERIFY(g.insertEdge("K3", "K2"));
  QCOMPARE(g.edgeCount(), 2);
  QVERIFY(!g.insertEdge("K3", "K2"));
  QCOMPARE(g.edgeCount(), 2);
  qDebug() << g.graphDump();
  // Remove edge K3->K2
  QVERIFY(g.removeEdge("K3", "K2"));
  QCOMPARE(g.edgeCount(), 1);
  QVERIFY(!g.removeEdge("K3", "K2"));
  QCOMPARE(g.edgeCount(), 1);


  // Display graph
  qDebug() << g.graphDump();

}

void mdtGraphTest::bfsTest()
{
  mdtGraph g;
  QList<mdtGraphVertexData*> l;

  // Insert vertices
  QVERIFY(g.insertVertex("K1", "D1"));
  QVERIFY(g.insertVertex("K2", "D2"));
  QVERIFY(g.insertVertex("K3", "D3"));
  QVERIFY(g.insertVertex("K4", "D4"));
  QVERIFY(g.insertVertex("K5", "D5"));
  
  QVERIFY(g.insertEdge("K1", "K3"));
  QVERIFY(g.insertEdge("K3", "K2"));
  QVERIFY(g.insertEdge("K3", "K5"));
  l = g.bfs("K1");
  qDebug() << g.dataListDump(l);
  
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtGraphTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
