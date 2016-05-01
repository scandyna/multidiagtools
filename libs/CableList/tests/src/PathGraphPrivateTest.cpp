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
#include "PathGraphPrivateTest.h"
#include "mdtApplication.h"

#include "Mdt/CableList/Path/GraphPrivate/Graph.h"
#include <vector>

// static void escape(void *p)
// {
//   asm volatile("" : : "g"(p) : "memory");
// }
// static void clobber()
// {
//   asm volatile("" : : : "memory");
// }

void PathGraphPrivateTest::connectionTest()
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

void PathGraphPrivateTest::linkTest()
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

void PathGraphPrivateTest::graphImplVerticesTest()
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

void PathGraphPrivateTest::graphImplVerticesBenchmark()
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

void PathGraphPrivateTest::graphImplEdgesTest()
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

void PathGraphPrivateTest::graphImplEdgesBenchmark()
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

void PathGraphPrivateTest::graphImplLinkedConnectionsTest()
{
  using Mdt::CableList::Path::GraphPrivate::Connection;
  using Mdt::CableList::Path::GraphPrivate::Link;

  Mdt::CableList::Path::GraphPrivate::GraphImpl graph;
  std::vector<Connection> connections;

  /*
   * Check with this graph:
   * (1)->(2)->(3)
   */
  // Build the graph
  Link link12(Connection(1), Connection(2), 0, false);
  graph.addEdge(link12);
  Link link23(Connection(2), Connection(3), 0, false);
  graph.addEdge(link23);
  QCOMPARE(graph.vertexCount(), 3);
  QCOMPARE(graph.edgeCount(), 2);
  // Get linked connections from 1 and check
  connections = graph.getLinkedConnections(Connection(1));
  QCOMPARE((int)connections.size(), 3);
  QCOMPARE((int)connections[0].id, 1);
  QCOMPARE((int)connections[1].id, 2);
  QCOMPARE((int)connections[2].id, 3);
  // Get linked connections from 3 and check
  connections = graph.getLinkedConnections(Connection(3));
  QCOMPARE((int)connections.size(), 1);
  QCOMPARE((int)connections[0].id, 3);
  /*
   * Check with this graph (same as previous, but undirected):
   * (1)-(2)-(3)
   */
  // Add complements to graph
  Link link21(Connection(2), Connection(1), 0, false);
  graph.addEdge(link21);
  Link link32(Connection(3), Connection(2), 0, false);
  graph.addEdge(link32);
  QCOMPARE(graph.vertexCount(), 3);
  QCOMPARE(graph.edgeCount(), 4);
  // Get linked connections from 1 and check
  connections = graph.getLinkedConnections(Connection(1));
  QCOMPARE((int)connections.size(), 3);
  QCOMPARE((int)connections[0].id, 1);
  QCOMPARE((int)connections[1].id, 2);
  QCOMPARE((int)connections[2].id, 3);
  // Get linked connections from 3 and check
  connections = graph.getLinkedConnections(Connection(3));
  QCOMPARE((int)connections.size(), 3);
  QCOMPARE((int)connections[0].id, 3);
  QCOMPARE((int)connections[1].id, 2);
  QCOMPARE((int)connections[2].id, 1);
  /*
   * Check with this graph:
   * (1)->(2)->(3)  (4)->(5)
   */
  // Build graph
  graph.clear();
  graph.addEdge(link12);
  graph.addEdge(link23);
  Link link45(Connection(4), Connection(5), 0, false);
  graph.addEdge(link45);
  QCOMPARE(graph.vertexCount(), 5);
  QCOMPARE(graph.edgeCount(), 3);
  // Get linked connections from 1 and check
  connections = graph.getLinkedConnections(Connection(1));
  QCOMPARE((int)connections.size(), 3);
  QCOMPARE((int)connections[0].id, 1);
  QCOMPARE((int)connections[1].id, 2);
  QCOMPARE((int)connections[2].id, 3);
  // Get linked connections from 4 and check
  connections = graph.getLinkedConnections(Connection(4));
  QCOMPARE((int)connections.size(), 2);
  QCOMPARE((int)connections[0].id, 4);
  QCOMPARE((int)connections[1].id, 5);



}

void PathGraphPrivateTest::graphImplLinkedConnectionsBenchmark()
{

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  PathGraphPrivateTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
