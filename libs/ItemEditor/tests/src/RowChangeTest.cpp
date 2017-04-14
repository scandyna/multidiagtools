/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "RowChangeTest.h"
#include "RowStateChangedSpy.h"
#include "CurrentRowToBeSetSpy.h"
#include "CurrentRowChangedSpy.h"
#include "Mdt/ItemEditor/RowState.h"
#include "Mdt/ItemEditor/RowChangeEventDispatcher.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>

using namespace Mdt::ItemEditor;
using Mdt::ItemModel::VariantTableModel;

/*
 * Init/cleaup
 */

void RowChangeTest::initTestCase()
{
}

void RowChangeTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RowChangeTest::rowStateTest()
{
  /*
   * Initial state
   */
  RowState rs;
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(rs.isValid());
  QVERIFY(rs.isNull());
  /*
   * Set
   */
  // N: 1 , Current: -1
  rs.setRowCount(1);
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 1 , Current: 0
  rs.setCurrentRow(0);
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 1 , Current: 1
  rs.setRowCount(1);
  rs.setCurrentRow(1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: -1
  rs.setRowCount(2);
  rs.setCurrentRow(-1);
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: 0
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 2 , Current: 1
  rs.setRowCount(2);
  rs.setCurrentRow(1);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 2 , Current: 2
  rs.setRowCount(2);
  rs.setCurrentRow(2);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: 3
  rs.setRowCount(2);
  rs.setCurrentRow(3);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  /*
   * Clear
   */
  rs.clear();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(rs.isValid());
  QVERIFY(rs.isNull());
}

void RowChangeTest::rowStateComparisonTest()
{
  RowState a, b;
  /*
   * Both a and b are null
   * Null state must be ignored
   */
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  /*
   * Only b is null
   */
  a.setRowCount(2);
  a.setCurrentRow(0);
  QVERIFY(!(a == b));
  QVERIFY(a != b);
  /*
   * Both a and be are not null
   */
  // a == b
  a.setRowCount(3);
  a.setCurrentRow(1);
  b.setRowCount(3);
  b.setCurrentRow(1);
  QVERIFY(a == b);
  QVERIFY(!(a != b));
  // a != b
  b.setCurrentRow(0);
  QVERIFY(!(a == b));
  QVERIFY(a != b);
}

void RowChangeTest::eventDispatcherChangeModelTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
//   CurrentRowChangedSpy currentRowChangedSpy(dispatcher);
  /*
   * Initial state
   */
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a empty model
   */
  VariantTableModel model1;
  dispatcher.setModel(&model1);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a model with data
   */
  VariantTableModel model2;
  model2.resize(2, 1);
  dispatcher.setModel(&model2);
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), 0);
  /*
   * Set a empty model
   */
  VariantTableModel model3;
  dispatcher.setModel(&model3);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), -1);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), -1);
}

void RowChangeTest::eventDispatcherTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QStringListModel model;
  QModelIndex index;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
//   CurrentRowChangedSpy currentRowChangedSpy(dispatcher);
  /*
   * Initial state
   */
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Checks without model set
   */
  dispatcher.setCurrentRow(-1);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a empty model
   */
  dispatcher.setModel(&model);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Populate model
   */
  model.setStringList({"A","B","C"});
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), 0);
  /*
   * Change current row
   */
  dispatcher.setCurrentRow(1);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 1);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), 1);
  /*
   * Set the same current row again
   */
  dispatcher.setCurrentRow(1);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was not signaled again
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a empty model
   */
  QStringListModel model2;
  dispatcher.setModel(&model2);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model2.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), -1);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), -1);
  /*
   * Set previous model again
   */
  QVERIFY(model.rowCount() > 0);
  dispatcher.setModel(&model);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), 0);
  /*
   * Go to row 0 and set a new populated model with same row count.
   * rowStateChanged must not be signaled
   * currentRowToBeSet must be signaled
   */
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  VariantTableModel model3;
  model3.resize(3, 1);
  dispatcher.setModel(&model3);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Go to row 0 and repopulated model with same row count (will produce a model reset)
   * rowStateChanged must not be signaled
   * currentRowToBeSet must be signaled
   */
  model3.resize(3, 1);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Set a null pointer model
   * Must reset and signal row state
   * (is different than before)
   */
  dispatcher.setModel(nullptr);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), -1);
