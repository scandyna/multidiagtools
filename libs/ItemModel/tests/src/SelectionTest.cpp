/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "SelectionTest.h"
#include "Mdt/ItemModel/RowSelection.h"
#include "Mdt/ItemModel/RowSelectionAlgorithm.h"
#include "Mdt/ItemModel/RowRange.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/Container/StlContainer.h"
#include <QItemSelectionModel>

using namespace Mdt::ItemModel;
using namespace Mdt::Container;

/*
 * Tests
 */

void SelectionTest::rowDefaultConstructTest()
{
  RowSelection s;
  QCOMPARE(s.rangeCount(), 0);
  QVERIFY(s.isEmpty());

  QFAIL("Not complete");
}

void SelectionTest::rowIsRangeIncludedTest()
{
  RowRange a, b;

  /*
   * a: |0|1|
   * b:     |2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(2);
  b.setLastRow(3);
  QVERIFY(!isRangeIncluded(a, b));
  QVERIFY(!isRangeIncluded(b, a));
  QVERIFY(isRangeIncluded(a, a));
  QVERIFY(isRangeIncluded(b, b));

  /*
   * a: |0|1|
   * b:   |1|2|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(1);
  b.setLastRow(2);
  QVERIFY(!isRangeIncluded(a, b));
  QVERIFY(!isRangeIncluded(b, a));

  /*
   * a: |0|1|2|3|
   * b:   |1|2|
   */
  a.setFirstRow(0);
  a.setLastRow(3);
  b.setFirstRow(1);
  b.setLastRow(2);
  QVERIFY(!isRangeIncluded(a, b));
  QVERIFY(isRangeIncluded(b, a));

  /*
   * a: |0|1|2|3|
   * b: |0|1|
   */
  a.setFirstRow(0);
  a.setLastRow(3);
  b.setFirstRow(0);
  b.setLastRow(1);
  QVERIFY(!isRangeIncluded(a, b));
  QVERIFY(isRangeIncluded(b, a));

  /*
   * a: |0|1|2|3|
   * b:     |2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(3);
  b.setFirstRow(2);
  b.setLastRow(3);
  QVERIFY(!isRangeIncluded(a, b));
  QVERIFY(isRangeIncluded(b, a));
}

void SelectionTest::rowIsRangeIncludedInSelectionTest()
{
  RowSelection s;
  RowRange r;

  /*
   * s:
   * r: |0|1|
   */
  r.setFirstRow(0);
  r.setLastRow(1);
  QVERIFY(!s.isRangeIncluded(r));
  /*
   * s: |0|1|
   * r: |0|1|
   */
  s.appendRange(r);
  QVERIFY(s.isRangeIncluded(r));
  /*
   * s: |0|1|
   * r:   |1|2|
   */
  r.setFirstRow(1);
  r.setLastRow(2);
  QVERIFY(!s.isRangeIncluded(r));
}

void SelectionTest::rowCompareRangesTest()
{
  RowRange a, b;

  /*
   * a: |0|1|
   * b:     |2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(2);
  b.setLastRow(3);
  QVERIFY(compareRanges(a, b));
  QVERIFY(!compareRanges(b, a));

  /*
   * a: |0|1|
   * b:   |1|2|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(1);
  b.setLastRow(2);
  QVERIFY(compareRanges(a, b));
  QVERIFY(!compareRanges(b, a));

  /*
   * a: |0|1|2|
   * b:   |1|2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(2);
  b.setFirstRow(1);
  b.setLastRow(3);
  QVERIFY(compareRanges(a, b));
  QVERIFY(!compareRanges(b, a));
}

void SelectionTest::rowSortRangeTest()
{
  RowRangeList list;
  RowRange r;

  /*
   * list:
   */
  sortRanges(list);
  QVERIFY(containerIsEmpty(list));
  /*
   * list: |0|1|
   */
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  sortRanges(list);
  QCOMPARE(containerSize(list), 1);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);

  /*
   * list: |0|1| |3|4|
   */
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  sortRanges(list);
  QCOMPARE(containerSize(list), 2);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);
  QCOMPARE(constValueAtIndex(list, 1).firstRow(), 3);
  QCOMPARE(constValueAtIndex(list, 1).lastRow(), 4);

  /*
   * list: |0|1| |3|4|
   *         |1|2|
   */
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  sortRanges(list);
  QCOMPARE(containerSize(list), 3);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);
  QCOMPARE(constValueAtIndex(list, 1).firstRow(), 1);
  QCOMPARE(constValueAtIndex(list, 1).lastRow(), 2);
  QCOMPARE(constValueAtIndex(list, 2).firstRow(), 3);
  QCOMPARE(constValueAtIndex(list, 2).lastRow(), 4);
}

