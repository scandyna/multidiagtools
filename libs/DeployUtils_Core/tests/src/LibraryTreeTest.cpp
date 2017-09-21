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
  Mdt::CoreApplication app(argc, argv);
  LibraryTreeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
