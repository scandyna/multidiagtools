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
#include "RowChangeTest.h"
#include "TestTableModel.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/RowState.h"
#include "Mdt/ItemEditor/RowChangeEventDispatcher.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
// #include "Mdt/ItemEditor/NavigationActions.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>

#include <QDebug>

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
  using Mdt::ItemEditor::RowState;

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
  using Mdt::ItemEditor::RowState;

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

void RowChangeTest::eventDispatcherTest()
{
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::RowChangeEventDispatcher;

  RowChangeEventDispatcher dispatcher;
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;
  RowState rs;
  QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Checks without model set
   */
  dispatcher.setCurrentRow(-1);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   * Note: setting a model must allways been signaled
   */
  dispatcher.setModel(&model);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Populate model
   */
  model.setStringList({"A","B","C"});
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Set current row
   */
  dispatcher.setCurrentRow(1);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 1);
  /*
   * Set the same current row again
   */
  dispatcher.setCurrentRow(1);
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was not signaled again
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set a empty model
   */
  QStringListModel model2;
  dispatcher.setModel(&model2);
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model2.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Set previous model again
   */
  QVERIFY(model.rowCount() > 0);
  dispatcher.setModel(&model);
  QCOMPARE(dispatcher.rowCount(), model.rowCount());
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
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

// void RowChangeTest::eventDispatcherWithSelectionModelTest()
// {
//   using Mdt::ItemEditor::RowState;
//   using Mdt::ItemEditor::RowChangeEventDispatcher;
//   using Mdt::ItemEditor::ItemSelectionModel;
// 
//   RowChangeEventDispatcher dispatcher;
//   QList<QVariant> spyItem;
//   QStringListModel model;
//   ItemSelectionModel selectionModel(&model);
//   QModelIndex index;
//   RowState rs;
//   QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Initial state
//    */
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Checks without model set
//    */
//   dispatcher.setCurrentRow(-1);
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Set model
//    * Note: setting a model must allways been signaled
//    */
//   dispatcher.setModel(&model);
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Set selection model
//    * Note: row state does not change here
//    */
//   dispatcher.setSelectionModel(&selectionModel);
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), -1);
//   // Check that row state was not signaled again
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Populate model
//    */
//   model.setStringList({"A","B","C"});
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Set selection model
//    * Note: row state does not change here
//    */
//   dispatcher.setSelectionModel(&selectionModel);
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was not signaled again
//   QCOMPARE(rowStateSpy.count(), 0);
//   /*
//    * Set current row from selection model
//    */
//   index = model.index(1, 0);
//   QVERIFY(index.isValid());
//   selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 1);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 1);
//   /*
//    * Set current row from dispatcher
//    */
//   dispatcher.setCurrentRow(2);
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 2);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 2);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 2);
//   /*
//    * Set a empty model
//    */
//   QStringListModel model2;
//   dispatcher.setModel(&model2);
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), -1);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model2.rowCount());
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Set previous model again
//    */
//   QVERIFY(model.rowCount() > 0);
//   dispatcher.setModel(&model);
//   QCOMPARE(dispatcher.rowCount(), model.rowCount());
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
// }

void RowChangeTest::eventDispatcherInsertTest()
{
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::RowChangeEventDispatcher;

  RowChangeEventDispatcher dispatcher;
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;
  RowState rs;
  QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);

  QVERIFY(rowStateSpy.isValid());
  /*
   * Setup
   */
  dispatcher.setModel(&model);
  rowStateSpy.clear();
  /*
   * Insert on a empty model
   */
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  QVERIFY(model.insertRow(0));
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Insert before current row
   */
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.insertRow(0));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled (row count changed)
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Insert at end
   */
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.insertRow(2));
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 2);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 2);
}

// void RowChangeTest::eventDispatcherWithSelectionModelInsertTest()
// {
//   using Mdt::ItemEditor::RowState;
//   using Mdt::ItemEditor::RowChangeEventDispatcher;
// 
//   RowChangeEventDispatcher dispatcher;
//   QList<QVariant> spyItem;
//   QStringListModel model;
//   QItemSelectionModel selectionModel(&model);
//   QModelIndex index;
//   RowState rs;
//   QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Setup
//    */
//   dispatcher.setModel(&model);
//   dispatcher.setSelectionModel(&selectionModel);
//   rowStateSpy.clear();
//   /*
//    * Insert on a empty model
//    */
//   QCOMPARE(dispatcher.currentRow(), -1);
//   QVERIFY(model.insertRow(0));
//   QCOMPARE(dispatcher.rowCount(), 1);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Insert before current row
//    */
//   QCOMPARE(dispatcher.currentRow(), 0);
//   QVERIFY(model.insertRow(0));
//   QCOMPARE(dispatcher.rowCount(), 2);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was signaled (row count changed)
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Insert at end
//    */
//   QCOMPARE(dispatcher.currentRow(), 0);
//   QCOMPARE(dispatcher.rowCount(), 2);
//   QVERIFY(model.insertRow(2));
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 2);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 2);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 2);
// }