void SelectionTest::rowIsRangeAdjacentTest()
{
  RowRange a, b;

  /*
   * a: |0|1|
   * b:   |1|2|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(1);
  b.setLastRow(2);
  QVERIFY(isRangeAdjacent(a, b));

  /*
   * a: |0|1|
   * b:     |2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(2);
  b.setLastRow(3);
  QVERIFY(isRangeAdjacent(a, b));

  /*
   * a: |0|1|2|
   * b:   |1|2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(2);
  b.setFirstRow(1);
  b.setLastRow(3);
  QVERIFY(isRangeAdjacent(a, b));

  /*
   * a: |0|1|
   * b:       |3|4|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(3);
  b.setLastRow(4);
  QVERIFY(!isRangeAdjacent(a, b));
}

// void SelectionTest::rowRangesAreConsecutiveTest()
// {
//   RowRange a, b;
// 
//   a.setFirstRow(0);
//   a.setLastRow(1);
//   b.setFirstRow(2);
//   b.setLastRow(3);
//   QVERIFY(RowSelection::rangesAreConsecutive(a, b));
//   QVERIFY(RowSelection::rangesAreConsecutive(b, a));
// 
//   a.setFirstRow(0);
//   a.setLastRow(1);
//   b.setFirstRow(3);
//   b.setLastRow(4);
//   QVERIFY(!RowSelection::rangesAreConsecutive(a, b));
//   QVERIFY(!RowSelection::rangesAreConsecutive(b, a));
// 
//   a.setFirstRow(0);
//   a.setLastRow(0);
//   b.setFirstRow(0);
//   b.setLastRow(1);
//   QVERIFY(RowSelection::rangesAreConsecutive(a, b));
//   QVERIFY(RowSelection::rangesAreConsecutive(b, a));
// 
//   a.setFirstRow(0);
//   a.setLastRow(0);
//   b.setFirstRow(0);
//   b.setLastRow(0);
//   QVERIFY(RowSelection::rangesAreConsecutive(a, b));
//   QVERIFY(RowSelection::rangesAreConsecutive(b, a));
// 
//   QFAIL("Not complete");
// }

void SelectionTest::rowMergeRangesTest()
{
  RowRange a, b, r;

  /*
   * a: |0|1|
   * b:   |1|2|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(1);
  b.setLastRow(2);
  r = mergeRanges(a, b);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 2);

  /*
   * a: |0|1|
   * b:     |2|3|
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(2);
  b.setLastRow(3);
  r = mergeRanges(a, b);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 3);
  /*
   * a: (0-1)
   * b:       (3-4)
   */
  a.setFirstRow(0);
  a.setLastRow(1);
  b.setFirstRow(3);
  b.setLastRow(4);
  r = mergeRanges(a, b);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.lastRow(), 4);

}