//   QCOMPARE(currentRowChangedSpy.count(), 1);
//   QCOMPARE(currentRowChangedSpy.takeCurrentRow(), -1);
  /*
   * Set a null pointer model again
   */
  dispatcher.setModel(nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
//   QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Play
   */
//   QTableView tv;
//   tv.setModel(&model);
//   tv.show();
//   while(tv.isVisible()){
//     QTest::qWait(500);
//   }
}

void RowChangeTest::eventDispatcherInsertFromModelTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QStringListModel model;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
  /*
   * Setup
   */
  dispatcher.setModel(&model);
  rowStateSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
  /*
   * Insert a row to a empty model
   * - Current row must not change
   *
   * Before   After
   * >        >
   *           ---
   *          N|0|
   *           ---
   *
   */
  QVERIFY(model.insertRow(0));
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
  /*
   * Go to row 0
   */
  dispatcher.setCurrentRow(0);
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  /*
   * Insert a row after row 0
   * - Current row must not change
   *
   * Before   After
   *  ---      ---
   * >|0|     >|0|
   *  ---      ---
   *          N|1|
   *           ---
   */
  QVERIFY(model.insertRow(1));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
  /*
   * Insert a row before row 0
   * - Current row must be updated to 1
   *
   * Before   After
   *           ---
   *          N|0|
   *  ---      ---
   * >|0|     >|1|
   *  ---      ---
   *  |1|      |2|
   *  ---      ---
   */
  QVERIFY(model.insertRow(0));
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 1);
  /*
   * Insert a row before row 0
   * - Current row must be updated to 2
   *
   * Before   After
   *           ---
   *          N|0|
   *  ---      ---
   *  |0|      |1|
   *  ---      ---
   * >|1|     >|2|
   *  ---      ---
   *  |2|      |3|
   *  ---      ---
   */
  QVERIFY(model.insertRow(0));
  QCOMPARE(dispatcher.rowCount(), 4);
  QCOMPARE(dispatcher.currentRow(), 2);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 2);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 2);
}

void RowChangeTest::eventDispatcherInsertFromModelMultiTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QStringListModel model;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  /*
   * Setup
   */
  dispatcher.setModel(&model);
  rowStateSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  /*
   * Insert 2 rows to a empty model
   * - Current row must not change
   *
   * Before   After
   * >        >
   *           ---
   *          N|0|
   *           ---
   *          N|1|
   *           ---
   */
  QVERIFY(model.insertRows(0, 2));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Go to row 0
   */
  dispatcher.setCurrentRow(0);
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  rowStateSpy.clear();
  /*
   * Insert 2 rows after row 0
   * - Current row must not change
   *
   * Before   After
   *  ---      ---
   * >|0|     >|0|
   *  ---      ---
   *  |1|     N|1|
   *  ---      ---
   *          N|2|
   *           ---
   *           |3|
   *           ---
   */
  QVERIFY(model.insertRows(1, 2));
  QCOMPARE(dispatcher.rowCount(), 4);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Insert 2 rows before row 0
   * - Current row must become 2
   *
   * Before   After
   *           ---
   *          N|0|
   *           ---
   *          N|1|
   *  ---      ---
   * >|0|     >|2|
   *  ---      ---
   *  |1|      |3|
   *  ---      ---
   *  |2|      |4|
   *  ---      ---
   *  |3|      |5|
   *  ---      ---
   */
  QVERIFY(model.insertRows(0, 2));
  QCOMPARE(dispatcher.rowCount(), 6);
  QCOMPARE(dispatcher.currentRow(), 2);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 6);
  QCOMPARE(rs.currentRow(), 2);
}