void RowChangeTest::eventDispatcherRemoveTest()
{
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::RowChangeEventDispatcher;

  RowChangeEventDispatcher dispatcher;
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;
  RowState rs;
  QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);

  QVERIFY(rowStateSpy.isValid());
  /*
   * Setup
   */
  model.setStringList({"A","B","C","D"});
  dispatcher.setModel(&model);
  dispatcher.setCurrentRow(0);
  rowStateSpy.clear();
  /*
   * Remove at beginning - current row is also at beginning
   */
  QCOMPARE(dispatcher.rowCount(), 4);
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.removeRow(0));
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Remove at end - current row is also at end
   */
  // Move to end
  dispatcher.setCurrentRow(2);
  rowStateSpy.clear();
  // Remove
  QCOMPARE(dispatcher.rowCount(), 3);
  QCOMPARE(dispatcher.currentRow(), 2);
  QVERIFY(model.removeRow(2));
  QCOMPARE(dispatcher.rowCount(), 2);
  QCOMPARE(dispatcher.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 1);
  /*
   * Remove at with a 1 element model
   */
  model.setStringList({"A"});
  rowStateSpy.clear();
  QCOMPARE(dispatcher.rowCount(), 1);
  QCOMPARE(dispatcher.currentRow(), 0);
  QVERIFY(model.removeRow(0));
  QCOMPARE(dispatcher.rowCount(), 0);
  QCOMPARE(dispatcher.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), -1);
}

// void RowChangeTest::eventDispatcherWithSelectionModelRemoveTest()
// {
//   using Mdt::ItemEditor::RowState;
//   using Mdt::ItemEditor::RowChangeEventDispatcher;
//   using Mdt::ItemEditor::ItemSelectionModel;
// 
//   RowChangeEventDispatcher dispatcher;
//   QList<QVariant> spyItem;
//   QStringListModel model;
//   ItemSelectionModel selectionModel(&model);
//   QModelIndex index;
//   RowState rs;
//   QSignalSpy rowStateSpy(&dispatcher, &RowChangeEventDispatcher::rowStateUpdated);
// 
//   QVERIFY(rowStateSpy.isValid());
//   /*
//    * Setup
//    */
//   model.setStringList({"A","B","C","D"});
//   dispatcher.setModel(&model);
//   dispatcher.setSelectionModel(&selectionModel);
//   rowStateSpy.clear();
//   /*
//    * Remove at beginning - current row is also at beginning
//    */
//   QCOMPARE(dispatcher.rowCount(), 4);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   qDebug() << "TEST: remove(0)";
//   QVERIFY(model.removeRow(0));
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Remove at end - current row is also at end
//    */
//   // Move to end
//   dispatcher.setCurrentRow(2);
//   rowStateSpy.clear();
//   // Remove
//   QCOMPARE(dispatcher.rowCount(), 3);
//   QCOMPARE(dispatcher.currentRow(), 2);
//   QVERIFY(model.removeRow(2));
//   QCOMPARE(dispatcher.rowCount(), 2);
//   QCOMPARE(dispatcher.currentRow(), 1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 2);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 1);
//   /*
//    * Remove at with a 1 element model
//    */
//   model.setStringList({"A"});
//   rowStateSpy.clear();
//   QCOMPARE(dispatcher.rowCount(), 1);
//   QCOMPARE(dispatcher.currentRow(), 0);
//   QVERIFY(model.removeRow(0));
//   QCOMPARE(dispatcher.rowCount(), 0);
//   QCOMPARE(dispatcher.currentRow(), -1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), -1);
//   // Check that row state was signaled
//   QCOMPARE(rowStateSpy.count(), 1);
//   spyItem = rowStateSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), -1);
// }