void SelectionTest::rowFindAdjacentRangesTest()
{
  RowRangeList list;
  RowRange r;

  /*
   * list:
   */
  auto it = findAdjacentRanges(list.cbegin(), list.cend());
  QVERIFY(it == list.cend());
  QVERIFY(!hasAdjacentRanges(list));
  /*
   * list: |0|1|
   */
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  it = findAdjacentRanges(list.cbegin(), list.cend());
  QVERIFY(it == list.cend());
  QVERIFY(!hasAdjacentRanges(list));
  /*
   * list: |0|1| |3|4|
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  it = findAdjacentRanges(list.cbegin(), list.cend());
  QVERIFY(it == list.cend());
  QVERIFY(!hasAdjacentRanges(list));
  /*
   * list: |0|1| |3|4|
   *         |1|2|
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  it = findAdjacentRanges(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin()); // range |0|1| (adjacent to |1|2|)
  QVERIFY(hasAdjacentRanges(list));
  ++it;
  it = findAdjacentRanges(it, list.cend());
  QVERIFY(it == list.cbegin()+1); // range |1|2| (adjacent to |3|4|)
  QVERIFY(hasAdjacentRanges(list));
}

void SelectionTest::rowFindLastAdjacentRangeTest()
{
  RowRangeList list;
  RowRange r;

  /*
   * list:
   */
//   auto it = findLastAdjacentRange(list.cbegin(), list.cend());
//   QVERIFY(it == list.cend());

  /*
   * list: (0-1)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  auto it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin());

  /*
   * list: (0-1)(1-2)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin()+1);
//   ++it;
//   it = findLastAdjacentRange(it, list.cend());
//   QVERIFY(it == list.cend());

  /*
   * list: (0-1) (3-4)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin());
  ++it;
  it = findLastAdjacentRange(it, list.cend());
  QVERIFY(it == list.cbegin()+1);

  /*
   * list: (0-1)(1-2)(3-4)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin()+2);
//   ++it;
//   it = findLastAdjacentRange(it, list.cend());
//   QVERIFY(it == list.cend());

  /*
   * list: (0-1)(1-2) (4-5)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(4);
  r.setLastRow(5);
  appendToContainer(list, r);
  it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin()+1);
  ++it;
  it = findLastAdjacentRange(it, list.cend());
  QVERIFY(it == list.cbegin()+2);

  /*
   * list: (0-1)(1-2) (4-5)(6-7)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(4);
  r.setLastRow(5);
  appendToContainer(list, r);
  r.setFirstRow(6);
  r.setLastRow(7);
  appendToContainer(list, r);
  it = findLastAdjacentRange(list.cbegin(), list.cend());
  QVERIFY(it == list.cbegin()+1);
  ++it;
  it = findLastAdjacentRange(it, list.cend());
  QVERIFY(it == list.cbegin()+3);
//   ++it;
//   it = findLastAdjacentRange(it, list.cend());
//   QVERIFY(it == list.cend());

//   QFAIL("Not correct (new contract)");
}

void SelectionTest::rowMergeAdjacentRangesTest()
{
  RowRangeList list;
  RowRange r;

  /*
   * list:
   */
  mergeAdjacentRanges(list);
  QVERIFY(containerIsEmpty(list));
  /*
   * list: |0|1|
   */
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  mergeAdjacentRanges(list);
  QCOMPARE(containerSize(list), 1);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);
  /*
   * list: |0|1| |3|4|
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  mergeAdjacentRanges(list);
  QCOMPARE(containerSize(list), 2);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);
  QCOMPARE(constValueAtIndex(list, 1).firstRow(), 3);
  QCOMPARE(constValueAtIndex(list, 1).lastRow(), 4);
  /*
   * list: (0-1) (3-4)(5-6)
   *  -->: (0-1) (3  -   6)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  r.setFirstRow(5);
  r.setLastRow(6);
  appendToContainer(list, r);
  mergeAdjacentRanges(list);
  QCOMPARE(containerSize(list), 2);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 1);
  QCOMPARE(constValueAtIndex(list, 1).firstRow(), 3);
  QCOMPARE(constValueAtIndex(list, 1).lastRow(), 6);
  /*
   * list: (0-1)(1-2)(3-4)
   *  -->: (0    -      4)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  mergeAdjacentRanges(list);
  QCOMPARE(containerSize(list), 1);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 4);
  /*
   * list: (0-1)(1-2)(3-4) (6-7) (10-11)(12-13)
   *  -->: (0    -      4) (6-7) (10   -    13)
   */
  list.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  appendToContainer(list, r);
  r.setFirstRow(1);
  r.setLastRow(2);
  appendToContainer(list, r);
  r.setFirstRow(3);
  r.setLastRow(4);
  appendToContainer(list, r);
  r.setFirstRow(6);
  r.setLastRow(7);
  appendToContainer(list, r);
  r.setFirstRow(10);
  r.setLastRow(11);
  appendToContainer(list, r);
  r.setFirstRow(12);
  r.setLastRow(13);
  appendToContainer(list, r);
  mergeAdjacentRanges(list);
  QCOMPARE(containerSize(list), 3);
  QCOMPARE(constValueAtIndex(list, 0).firstRow(), 0);
  QCOMPARE(constValueAtIndex(list, 0).lastRow(), 4);
  QCOMPARE(constValueAtIndex(list, 1).firstRow(), 6);
  QCOMPARE(constValueAtIndex(list, 1).lastRow(), 7);
  QCOMPARE(constValueAtIndex(list, 2).firstRow(), 10);
  QCOMPARE(constValueAtIndex(list, 2).lastRow(), 13);
}