void RowChangeTest::eventDispatcherInsertAtBeginningTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QStringListModel model;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
  /*
   * Setup
   */
  dispatcher.setModel(&model);
  rowStateSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  /*
   * Insert a row to a empty model
   * - Current row must be set to 0
   */
  QVERIFY(dispatcher.insertRowAtBeginning());
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Insert a row before row 0
   * - Current row must be set to 0
   */
  QVERIFY(dispatcher.insertRowAtBeginning());
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
}

void RowChangeTest::eventDispatcherInsertAtEndTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QStringListModel model;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
  /*
   * Setup
   */
  dispatcher.setModel(&model);
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  /*
   * Insert a row to a empty model
   * - Current row must be set to 0
   */
  QVERIFY(dispatcher.insertRowAtEnd());
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Insert a row after row 0
   * - Current row must be set to 1
   */
  QVERIFY(dispatcher.insertRowAtEnd());
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 1);
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 1);
}

void RowChangeTest::eventDispatcherRemoveTest()
{
  RowChangeEventDispatcher dispatcher(nullptr);
  QList<QVariant> spyItem;
  QStringListModel model;
  RowState rs;
  RowStateChangedSpy rowStateSpy(dispatcher);
  CurrentRowToBeSetSpy currentRowToBeSetSpy(dispatcher);
  QSignalSpy rowsRemovedSpy(&dispatcher, &RowChangeEventDispatcher::rowsRemoved);

  QVERIFY(rowsRemovedSpy.isValid());
  /*
   * Setup
   */
  model.setStringList({"A","B","C","D"});
  dispatcher.setModel(&model);
  dispatcher.setCurrentRow(0);
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  /*
   * Remove at beginning - current row is also at beginning
   *
   * Now: A,B,C,D   - After remove: B,C,D
   *      ^                         ^
   */
  QCOMPARE(dispatcher.rowCount(), 4);
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.removeRow(0));
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  // Check that rows removed isgnal was emitted
  QCOMPARE(rowsRemovedSpy.count(), 1);
  rowsRemovedSpy.clear();
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Remove at end - current row is also at end
   *
   * Now: B,C,D   - After remove: B,C
   *          ^                     ^
   */
  // Move to end
  dispatcher.setCurrentRow(2);
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  // Remove
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 2);
  QVERIFY(model.removeRow(2));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 1);
  // Check that rows removed isgnal was emitted
  QCOMPARE(rowsRemovedSpy.count(), 1);
  rowsRemovedSpy.clear();
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 1);
  /*
   * Remove after current row
   *
   * Now: A,B,C,D   - After remove: A,B,C
   *      ^                         ^
   */
  // Repopulate model
  model.setStringList({"A","B","C","D"});
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 4);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Remove and check
  QVERIFY(model.removeRow(3));
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  // Check that rows removed isgnal was emitted
  QCOMPARE(rowsRemovedSpy.count(), 1);
  rowsRemovedSpy.clear();
  QCOMPARE(currentRowToBeSetSpy.count(), 0);
  /*
   * Remove before current row
   *
   * Now: A,B,C   - After remove: B,C
   *        ^                     ^
   */
  dispatcher.setCurrentRow(1);
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  QCOMPARE(dispatcher.currentRow(), 1);
  // Remove and check
  QVERIFY(model.removeRow(0));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  // Check that rows removed isgnal was emitted
  QCOMPARE(rowsRemovedSpy.count(), 1);
  rowsRemovedSpy.clear();
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), 0);
  /*
   * Remove at with a 1 element model
   */
  model.setStringList({"A"});
  rowStateSpy.clear();
  currentRowToBeSetSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.removeRow(0));
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  rs = rowStateSpy.takeRowState();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  // Check that rows removed isgnal was emitted
  QCOMPARE(rowsRemovedSpy.count(), 1);
  rowsRemovedSpy.clear();
  QCOMPARE(currentRowToBeSetSpy.count(), 1);
  QCOMPARE(currentRowToBeSetSpy.takeCurrentRow(), -1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  RowChangeTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
