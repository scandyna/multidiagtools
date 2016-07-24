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
// #include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

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
  // Submit
  QVERIFY(!ControllerStatePermission::canSubmit(ControllerState::Visualizing));
  QVERIFY(ControllerStatePermission::canSubmit(ControllerState::Editing));
  // Revert
  QVERIFY(!ControllerStatePermission::canRevert(ControllerState::Visualizing));
  QVERIFY(ControllerStatePermission::canRevert(ControllerState::Editing));
  // Remove
  QVERIFY(ControllerStatePermission::canRemove(ControllerState::Visualizing));
  QVERIFY(!ControllerStatePermission::canRemove(ControllerState::Editing));
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

void ControllerTest::tableViewCurrentRowChangeBenchmark()
{
  TestTableModel model;
  QModelIndex index;
  QTableView view;

  view.setModel(&model);
  model.populate(5, 3);
  view.show();

  index = model.index(0, 0);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  QCOMPARE(view.currentIndex().row(), 0);

  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QBENCHMARK{
    view.setCurrentIndex(index);
    QCOMPARE(view.currentIndex().row(), 1);
  }
}

void ControllerTest::tableViewControllerCurrentRowChangeBenchmark()
{
  using Mdt::ItemEditor::TableViewController;

  TestTableModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  controller.setModel(&model);
  controller.setView(&view);
  model.populate(5, 3);
  view.show();

  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(view.currentIndex().row(), 0);

  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QBENCHMARK{
    view.setCurrentIndex(index);
    QCOMPARE(controller.currentRow(), 1);
  }
}

void ControllerTest::tableViewControllerEditTest()
{
  using Mdt::ItemEditor::TableViewController;
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
   * Edit some text
   */
  editText(view, index, "ABCD");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * End edition (by user)
   */
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was updated
  QCOMPARE(model.data(index).toString(), QString("ABCD"));
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
   * Edit some text
   */
  editText(view, index, "1234");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * Call submit from controller
   */
  QVERIFY(controller.submit());
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was updated
  QCOMPARE(model.data(index).toString(), QString("1234"));
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
   * Edit some text
   */
  editText(view, index, "EFGH");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(stateSpy.count(), 0);
  /*
   * Call revert from controller
   */
  controller.revert();
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was not updated
  QCOMPARE(model.data(index).toString(), QString("1234"));

  /*
   * Play
   */
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

// void ControllerTest::mappedWidgetListTest()
// {
//   using Mdt::ItemEditor::MappedWidgetList;
//   using Mdt::ItemEditor::RowState;
// 
//   RowState rs;
//   MappedWidgetList mwl;
//   QLineEdit editA, editB;
//   TestTableModel model;
// 
//   /*
//    * Setup
//    */
//   model.populate(3, 2);
//   mwl.setModel(&model);
//   mwl.addWidget(&editA, 0);
//   /*
//    * Initial state
//    */
//   QVERIFY(!editA.isEnabled());
//   /*
//    * Set row state that allows edition
//    */
//   rs.setRowCount(2);
//   rs.setCurrentRow(0);
//   mwl.setRowState(rs);
//   QVERIFY(editA.isEnabled());
//   /*
//    * Set row state that not allows edition
//    */
//   rs.setRowCount(0);
//   rs.setCurrentRow(-1);
//   mwl.setRowState(rs);
//   QVERIFY(!editA.isEnabled());
//   /*
//    * Add a widget
//    */
//   mwl.addWidget(&editB, 1);
//   QVERIFY(!editA.isEnabled());
//   QVERIFY(!editB.isEnabled());
//   /*
//    * Set row state that allows edition
//    */
//   rs.setRowCount(2);
//   rs.setCurrentRow(0);
//   mwl.setRowState(rs);
//   QVERIFY(editA.isEnabled());
//   QVERIFY(editB.isEnabled());
// }

// void ControllerTest::mappedWidgetListSetModelTest()
// {
//   using Mdt::ItemEditor::MappedWidgetList;
//   using Mdt::ItemEditor::RowState;
// 
//   RowState rs;
//   MappedWidgetList mwl;
//   QLineEdit editA, editB;
// 
//   /*
//    * Initial state
//    */
//   QCOMPARE(mwl.size(), 0);
//   QVERIFY(mwl.isEmpty());
//   /*
//    * map widgets
//    */
//   mwl.addWidget(&editA, 0);
//   mwl.addWidget(&editB, 1);
//   QCOMPARE(mwl.size(), 2);
//   QVERIFY(!mwl.isEmpty());
//   QVERIFY(!editA.isEnabled());
//   QVERIFY(!editB.isEnabled());
//   /*
//    * Set a empty model
//    */
//   TestTableModel model1;
//   mwl.setModel(&model1);
//   QVERIFY(!editA.isEnabled());
//   QVERIFY(!editB.isEnabled());
//   /*
//    * Populate model with 1 column
//    */
//   model1.populate(3, 1);
//   rs.setRowCount(3);
//   rs.setCurrentRow(0);
//   mwl.setRowState(rs);
//   QVERIFY(editA.isEnabled());
//   QVERIFY(!editB.isEnabled());
//   /*
//    * Populate model with 2 columns
//    */
//   model1.populate(3, 2);
//   rs.setRowCount(3);
//   rs.setCurrentRow(0);
//   mwl.setRowState(rs);
//   QVERIFY(editA.isEnabled());
//   QVERIFY(editB.isEnabled());
//   /*
//    * Set a other model that is allready populated
//    */
//   TestTableModel model2;
//   model2.populate(5, 1);
//   mwl.setModel(&model2);
//   QVERIFY(editA.isEnabled());
//   QVERIFY(!editB.isEnabled());
//   /*
//    * Clear
//    */
//   mwl.clear();
//   QCOMPARE(mwl.size(), 0);
//   QVERIFY(mwl.isEmpty());
// }

void ControllerTest::widgetMapperControllerSetModelTest()
{
  using Mdt::ItemEditor::WidgetMapperController;
  using Mdt::ItemEditor::RowState;

  TestTableModel tableModel;
  QStringListModel listModel;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
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
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
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
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  QVERIFY(editB.text().isEmpty());
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
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  index = tableModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), tableModel.data(index).toString());
  /*
   * Change to a empty model model
   */
  QCOMPARE(listModel.rowCount(), 0);
  controller.clearMapping();
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
  // Map widgets
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  // Check mapped widgets enable state
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
  /*
   * Populate previous model and set it to controller
   */
  tableModel.populate(3, 2);
  controller.clearMapping();
  controller.setModel(&tableModel);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Map widgets
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = tableModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), tableModel.data(index).toString());
  index = tableModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), tableModel.data(index).toString());
  /*
   * Clear model
   */
  tableModel.clear();
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check mapped widgets enable state
  QVERIFY(!editA.isEnabled());
  QVERIFY(!editB.isEnabled());
  // Check mapped widgets data
  QVERIFY(editA.text().isEmpty());
  QVERIFY(editB.text().isEmpty());
}

