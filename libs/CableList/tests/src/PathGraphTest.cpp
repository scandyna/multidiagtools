/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "PathGraphTest.h"
#include "mdtApplication.h"
#include "Mdt/CableList/Path/Graph.h"
#include "Mdt/CableList/Path/GraphPrivate/Graph.h"
#include <vector>

void PathGraphTest::privateConnectionTest()
{
  Mdt::CableList::Path::GraphPrivate::Connection cnx(10);
  std::vector<Mdt::CableList::Path::GraphPrivate::Connection> v;

  qDebug() << "sizeof(cnx): " << sizeof(cnx);
  /*
   * Initial state
   */
  QCOMPARE(cnx.id, (int64_t)10);
  /*
   * Usage with a container
   */
  v.emplace_back(1);
  QCOMPARE(v[0].id, (int64_t)1);
}

void PathGraphTest::privateLinkTest()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;

  Mdt::CableList::Path::GraphPrivate::Link link(Connection(1), Connection(2), 0, false);

  qDebug() << "sizeof(link): " << sizeof(link);

  /*
   * Initial state
   */
  QCOMPARE(link.start.id, (int64_t)1);
  QCOMPARE(link.end.id, (int64_t)2);
  QCOMPARE(link.weigth, 0);
  QVERIFY(!link.isComplement);
}

void PathGraphTest::graphImplVerticesTest()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;

  Mdt::CableList::Path::GraphPrivate::GraphImpl graph;
  Mdt::CableList::Path::GraphPrivate::Vertex v;

  /*
   * Initial state
   */
  QCOMPARE(graph.vertexCount(), 0);
  QCOMPARE(graph.getVertexCount(Connection(1)), 0);
  QCOMPARE(graph.edgeCount(), 0);
  /*
   * Add
   */
  v = graph.addVertex(Connection(1));
  QVERIFY(!v.isNull());
  QCOMPARE(graph.vertexCount(), 1);
  QCOMPARE(graph.getVertexCount(Connection(1)), 1);
  // Add the same connection again - Must reuse existing one
  v = graph.addVertex(Connection(1));
  QVERIFY(!v.isNull());
  QCOMPARE(graph.vertexCount(), 1);
  QCOMPARE(graph.getVertexCount(Connection(1)), 1);
  // Add a other connection
  v = graph.addVertex(Connection(2));
  QVERIFY(!v.isNull());
  QCOMPARE(graph.vertexCount(), 2);
  QCOMPARE(graph.getVertexCount(Connection(1)), 1);
  QCOMPARE(graph.getVertexCount(Connection(2)), 1);
  /*
   * Remove...
   */
  
  /*
   * Clear
   */
  graph.clear();
  QCOMPARE(graph.vertexCount(), 0);
  QCOMPARE(graph.edgeCount(), 0);
}

void PathGraphTest::graphImplVerticesBenchmark()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;

  Mdt::CableList::Path::GraphPrivate::GraphImpl graph;
  Mdt::CableList::Path::GraphPrivate::Vertex v;
  int cnx = 0;

  QBENCHMARK{
    v = graph.addVertex(Connection(cnx));
    QVERIFY(!v.isNull());
    ++cnx;
  }
  QCOMPARE(graph.vertexCount(), cnx);
}

void PathGraphTest::graphImplEdgesTest()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;
  using Mdt::CableList::Path::GraphPrivate::Link;
  using Mdt::CableList::Path::GraphPrivate::Edge;

  Mdt::CableList::Path::GraphPrivate::GraphImpl graph;
  Mdt::CableList::Path::GraphPrivate::Vertex v;

  /*
   * Initial state
   */
  QCOMPARE(graph.vertexCount(), 0);
  QCOMPARE(graph.edgeCount(), 0);
  /*
   * Add
   */
  // Add link 1-2
  Link link12(Connection(1), Connection(2), 1, false);
  Edge e12 = graph.addEdge(link12);
  QCOMPARE(graph.edgeCount(), 1);
  // Try to add same link a second time - must do nothing
  Edge e12_2 = graph.addEdge(link12);
  QCOMPARE(graph.edgeCount(), 1);
  QVERIFY(e12 == e12_2);
  // Add link 3-4
  Link link34(Connection(3), Connection(4), 1, false);
  graph.addEdge(link34);
  QCOMPARE(graph.edgeCount(), 2);
  // Add link 4-3 (complement)
  Link link43(Connection(4), Connection(3), 1, true);
  graph.addEdge(link43);
  QCOMPARE(graph.edgeCount(), 3);
  // Add link 2-3
  Link link23(Connection(2), Connection(3), 1, false);
  Edge e23 = graph.addEdge(link23);
  QCOMPARE(graph.edgeCount(), 4);

  /*
   * Remove some edges
   */
  std::vector<Edge> ghostEdges;
  ghostEdges.push_back(e12);
  ghostEdges.push_back(e23);
  for(const auto & edge : ghostEdges){
    graph.removeEdge(edge);
  }
  QCOMPARE(graph.edgeCount(), 2);
  // Vertices are untouched
  QCOMPARE(graph.vertexCount(), 4);
  /*
   * Check reuse of existing vertices
   */
  graph.addEdge(link12);
  graph.addEdge(link23);
  QCOMPARE(graph.edgeCount(), 4);
  QCOMPARE(graph.vertexCount(), 4);
  /*
   * Clear
   */
  graph.clear();
  QCOMPARE(graph.vertexCount(), 0);
  QCOMPARE(graph.edgeCount(), 0);
}

void PathGraphTest::graphImplEdgesBenchmark()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;
  using Mdt::CableList::Path::GraphPrivate::Link;

  Mdt::CableList::Path::GraphPrivate::GraphImpl graph;
  int i = 0;

  QBENCHMARK{
    graph.addEdge(Link(Connection(i), Connection(i+1), 1, false));
    ++i;
  }
  QCOMPARE(graph.edgeCount(), i);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  PathGraphTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
