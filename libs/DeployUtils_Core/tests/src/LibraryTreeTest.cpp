/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "LibraryTreeTest.h"
#include "Mdt/DeployUtils/LibraryTree.h"

using namespace Mdt::DeployUtils;

void LibraryTreeTest::initTestCase()
{
}

void LibraryTreeTest::cleanupTestCase()
{
}

/**
 * Sandbox
 */

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <utility>

struct VertexData
{
  VertexData() = default;

  VertexData(const QString & library)
   : libraryName(library)
  {
  }

  QString libraryName;
};

/// \todo see https://stackoverflow.com/questions/2244580/find-boost-bgl-vertex-by-a-key

using Graph = boost::adjacency_list<
  boost::hash_setS,     // OutEdgeList: hash_setS (boost::unordered_set): choosed to avoid adding a edge more than once
  boost::vecS,          // VertexList: vecS (std::vector)
  boost::directedS,     // Directed: undirectedS (directed graph)
  VertexData,           // VertexProperties: we use our custom struct
  boost::no_property,   // EdgeProperties: we use no property
  boost::no_property,   // GraphProperties: no_property (None)
  boost::listS          // EdgeList: listS (std::list)
>;

using LabeledGraph = boost::labeled_graph<Graph, int>;

using Vertex = boost::graph_traits<LabeledGraph>::vertex_descriptor;


void LibraryTreeTest::graphSandbox()
{
  LabeledGraph graph;
  Vertex root = boost::add_vertex(10, VertexData("Root"), graph);
  Vertex a0 = boost::add_vertex(1, VertexData("a0"), graph);
  Vertex b0 = boost::add_vertex(2, VertexData("b0"), graph);
  Vertex c0 = boost::add_vertex(3, VertexData("c0"), graph);
  Vertex d1 = boost::add_vertex(4, VertexData("d1"), graph);
  Vertex e1 = boost::add_vertex(5, VertexData("e1"), graph);
  
  boost::add_edge(10, 1, graph);
  boost::add_edge(10, 2, graph);
  boost::add_edge(10, 3, graph);
  boost::add_edge(a0, 4, graph);
  boost::add_edge(a0, 5, graph);
  
  
  qDebug() << "g[root]: " << graph[10].libraryName;
  qDebug() << "g[a0]: " << graph[1].libraryName;
  qDebug() << "g[b0]: " << graph[2].libraryName;
  qDebug() << "g[c0]: " << graph[3].libraryName;
  qDebug() << "g[d1]: " << graph[4].libraryName;
  qDebug() << "g[e1]: " << graph[5].libraryName;
  
  qDebug() << "Adjacent to root:";
  auto firstEndIt = boost::adjacenct_vertices(10, graph);
  for(auto it = firstEndIt.first; it != firstEndIt.second; ++it){
    qDebug() << boost::get(boost::vertex_bundle, graph)[*it].libraryName;
  }
  
  qDebug() << "Adjacent to a0:";
  firstEndIt = boost::adjacenct_vertices(1, graph);
  for(auto it = firstEndIt.first; it != firstEndIt.second; ++it){
    qDebug() << boost::get(boost::vertex_bundle, graph)[*it].libraryName;
  }
  
  qDebug() << "Vertices count: " << boost::num_vertices(graph);
  qDebug() << "- clear..";
  graph.graph().clear();
  qDebug() << "Vertices count: " << boost::num_vertices(graph);
}

/*
 * Tests
 */

void LibraryTreeTest::treeNodeTest()
{
  /*
   * Default construced
   */
  LibraryTreeNode n0;
  QCOMPARE(n0.id(), 0);
  QVERIFY(n0.isNull());
  /*
   * Non null node
   */
  LibraryTreeNode n1(1);
  QCOMPARE(n1.id(), 1);
  QVERIFY(!n1.isNull());
}

void LibraryTreeTest::treeTest()
{
  /*
   * Initial state
   */
  LibraryTree tree;
  QVERIFY(!tree.containsNode(LibraryTreeNode(1)));
  QVERIFY(tree.rootBinary().isEmpty());
  QVERIFY(tree.toFlatList().isEmpty());
  /*
   * (exe)
   */
  const auto exeNode = tree.setRootBinary("exe");
  QVERIFY(!exeNode.isNull());
  QVERIFY(tree.containsNode(exeNode));
  QCOMPARE(tree.rootBinary(), QString("exe"));
  QVERIFY(tree.getLibraryList(exeNode).isEmpty());
  QVERIFY(tree.toFlatList().isEmpty());
  /*
   *    (exe)
   *   /
   * (a1)
   */
  const auto a1Node = tree.addLibrary("a", exeNode);
  QVERIFY(!a1Node.isNull());
  QCOMPARE(tree.getLibraryList(exeNode), QStringList({"a"}));
  QCOMPARE(tree.toFlatList(), QStringList({"a"}));
  /*
   *    (exe)
   *   /     \
   * (a1)   (b1)
   */
  const auto b1Node = tree.addLibrary("b", exeNode);
  QVERIFY(!b1Node.isNull());
  QCOMPARE( sortedStringListCs(tree.getLibraryList(exeNode)), QStringList({"a","b"}) );
  QCOMPARE( sortedStringListCs(tree.toFlatList()), QStringList({"a","b"}) );
  /*
   *      (exe)
   *     /     \
   *   (a1)   (b1)
   *   /
   * (c2)
   */
  const auto c2Node = tree.addLibrary("c", a1Node);
  QVERIFY(!c2Node.isNull());
  QCOMPARE( sortedStringListCs(tree.getLibraryList(exeNode)), QStringList({"a","b"}) );
  QCOMPARE( sortedStringListCs(tree.getLibraryList(a1Node)), QStringList({"c"}) );
  QCOMPARE( sortedStringListCs(tree.toFlatList()), QStringList({"a","b","c"}) );
  /*
   *      (exe)
   *     /     \
   *   (a1)   (b1)
   *   /      /
   * (c2)   (a2)
   */
  const auto a2Node = tree.addLibrary("a", b1Node);
  QVERIFY(!a2Node.isNull());
  QCOMPARE( sortedStringListCs(tree.getLibraryList(exeNode)), QStringList({"a","b"}) );
  QCOMPARE( sortedStringListCs(tree.getLibraryList(a1Node)), QStringList({"c"}) );
  QCOMPARE( sortedStringListCs(tree.getLibraryList(b1Node)), QStringList({"a"}) );
  QCOMPARE( sortedStringListCs(tree.toFlatList()), QStringList({"a","b","c"}) );
  /*
   *      (exe)
   *     /     \
   *   (a1)   (b1)
   *   /      /  \
   * (c2)   (a2) (d2)
   */
  const auto d2Node = tree.addLibrary("d", b1Node);
  QVERIFY(!d2Node.isNull());
  QCOMPARE( sortedStringListCs(tree.getLibraryList(exeNode)), QStringList({"a","b"}) );
  QCOMPARE( sortedStringListCs(tree.getLibraryList(a1Node)), QStringList({"c"}) );
  QCOMPARE( sortedStringListCs(tree.getLibraryList(b1Node)), QStringList({"a","d"}) );
  QCOMPARE( sortedStringListCs(tree.toFlatList()), QStringList({"a","b","c","d"}) );
  /*
   * Clear
   */
  tree.clear();
  QVERIFY(tree.rootBinary().isEmpty());
  QVERIFY(tree.toFlatList().isEmpty());
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  LibraryTreeTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
