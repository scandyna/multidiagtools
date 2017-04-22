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
#include "ItemSelectionModelTest.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QItemSelection>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

using Mdt::ItemModel::VariantTableModel;
using Mdt::ItemEditor::ItemSelectionModel;

/*
 * Init and cleanup
 */

void ItemSelectionModelTest::initTestCase()
{
}

void ItemSelectionModelTest::cleanupTestCase()
{
}


/*
 * Tests
 */

void ItemSelectionModelTest::sandbox()
{
  QTableView view;
  VariantTableModel model;
  model.populate(5, 4);
  view.setModel(&model);

  //view.setSelectionBehavior(QTableView::SelectItems);
  //view.setSelectionMode(QTableView::MultiSelection);

  ItemSelectionModel selectionModel(&model);
  view.setSelectionModel(&selectionModel);

  displayWidget(view);
}

void ItemSelectionModelTest::setCurrentIndexTest()
{
  VariantTableModel model;
  QModelIndex index;
  QList<QVariant> spyItem;

  /*
   * Initial state
   */
  ItemSelectionModel selectionModel(&model);
  QSignalSpy currentRowSpy(&selectionModel, &ItemSelectionModel::currentRowChangeRequested);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Populate model
   */
  model.populate(3, 2);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Update current row
   */
  selectionModel.updateCurrentRow(0);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index column
   */
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index row
   */
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Selection must not be updated now
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  // Update current row
  selectionModel.updateCurrentRow(1);
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index row and column
   */
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Selection must not be updated now
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 2);
  // Update current row
  selectionModel.updateCurrentRow(2);
  QCOMPARE(selectionModel.currentIndex().row(), 2);
  QCOMPARE(selectionModel.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index column
   */
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  QCOMPARE(selectionModel.currentIndex().row(), 2);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Update current row
   */
  selectionModel.updateCurrentRow(1);
  QCOMPARE(selectionModel.currentIndex().row(), 1);
  QCOMPARE(selectionModel.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Update current row to a invalid one
   */
  selectionModel.updateCurrentRow(-1);
  QVERIFY(!selectionModel.currentIndex().isValid());
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to go to row 0
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Selection must not be updated now
  QVERIFY(!selectionModel.currentIndex().isValid());
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 0);
  // Update current row
  selectionModel.updateCurrentRow(0);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Reset the model
   * -> The controller also set current row to -1
   */
  model.resize(2, 3);
  selectionModel.updateCurrentRow(-1);
  QVERIFY(!selectionModel.currentIndex().isValid());
  /*
   * Simulate user wants to go to row 0
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Selection must not be updated now
  QVERIFY(!selectionModel.currentIndex().isValid());
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 0);
  // Update current row
  selectionModel.updateCurrentRow(0);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate a filter proxy model
   * which removes and add rows
   *
   * Bug discovered 20170415
   */
  model.removeLastRow();
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 0);
  model.appendRow();
  model.appendRow();
  QVERIFY(!selectionModel.currentIndex().isValid());
  /*
   * Simulate user wants to go to row 0
   *
   * Bug discovered 20170415
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  // Selection must not be updated now
  QVERIFY(!selectionModel.currentIndex().isValid());
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 0);
  // Update current row
  selectionModel.updateCurrentRow(0);
  QCOMPARE(selectionModel.currentIndex().row(), 0);
  QCOMPARE(selectionModel.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
}

void ItemSelectionModelTest::tableViewSetCurrentIndexTest()
{
  QTableView view;
  VariantTableModel model;
  QModelIndex index;
  QList<QVariant> spyItem;

  /*
   * Initial state
   */
  ItemSelectionModel selectionModel(&model);
  QSignalSpy currentRowSpy(&selectionModel, &ItemSelectionModel::currentRowChangeRequested);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Setup model and view
   */
  model.populate(3, 2);
  view.setModel(&model);
  view.setSelectionModel(&selectionModel);
  view.show();
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Update current row
   */
  selectionModel.updateCurrentRow(0);
  QCOMPARE(view.currentIndex().row(), 0);
  QCOMPARE(view.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index column
   */
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  QCOMPARE(view.currentIndex().row(), 0);
  QCOMPARE(view.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index row
   */
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  // Selection must not be updated now
  QCOMPARE(view.currentIndex().row(), 0);
  QCOMPARE(view.currentIndex().column(), 1);
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  // Update current row
  selectionModel.updateCurrentRow(1);
  QCOMPARE(view.currentIndex().row(), 1);
  QCOMPARE(view.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index row and column
   */
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  // Selection must not be updated now
  QCOMPARE(view.currentIndex().row(), 1);
  QCOMPARE(view.currentIndex().column(), 1);
  // Check that row change was requested
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 2);
  // Update current row
  selectionModel.updateCurrentRow(2);
  QCOMPARE(view.currentIndex().row(), 2);
  QCOMPARE(view.currentIndex().column(), 0);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Simulate user wants to change current index column
   */
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  view.setCurrentIndex(index);
  QCOMPARE(view.currentIndex().row(), 2);
  QCOMPARE(view.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Update current row
   */
  selectionModel.updateCurrentRow(1);
  QCOMPARE(view.currentIndex().row(), 1);
  QCOMPARE(view.currentIndex().column(), 1);
  QCOMPARE(currentRowSpy.count(), 0);

  /*
   * Play
   */
//   displayWidget(view);
}

// void ItemSelectionModelTest::tableViewSetCurrentIndexTestOLD()
// {
//   using Mdt::ItemEditor::ItemSelectionModel;
//   using Mdt::ItemEditor::ControllerState;
// 
//   QTableView view;
//   VariantTableModel model;
//   QModelIndex index;
// 
//   /*
//    * Initial state
//    */
//   ItemSelectionModel selectionModel(&model);
//   QVERIFY(selectionModel.isCurrentRowChangeAllowed());
//   /*
//    * Setup model and view
//    */
//   model.populate(3, 2);
//   view.setModel(&model);
//   view.setSelectionModel(&selectionModel);
//   view.show();
//   /*
//    * Check using setCurrentIndex()
//    */
//   index = model.index(1, 0);
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 1);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Inhibit row change
//   selectionModel.setControllerState(ControllerState::Editing);
//   QVERIFY(!selectionModel.isCurrentRowChangeAllowed());
//   index = model.index(0, 0);
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 1);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Changing column must still work
//   index = model.index(1, 1);
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 1);
//   QCOMPARE(view.currentIndex().column(), 1);
//   /*
//    * Check using select(QModelIndex) before setCurrentIndex()
//    */
//   // Allow row change and go back to row 0
//   selectionModel.setControllerState(ControllerState::Visualizing);
//   index = model.index(0, 0);
//   selectionModel.select(index, QItemSelectionModel::ClearAndSelect);
//   QVERIFY(selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Inhibit row change
//   selectionModel.setControllerState(ControllerState::Editing);
//   index = model.index(1, 0);
//   selectionModel.select(index, QItemSelectionModel::ClearAndSelect);
//   QVERIFY(!selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Changing column must still work
//   index = model.index(0, 1);
//   selectionModel.select(index, QItemSelectionModel::ClearAndSelect);
//   QVERIFY(selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 1);
//   /*
//    * Check using select(QItemSelection) before setCurrentIndex()
//    */
//   // Allow row change and go back to row 0
//   selectionModel.setControllerState(ControllerState::Visualizing);
//   index = model.index(0, 0);
//   selectionModel.select(QItemSelection(index, index), QItemSelectionModel::ClearAndSelect);
//   QVERIFY(selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Inhibit row change
//   selectionModel.setControllerState(ControllerState::Editing);
//   index = model.index(1, 0);
//   selectionModel.select(QItemSelection(index, index), QItemSelectionModel::ClearAndSelect);
//   QVERIFY(!selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 0);
//   // Changing column must still work
//   index = model.index(0, 1);
//   selectionModel.select(QItemSelection(index, index), QItemSelectionModel::ClearAndSelect);
//   QVERIFY(selectionModel.isSelected(index));
//   view.setCurrentIndex(index);
//   QCOMPARE(view.currentIndex().row(), 0);
//   QCOMPARE(view.currentIndex().column(), 1);
// 
//   /*
//    * Play
//    */
// //   view.resize(300, 200);
// //   while(view.isVisible()){
// //     QTest::qWait(500);
// //   }
// }

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ItemSelectionModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
