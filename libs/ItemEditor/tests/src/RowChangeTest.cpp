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
#include "Mdt/ItemEditor/RowChangeEventMapper.h"
#include "Mdt/ItemEditor/RowChangeEventDispatcher.h"
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

void RowChangeTest::rowChangeEventMapperTest()
{
  using Mdt::ItemEditor::RowChangeEventMapper;
  using Mdt::ItemEditor::RowChangeEventSource;
  using Mdt::ItemEditor::RowState;

  RowChangeEventMapper mapper;
  QSignalSpy rowStateSpy(&mapper, &RowChangeEventMapper::rowStateChanged);
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;
  RowState rs;
  RowChangeEventSource source;

  /*
   * Initial state
   */
  QVERIFY(rowStateSpy.isValid());
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  mapper.setModel(&model);
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Populate model (will produce a model reset)
   */
  model.setStringList({"A"});
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Add a item to model
   *  - Use insertRow()
   */
  QVERIFY(model.insertRow(0));
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection);  /// \todo Update once source is defined
  /*
   * Remove a item from model
   */
  QVERIFY(model.removeRow(0));
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection);  /// \todo Update once source is defined
  /*
   * Set a selection model
   */
  QItemSelectionModel selectionModel(&model);
  mapper.setSelectionModel(&selectionModel);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Change current row
   */
  index = model.index(0, 0);
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection);
  /*
   * Add a item to model
   *  - Use insertRow()
   *
   * Current row is 0 ,
   * we insert before row 0 ,
   * current row will become 1.
   */
  QVERIFY(model.insertRow(0));
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection); /// \todo Update once source is defined
  /*
   * Remove a item from model
   *  - use removeRow()
   *
   * Current row is 1 ,
   * we remove row 0 ,
   * current row will become 0.
   */
  QVERIFY(model.removeRow(0));
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection); /// \todo Update once source is defined
  /*
   * Populate model (will produce a model reset)
   *
   * After a model reset, selection model looses current index
   */
  model.setStringList({"A", "B", "C", "D"});
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Change current to a valid row before setModel() test
   */
  index = model.index(0, 0);
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 0);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection);
  /*
   * Change model of selection model
   *  - Set a empty model
   */
  TestTableModel model2;
  selectionModel.setModel(&model2);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Change model of selection model
   *  - Set a model with data
   */
  selectionModel.setModel(&model);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Change again to model2 for next test
   */
  model2.populate(3, 2);
  selectionModel.setModel(&model2);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), -1);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ModelReset);
  /*
   * Change current row
   */
  index = model2.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  source = spyItem.at(1).value<RowChangeEventSource>();
  QVERIFY(source == RowChangeEventSource::ItemSelection);
  /*
   * Change current column
   */
  index = model2.index(0, 1);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was not signaled
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Play
   */
//   QTableView tv;
//   tv.setModel(&model);
//   mapper.setSelectionModel(tv.selectionModel());
//   model.setStringList({"A", "B", "C"});
//   tv.show();
//   while(tv.isVisible()){
//     QTest::qWait(500);
//   }
}

void RowChangeTest::eventDispatcherTest()
{
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::RowChangeEventSource;
  using Mdt::ItemEditor::RowChangeEventMapper;
  using Mdt::ItemEditor::RowChangeEventDispatcher;

  RowChangeEventDispatcher dispatcher;
  RowChangeEventMapper mapper;
  QList<QVariant> spyItem;
  QStringListModel model;
  QItemSelectionModel selectionModel(&model);
  QModelIndex index;
  RowState rs;
  RowChangeEventSource source;

  /*
   * Setup
   */
  connect(&mapper, &RowChangeEventMapper::selectionModelChanged, &dispatcher, &RowChangeEventDispatcher::setSelectionModel);
  connect(&mapper, &RowChangeEventMapper::rowStateChanged, &dispatcher, &RowChangeEventDispatcher::setRowState);
  QSignalSpy navigationActionsSpy(&dispatcher, &RowChangeEventDispatcher::rowStateChanged);
  QSignalSpy controllerToFirstSpy(&dispatcher, &RowChangeEventDispatcher::modelReset);
  QSignalSpy controllerSetCurrentRowSpy(&dispatcher, &RowChangeEventDispatcher::currentRowChangedForController);
  /*
   * Initial state
   */
  QVERIFY(navigationActionsSpy.isValid());
  QCOMPARE(navigationActionsSpy.count(), 0);
  QVERIFY(controllerToFirstSpy.isValid());
  QCOMPARE(controllerToFirstSpy.count(), 0);
  QVERIFY(controllerSetCurrentRowSpy.isValid());
  QCOMPARE(controllerSetCurrentRowSpy.count(), 0);
  /*
   * Set model
   */
  mapper.setModel(&model);
  // Check that controller received model reset
  QCOMPARE(controllerToFirstSpy.count(), 1);
  controllerToFirstSpy.clear();
  // As controller, we call setCurrentRow()
  dispatcher.setCurrentRow(-1);
  // Check that navigation actions received row sate change
  QCOMPARE(navigationActionsSpy.count(), 1);
  spyItem = navigationActionsSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Set selection model
   */
  mapper.setSelectionModel(&selectionModel);
  // Check that controller received model reset
  QCOMPARE(controllerToFirstSpy.count(), 1);
  controllerToFirstSpy.clear();
  // As controller, we call setCurrentRow()
  dispatcher.setCurrentRow(-1);
  // Check that selection model is up to date
  QCOMPARE(selectionModel.currentIndex().row(), -1);
  // Check that navigation actions received row sate change
  QCOMPARE(navigationActionsSpy.count(), 1);
  spyItem = navigationActionsSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Populate model
   */
  model.setStringList({"A","B","C"});
  // Check that controller received model reset
  QCOMPARE(controllerToFirstSpy.count(), 1);
  controllerToFirstSpy.clear();
  // As controller, we call setCurrentRow()
  dispatcher.setCurrentRow(0);
  // Check that selection model is up to date
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  // Check that navigation actions received row sate change
  QCOMPARE(navigationActionsSpy.count(), 1);
  spyItem = navigationActionsSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Change current row from selection model
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Check that controller received current row change
  QCOMPARE(controllerSetCurrentRowSpy.count(), 1);
  spyItem = controllerSetCurrentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  ///source = spyItem.at(1).value<RowChangeEventSource>();
  ///QVERIFY(source == RowChangeEventSource::ItemSelection);
  // As controller, we call setCurrentRow()
  dispatcher.setCurrentRow(1);
  // Check that selection model is up to date
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  // Check that navigation actions received row sate change
  QCOMPARE(navigationActionsSpy.count(), 1);
  spyItem = navigationActionsSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 1);
  /*
   * Change current row from controller
   * (like a toNext)
   */
  dispatcher.setCurrentRow(2);
  // Check that selection model was updated
  QCOMPARE(selectionModel.currentIndex().row(), 2);
  // Check that controller did not receive current row change back
  QCOMPARE(controllerSetCurrentRowSpy.count(), 0);
  // Check that navigation actions received row sate change
  QCOMPARE(navigationActionsSpy.count(), 1);
  spyItem = navigationActionsSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), 2);

  
//   QVERIFY(rowStateSpy.isValid());
//   QCOMPARE(rowStateSpy.count(), 0);

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