void SelectionTest::rowAppendRangeTest()
{
  RowSelection s;
  RowRange r;
  QCOMPARE(s.rangeCount(), 0);
  /*
   * ---
   * |0|
   * ---
   */
  r.setFirstRow(0);
  r.setLastRow(0);
  s.appendRange(r);
  QCOMPARE(s.rangeCount(), 1);
  QCOMPARE(s.rangeAt(0).firstRow(), 0);
  QCOMPARE(s.rangeAt(0).lastRow(), 0);
  /*
   * ---
   * |0|
   * ---
   * | |
   * ---
   * |2|
   * ---
   * |3|
   * ---
   */
  r.setFirstRow(2);
  r.setLastRow(3);
  s.appendRange(r);
  QCOMPARE(s.rangeCount(), 2);
  QCOMPARE(s.rangeAt(0).firstRow(), 0);
  QCOMPARE(s.rangeAt(0).lastRow(), 0);
  QCOMPARE(s.rangeAt(1).firstRow(), 2);
  QCOMPARE(s.rangeAt(1).lastRow(), 3);
  /*
   * ---
   * |0|
   * ---
   * |1|
   * ---
   * |2|
   * ---
   * |3|
   * ---
   */
  r.setFirstRow(1);
  r.setLastRow(1);
  s.appendRange(r);
  QCOMPARE(s.rangeCount(), 1);
  QCOMPARE(s.rangeAt(0).firstRow(), 0);
  QCOMPARE(s.rangeAt(0).lastRow(), 3);

  QFAIL("Not complete");
}

void SelectionTest::rowFromSelectionModelTest()
{
  RowSelection s;
  RowRange r;
  VariantTableModel model;
  model.resize(5, 3);
  QItemSelectionModel selectionModel(&model);

  /*
   * Null pointer
   */
  s = RowSelection::fromSelectionModel(nullptr);
  QVERIFY(s.isEmpty());
  /*
   * Empty selection
   */
  QVERIFY(!selectionModel.hasSelection());
  s = RowSelection::fromSelectionModel(&selectionModel);
  QVERIFY(s.isEmpty());
  /*
   * -------
   * |S| | |
   * -------
   * | | | |
   * -------
   * | | | |
   * -------
   * | | | |
   * -------
   * | | | |
   * -------
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  s = RowSelection::fromSelectionModel(&selectionModel);
  QCOMPARE(s.rangeCount(), 1);
  r = s.rangeAt(0);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.rowCount(), 1);
  // Check also range for
  for(const auto & rowRange : s){
    QCOMPARE(rowRange.firstRow(), 0);
    QCOMPARE(rowRange.rowCount(), 1);
  }
  /*
   * -------
   * |S| | |
   * -------
   * | | | |
   * -------
   * | | |S|
   * -------
   * | |S|S|
   * -------
   * | | | |
   * -------
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 2), QItemSelectionModel::Select );
  selectionModel.select( model.index(3, 1), QItemSelectionModel::Select );
  selectionModel.select( model.index(3, 2), QItemSelectionModel::Select );
  s = RowSelection::fromSelectionModel(&selectionModel);
  QCOMPARE(s.rangeCount(), 2);
  r = s.rangeAt(0);
  QCOMPARE(r.firstRow(), 0);
  QCOMPARE(r.rowCount(), 1);
  r = s.rangeAt(1);
  QCOMPARE(r.firstRow(), 2);
  QCOMPARE(r.rowCount(), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectionTest test;

  return QTest::qExec(&test, argc, argv);
}