void ControllerTest::widgetMapperControllerCurrentRowChangedTest()
{
  using Mdt::ItemEditor::WidgetMapperController;
  using Mdt::ItemEditor::RowState;

  TestTableModel model;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
  QSignalSpy rowStateSpy(&controller, &WidgetMapperController::rowStateChanged);
  QList<QVariant> spyItem;
  RowState rs;

  QVERIFY(rowStateSpy.isValid());
  /*
   * Setup
   */
  model.populate(5, 2);
  controller.setModel(&model);
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  rowStateSpy.clear();
  /*
   * Initial state
   */
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  QCOMPARE(rowStateSpy.count(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Set current row
   */
  QVERIFY(controller.setCurrentRow(2));
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 2);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 2);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to first
   */
  controller.toFirst();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 0);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 0);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to next
   */
  controller.toNext();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to last
   */
  controller.toLast();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 4);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 4);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(4, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(4, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
  /*
   * Go to previous
   */
  controller.toPrevious();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 3);
  // Check that row state signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 3);
  // Check mapped widgets enable state
  QVERIFY(editA.isEnabled());
  QVERIFY(editB.isEnabled());
  // Check mapped widgets data
  index = model.index(3, 0);
  QVERIFY(index.isValid());
  QCOMPARE(editA.text(), model.data(index).toString());
  index = model.index(3, 1);
  QVERIFY(index.isValid());
  QCOMPARE(editB.text(), model.data(index).toString());
}

void ControllerTest::widgetMapperControllerEditTest()
{
  using Mdt::ItemEditor::WidgetMapperController;
  using Mdt::ItemEditor::ControllerState;

  
  
  QFETCH(QWidget*, editor0);

  TestTableModel model;
  QLineEdit editA, editB;
  WidgetMapperController controller;
  QModelIndex index;
  QSignalSpy stateSpy(&controller, &WidgetMapperController::controllerStateChanged);
  QList<QVariant> spyItem;
  QString originalText;

  QVERIFY(stateSpy.isValid());
  /*
   * Setup
   */
  model.populate(5, 2);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, QVariant());
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  model.setData(index, QVariant());
  controller.setModel(&model);
  controller.addMapping(&editA, 0);
  controller.addMapping(&editB, 1);
  /*
   * Initial state
   */
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
  QVERIFY(editA.text() != QString("A"));
  QTest::keyClicks(&editA, "A");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Editing);
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 1);
  /*
   * End edition by submit
   */
  QVERIFY(controller.submit());
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 1);
  // Check that model was updated
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("A"));
  // Check that widget data was updated
  QCOMPARE(editA.text(), QString("A"));
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Begin editing
   */
  originalText = editA.text();
  QVERIFY(originalText != QString("B"));
  QTest::keyClicks(&editA, "B");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Editing);
  // Check that we cannot change current row
  QVERIFY(!controller.setCurrentRow(1));
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End edition by revert
   */
  controller.revert();
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  // Check that new state was signaled
  QCOMPARE(stateSpy.count(), 1);
  spyItem = stateSpy.takeFirst();
  QVERIFY(spyItem.at(0).value<ControllerState>() == ControllerState::Visualizing);
  QCOMPARE(controller.currentRow(), 0);
  // Check that model was not updated
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index).toString(), originalText);
  // Check that widget data was updated
  QCOMPARE(editA.text(), originalText);
  // Check that we can change current row
  QVERIFY(controller.setCurrentRow(0));
  QCOMPARE(controller.currentRow(), 0);
}

void ControllerTest::widgetMapperControllerEditTest_data()
{
//   Q_DECLARE_METATYPE(QWidget*)

  QTest::addColumn<QWidget*>("editor0");

  QTest::newRow("QLineEdit") << (static_cast<QWidget*>(new QLineEdit));
  QTest::newRow("QSpinBox") << (static_cast<QWidget*>(new QSpinBox));

  auto *cb = new QComboBox;
  ///cb->setEditable(true);
  cb->addItem("A");
  cb->addItem("B");
  QTest::newRow("QComboBox") << (static_cast<QWidget*>(cb));
}

/*
 * Helper functions
 */

void ControllerTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void ControllerTest::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
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
