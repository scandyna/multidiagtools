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
#include "ControllerTest.h"
#include "TestTableModel.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include "Mdt/ItemEditor/RowChangeEventMapper.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QObject>

void ControllerTest::initTestCase()
{
}

void ControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerTest::statePermissionTest()
{
  using Mdt::ItemEditor::ControllerState;
  using Mdt::ItemEditor::ControllerStatePermission;

  // Current row change
  QVERIFY(ControllerStatePermission::canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canChangeCurrentRow(ControllerState::Editing));
}

void ControllerTest::tableViewControllerSetModelTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::RowState;

  TestTableModel tableModel;
  QStringListModel listModel;
  QTableView tableView;
  TableViewController controller;
  QSignalSpy rowStateSpy(&controller, &TableViewController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change model
   */
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Affect view to controller
   */
  controller.setView(&tableView);
  QCOMPARE(controller.view(), &tableView);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);

  /*
   * Play
   */
//   tableView.show();
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

void ControllerTest::tableViewControllerCurrentRowChangeTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::RowState;

  TestTableModel modelA, modelB;
  QModelIndex index;
  QTableView viewA, viewB;
  TableViewController controller;
  QSignalSpy rowStateSpy(&controller, &TableViewController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model and view
   *  1) model
   *  2) view
   */
  controller.setModel(&modelA);
  controller.setView(&viewA);
  QVERIFY(viewA.selectionModel() != nullptr);
  QCOMPARE(controller.rowCount(), modelA.rowCount());
  QCOMPARE(controller.currentRow(), viewA.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 2);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelA.rowCount());
  QCOMPARE(rs.currentRow(), -1);  // Was handled while selection model was not set
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelA.rowCount());
  QCOMPARE(rs.currentRow(), viewA.currentIndex().row());
  /*
   * Populate model
   */
  modelA.populate(5, 3);
  QCOMPARE(controller.rowCount(), modelA.rowCount());
  QCOMPARE(controller.currentRow(), viewA.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelA.rowCount());
  QCOMPARE(rs.currentRow(), viewA.currentIndex().row());
  /*
   * Set model and view
   *  1) view
   *  2) model
   */
  controller.setView(&viewB);
  controller.setModel(&modelB);
  QVERIFY(viewB.selectionModel() != nullptr);
  QCOMPARE(controller.rowCount(), modelB.rowCount());
  QCOMPARE(controller.currentRow(), viewB.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 2);
  // The first event was sent with old model (modelA) - We not care
  spyItem = rowStateSpy.takeFirst();
  // Check that second event was sent correctly
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Populate model
   */
  modelB.populate(5, 3);
  QCOMPARE(controller.rowCount(), modelB.rowCount());
  QCOMPARE(controller.currentRow(), viewB.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Change current row from selection modelB
   */
  index = modelB.index(1, 0);
  QVERIFY(index.isValid());
  viewB.setCurrentIndex(index);
  QCOMPARE(viewB.currentIndex().row(), 1);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Change current row from controller
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.currentRow(), 2);
  QCOMPARE(viewB.currentIndex().row(), 2);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Check toFirst slot
   */
  controller.toFirst();
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(viewB.currentIndex().row(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Check toNext slot
   */
  controller.toNext();
  QCOMPARE(controller.currentRow(), 1);
  QCOMPARE(viewB.currentIndex().row(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Check toLast slot
   */
  controller.toLast();
  QCOMPARE(controller.currentRow(), 4);
  QCOMPARE(viewB.currentIndex().row(), 4);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Check toPrevious slot
   */
  controller.toPrevious();
  QCOMPARE(controller.currentRow(), 3);
  QCOMPARE(viewB.currentIndex().row(), 3);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), modelB.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Change model
   */
  QStringListModel listModel;
  listModel.setStringList({"A","B","C"});
  controller.setModel(&listModel);
  QCOMPARE(controller.rowCount(), listModel.rowCount());
  QCOMPARE(controller.currentRow(), viewB.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), listModel.rowCount());
  QCOMPARE(rs.currentRow(), viewB.currentIndex().row());
  /*
   * Insert
   * NOTE: is a other test to implement
   */

  /*
   * Remove
   * NOTE: is a other test to implement
   */

  /*
   * Play
   */
//   viewB.show();
//   while(viewB.isVisible()){
//     QTest::qWait(500);
//   }
}

void ControllerTest::setModelTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::RowState;

  TestTableModel tableModel;
  QStringListModel listModel;
  TableViewController controller;
  QSignalSpy rowStateSpy(&controller, &TableViewController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change model
   */
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
}

void ControllerTest::currentRowChangeTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::ItemSelectionModel;
  using Mdt::ItemEditor::RowState;

  TableViewController controller;
  TestTableModel model;
  QModelIndex index;
  ItemSelectionModel selectionModel(&model);
  QSignalSpy rowStateSpy(&controller, &TableViewController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  /*
   * Initial state
   */
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model and selection model
   */
  controller.setModel(&model);
  ///controller.setSelectionModel(&selectionModel);
  QCOMPARE(controller.rowCount(), model.rowCount());
  QCOMPARE(controller.currentRow(), selectionModel.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 2);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  rowStateSpy.clear();
  /*
   * Populate model
   */
  model.populate(5, 3);
  QCOMPARE(controller.rowCount(), model.rowCount());
  QCOMPARE(controller.currentRow(), selectionModel.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Change current row from selection model
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Change current row from controller
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.currentRow(), 2);
  QCOMPARE(selectionModel.currentIndex().row(), 2);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Check toFirst slot
   */
  controller.toFirst();
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Check toNext slot
   */
  controller.toNext();
  QCOMPARE(controller.currentRow(), 1);
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Check toLast slot
   */
  controller.toLast();
  QCOMPARE(controller.currentRow(), 4);
  QCOMPARE(selectionModel.currentIndex().row(), 4);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Check toPrevious slot
   */
  controller.toPrevious();
  QCOMPARE(controller.currentRow(), 3);
  QCOMPARE(selectionModel.currentIndex().row(), 3);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), model.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Change model
   */
  QStringListModel listModel;
  listModel.setStringList({"A","B","C"});
  controller.setModel(&listModel);
  QCOMPARE(controller.rowCount(), listModel.rowCount());
  QCOMPARE(controller.currentRow(), selectionModel.currentIndex().row());
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), listModel.rowCount());
  QCOMPARE(rs.currentRow(), selectionModel.currentIndex().row());
  /*
   * Insert
   * NOTE: is a other test to implement
   */

  /*
   * Remove
   * NOTE: is a other test to implement
   */

  /*
   * Play
   */
//   QTableView tableView;
//   tableView.setModel(controller.model());
//   tableView.show();
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
