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
#include "TableViewControllerTest.h"
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

namespace ItemEditor = Mdt::ItemEditor;
namespace ItemModel = Mdt::ItemModel;
using ItemEditor::TableViewController;
using ItemEditor::ControllerState;
using ItemModel::VariantTableModel;
using ItemModel::FilterColumn;
using ItemModel::ParentModelColumn;

void TableViewControllerTest::initTestCase()
{
}

void TableViewControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void TableViewControllerTest::basicStateTest()
{
  /*
   * Initial state
   */
  TableViewController controller;
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  QVERIFY(controller.primaryKey().isNull());
}

void TableViewControllerTest::setModelTest()
{
  using Mdt::ItemEditor::RowState;

  VariantTableModel tableModel;
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
  QVERIFY(controller.sourceModel() == nullptr);
  QVERIFY(controller.view() == nullptr);
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QVERIFY(controller.sourceModel() == &tableModel);
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
  // Check that row state was signaled
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

void TableViewControllerTest::primaryKeyTest()
{
  QFAIL("Not complete");
}

void TableViewControllerTest::currentRowChangeTest()
{
  using Mdt::ItemEditor::RowState;

  VariantTableModel modelA, modelB;
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

void TableViewControllerTest::viewCurrentRowChangeBenchmark()
{
  VariantTableModel model;
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

void TableViewControllerTest::controllerCurrentRowChangeBenchmark()
{
  VariantTableModel model;
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

void TableViewControllerTest::editTest()
{
  VariantTableModel model;
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

void TableViewControllerTest::insertTest()
{
  QStringListModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Insert at end
   */
  controller.setInsertLocation(TableViewController::InsertAtEnd);
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtEnd);
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 2);

  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::insertFromModelTest()
{
  QStringListModel model;
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == TableViewController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Begin editing
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  beginEditing(view, index, BeginEditTrigger::DoubleClick);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  editText(view, index, "ABCD");
  /*
   * Insert at end of model
   */
  QVERIFY(model.insertRow(2));
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End
   */
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("ABCD"));
}

void TableViewControllerTest::removeTest()
{
  QStringListModel model({"A","B"});
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit and check that remove fails
   */
   // Begin editing
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  beginEditing(view, index, BeginEditTrigger::DoubleClick);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  editText(view, index, "ABCD");
  // Try to remove
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing and check
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("ABCD"));
  /*
   * Remove
   */
  QVERIFY(controller.remove());
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove
   */
  QVERIFY(controller.remove());
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Remove
   */
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::removeFromModelTest()
{
  QStringListModel model({"A","B","C","D"});
  QModelIndex index;
  QTableView view;
  TableViewController controller;

  /*
   * Setup
   */
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit row 0 , remove last row
   */
   // Begin editing
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  beginEditing(view, index, BeginEditTrigger::DoubleClick);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  editText(view, index, "1234");
  // Remove last row
  QVERIFY(model.removeRow(3));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing and check
  endEditing(view, index, EndEditTrigger::EnterKeyClick);
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("1234"));
  /*
   * Go to row 1 , remove row 0
   */
  // Go to row 1
  QVERIFY(controller.setCurrentRow(1));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 1);
  // Remove row 0 , and check that controller goes to row 0
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove current row (row 0)
   */
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Remove last row
   */
  QVERIFY(model.removeRow(0));
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);

  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void TableViewControllerTest::filterTest()
{
  VariantTableModel model;
  QTableView view;
  TableViewController controller;
  /*
   * Setup
   */
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Check enable/disable filter
   */
  // Enable filter
  QVERIFY(!controller.isFilterEnabled());
  controller.setFilterEnabled(true);
  QVERIFY(controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Disable filter
  controller.setFilterEnabled(false);
  QVERIFY(!controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Enable filter
  controller.setFilterEnabled(true);
  QVERIFY(controller.isFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Apply a filter
   */
  controller.setFilter( FilterColumn(0) > 1 );
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Disable filter
   */
  controller.setFilterEnabled(false);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Apply a filter
   *  -> Must also enable filter
   */
  controller.setFilter( FilterColumn(0) == 2 );
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  /**
   * \todo
   * Try while editing
   */

  /*
   * Play
   */
//   view.show();
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }

  QFAIL("Not complete");
}

void TableViewControllerTest::sortTest()
{
  QFAIL("Not complete");
}

/*
 * Helper
 */

void TableViewControllerTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void TableViewControllerTest::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
}

void TableViewControllerTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void TableViewControllerTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  TableViewControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