// void RowChangeTest::eventDispatcherTest()
// {
//   using Mdt::ItemEditor::RowState;
//   using Mdt::ItemEditor::RowChangeEventSource;
//   using Mdt::ItemEditor::RowChangeEventMapper;
//   using Mdt::ItemEditor::RowChangeEventDispatcher;
// 
//   RowChangeEventDispatcher dispatcher;
//   RowChangeEventMapper mapper;
//   QList<QVariant> spyItem;
//   QStringListModel model;
//   QItemSelectionModel selectionModel(&model);
//   QModelIndex index;
//   RowState rs;
//   RowChangeEventSource source;
// 
//   /*
//    * Setup
//    */
//   connect(&mapper, &RowChangeEventMapper::selectionModelChanged, &dispatcher, &RowChangeEventDispatcher::setSelectionModel);
//   connect(&mapper, &RowChangeEventMapper::rowStateChanged, &dispatcher, &RowChangeEventDispatcher::setRowState);
//   QSignalSpy navigationActionsSpy(&dispatcher, &RowChangeEventDispatcher::rowStateChanged);
//   QSignalSpy controllerToFirstSpy(&dispatcher, &RowChangeEventDispatcher::modelReset);
//   QSignalSpy controllerSetCurrentRowSpy(&dispatcher, &RowChangeEventDispatcher::currentRowChangedForController);
//   /*
//    * Initial state
//    */
//   QVERIFY(navigationActionsSpy.isValid());
//   QCOMPARE(navigationActionsSpy.count(), 0);
//   QVERIFY(controllerToFirstSpy.isValid());
//   QCOMPARE(controllerToFirstSpy.count(), 0);
//   QVERIFY(controllerSetCurrentRowSpy.isValid());
//   QCOMPARE(controllerSetCurrentRowSpy.count(), 0);
//   /*
//    * Set model
//    */
//   mapper.setModel(&model);
//   // Check that controller received model reset
//   QCOMPARE(controllerToFirstSpy.count(), 1);
//   controllerToFirstSpy.clear();
//   // As controller, we call setCurrentRow()
//   dispatcher.setCurrentRow(-1);
//   // Check that navigation actions received row sate change
//   QCOMPARE(navigationActionsSpy.count(), 1);
//   spyItem = navigationActionsSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Set selection model
//    */
//   mapper.setSelectionModel(&selectionModel);
//   // Check that controller received model reset
//   QCOMPARE(controllerToFirstSpy.count(), 1);
//   controllerToFirstSpy.clear();
//   // As controller, we call setCurrentRow()
//   dispatcher.setCurrentRow(-1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), -1);
//   // Check that navigation actions received row sate change
//   QCOMPARE(navigationActionsSpy.count(), 1);
//   spyItem = navigationActionsSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), -1);
//   /*
//    * Populate model
//    */
//   model.setStringList({"A","B","C"});
//   // Check that controller received model reset
//   QCOMPARE(controllerToFirstSpy.count(), 1);
//   controllerToFirstSpy.clear();
//   // As controller, we call setCurrentRow()
//   dispatcher.setCurrentRow(0);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 0);
//   // Check that navigation actions received row sate change
//   QCOMPARE(navigationActionsSpy.count(), 1);
//   spyItem = navigationActionsSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 0);
//   /*
//    * Change current row from selection model
//    */
//   index = model.index(1, 0);
//   QVERIFY(index.isValid());
//   selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
//   // Check that controller received current row change
//   QCOMPARE(controllerSetCurrentRowSpy.count(), 1);
//   spyItem = controllerSetCurrentRowSpy.takeFirst();
//   QCOMPARE(spyItem.at(0).toInt(), 1);
//   ///source = spyItem.at(1).value<RowChangeEventSource>();
//   ///QVERIFY(source == RowChangeEventSource::ItemSelection);
//   // As controller, we call setCurrentRow()
//   dispatcher.setCurrentRow(1);
//   // Check that selection model is up to date
//   QCOMPARE(selectionModel.currentIndex().row(), 1);
//   // Check that navigation actions received row sate change
//   QCOMPARE(navigationActionsSpy.count(), 1);
//   spyItem = navigationActionsSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 1);
//   /*
//    * Change current row from controller
//    * (like a toNext)
//    */
//   dispatcher.setCurrentRow(2);
//   // Check that selection model was updated
//   QCOMPARE(selectionModel.currentIndex().row(), 2);
//   // Check that controller did not receive current row change back
//   QCOMPARE(controllerSetCurrentRowSpy.count(), 0);
//   // Check that navigation actions received row sate change
//   QCOMPARE(navigationActionsSpy.count(), 1);
//   spyItem = navigationActionsSpy.takeFirst();
//   rs = spyItem.at(0).value<RowState>();
//   QCOMPARE(rs.rowCount(), model.rowCount());
//   QCOMPARE(rs.currentRow(), 2);
// 
//   
// //   QVERIFY(rowStateSpy.isValid());
// //   QCOMPARE(rowStateSpy.count(), 0);
// 
// }

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
