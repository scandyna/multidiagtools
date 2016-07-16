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
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QLineEdit>
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
  // Insert
  QVERIFY(ControllerStatePermission::canInsert(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canInsert(ControllerState::Editing));
}

void ControllerTest::tableViewControllerBasicStateTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::ControllerState;

  /*
   * Initial state
   */
  TableViewController controller;
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
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
  // Check that row state was not signaled (no view is attached)
  QCOMPARE(rowStateSpy.count(), 0);
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
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check that view was updated
  QCOMPARE(viewA.currentIndex().row(), -1);
  /*
   * Populate model
   */
  modelA.populate(5, 3);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewA.currentIndex().row(), 0);
  /*
   * Set model and view
   *  1) view
   *  2) model
   */
  controller.setView(&viewB);
  controller.setModel(&modelB);
  QVERIFY(viewB.selectionModel() != nullptr);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), -1);
  /*
   * Populate model
   */
  modelB.populate(5, 3);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 0);
  /*
   * Change current row from view
   */
  index = modelB.index(1, 0);
  QVERIFY(index.isValid());
  viewB.setCurrentIndex(index);
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 1);
  /*
   * Change current row from controller
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 2);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 2);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 2);
  /*
   * Check toFirst slot
   */
  controller.toFirst();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 0);
  /*
   * Check toNext slot
   */
  controller.toNext();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 1);
  /*
   * Check toLast slot
   */
  controller.toLast();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 4);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 4);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 4);
  /*
   * Check toPrevious slot
   */
  controller.toPrevious();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 3);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 3);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 3);
  /*
   * Change model
   */
  QStringListModel listModel;
  listModel.setStringList({"A","B","C"});
  controller.setModel(&listModel);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check that view was updated
  QCOMPARE(viewB.currentIndex().row(), 0);
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

void ControllerTest::tableViewControllerEditTest()
{
  using Mdt::ItemEditor::TableViewController;
  using Mdt::ItemEditor::RowState;
  using Mdt::ItemEditor::ControllerState;

  TestTableModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;
  QSignalSpy stateSpy(&controller, &TableViewController::controllerStateChanged);
  QList<QVariant> spyItem;

  QVERIFY(stateSpy.isValid());
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  model.populate(3, 2);
  view.show();
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check that we can change current row
   */
  QVERIFY(controller.setCurrentRow(1));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Begin editing
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  beginEditing(view, index, BeginEditTrigger::DoubleClick);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Editing);
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * End edition
   */
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);

  /*
   * Play
   */
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void ControllerTest::mappedWidgetListTest()
{
  using Mdt::ItemEditor::MappedWidgetList;
  using Mdt::ItemEditor::RowState;

  RowState rs;
  MappedWidgetList mwl;
  QLineEdit editA, editB;
  TestTableModel model;

  /*
   * Setup
   */
  model.populate(3, 2);
  mwl.setModel(&model);
  mwl.addWidget(&editA, 0);
  /*
   * Initial state
   */
  QVERIFY(!editA.isEnabled());
  /*
   * Set row state that allows edition
   */
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  mwl.setRowState(rs);
  QVERIFY(editA.isEnabled());
  /*
   * Set row state that not allows edition
   */
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  mwl.setRowState(rs);
  QVERIFY(!editA.isEnabled());
  /*
   * Add a widget
   */
  mwl.addWidget(&editB, 1);
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Set row state that allows edition
   */
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  mwl.setRowState(rs);
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
}

void ControllerTest::mappedWidgetListSetModelTest()
{
  using Mdt::ItemEditor::MappedWidgetList;
  using Mdt::ItemEditor::RowState;

  RowState rs;
  MappedWidgetList mwl;
  QLineEdit editA, editB;

  /*
   * map widgets
   */
  mwl.addWidget(&editA, 0);
  mwl.addWidget(&editB, 1);
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Set a empty model
   */
  TestTableModel model1;
  mwl.setModel(&model1);
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Populate model with 1 column
   */
  model1.populate(3, 1);
  rs.setRowCount(3);
  rs.setCurrentRow(0);
  mwl.setRowState(rs);
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Populate model with 2 columns
   */
  model1.populate(3, 2);
  rs.setRowCount(3);
  rs.setCurrentRow(0);
  mwl.setRowState(rs);
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  /*
   * Set a other model that is allready populated
   */
  TestTableModel model2;
  model2.populate(5, 1);
  mwl.setModel(&model2);
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
}

void ControllerTest::widgetMapperControllerSetModelTest()
{
  using Mdt::ItemEditor::WidgetMapperController;
  using Mdt::ItemEditor::RowState;

  TestTableModel tableModel;
  QStringListModel listModel;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QSignalSpy rowStateSpy(&controller, &WidgetMapperController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.widgetMapper() != nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set a empty model
   */
  QCOMPARE(tableModel.rowCount(), 0);
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Map widgets
   */
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Populate model with 1 column
   */
  tableModel.populate(3, 1);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check that mapped widgets has been updated
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Populate model with 2 columns
   */
  tableModel.populate(3, 2);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check that mapped widgets has been updated
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  /*
   * Change to a empty model model
   */
  QCOMPARE(listModel.rowCount(), 0);
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check that mapped widgets has been updated
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  /*
   * Populate previous model and set it to controller
   */
  tableModel.populate(3, 2);
  controller.setModel(&tableModel);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check that mapped widgets has been updated
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
}

void ControllerTest::widgetMapperControllerCurrentRowChangedTest()
{
  QFAIL("Not implemented yet");
}

/*
 * Helper functions
 */

void ControllerTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void ControllerTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void ControllerTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str,
                            BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
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
