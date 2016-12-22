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
#include "WidgetMapperControllerTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
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

namespace ItemEditor = Mdt::ItemEditor;
using Mdt::ItemModel::VariantTableModel;
using Mdt::ItemEditor::WidgetMapperController;

void WidgetMapperControllerTest::initTestCase()
{
}

void WidgetMapperControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void WidgetMapperControllerTest::setModelTest()
{
  using Mdt::ItemEditor::RowState;

  VariantTableModel tableModel;
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

void WidgetMapperControllerTest::currentRowChangedTest()
{
  using Mdt::ItemEditor::RowState;

  VariantTableModel model;
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

void WidgetMapperControllerTest::editTest()
{
  using Mdt::ItemEditor::ControllerState;

  
  
  QFETCH(QWidget*, editor0);

  VariantTableModel model;
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
  /*
   * Cleanup
   */
  delete editor0;
}

void WidgetMapperControllerTest::editTest_data()
{
  QTest::addColumn<QWidget*>("editor0");

  QTest::newRow("QLineEdit") << (static_cast<QWidget*>(new QLineEdit));
  QTest::newRow("QSpinBox") << (static_cast<QWidget*>(new QSpinBox));

  auto *cb = new QComboBox;
  ///cb->setEditable(true);
  cb->addItem("A");
  cb->addItem("B");
  QTest::newRow("QComboBox") << (static_cast<QWidget*>(cb));
}

void WidgetMapperControllerTest::insertTest()
{
  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  QVERIFY(!editor0.isEnabled());
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 1);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(editor0.isEnabled());
  /*
   * Insert at beginning
   */
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(editor0.isEnabled());
  /*
   * Insert at end
   */
  controller.setInsertLocation(WidgetMapperController::InsertAtEnd);
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtEnd);
  QVERIFY(controller.insert());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 2);
  QVERIFY(editor0.isEnabled());
}

void WidgetMapperControllerTest::insertFromModelTest()
{
  using Mdt::ItemEditor::ControllerState;

  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Initial state
   */
  QVERIFY(controller.insertLocation() == WidgetMapperController::InsertAtBeginning);
  /*
   * Setup
   */
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
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
  editor0.setText("ABCD");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  /*
   * Insert at end in model
   */
  QVERIFY(model.insertRow(2));
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * End
   */
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("ABCD"));
}

void WidgetMapperControllerTest::removeTest()
{
  using ItemEditor::ControllerState;

  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Setup
   */
  model.setStringList({"A","B"});
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit and check that remove fails
   */
  // Begin editing
  editor0.setText("1234");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Try to remove
  QVERIFY(!controller.remove());
  QCOMPARE(controller.rowCount(), 2);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("1234"));
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
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
//   editor0.show();
//   while(editor0.isVisible()){
//     QTest::qWait(500);
//   }
}

void WidgetMapperControllerTest::removeFromModelTest()
{
  using ItemEditor::ControllerState;

  QStringListModel model;
  QLineEdit editor0;
  WidgetMapperController controller;
  QModelIndex index;

  /*
   * Setup
   */
  model.setStringList({"A","B","C","D"});
  controller.setModel(&model);
  controller.addMapping(&editor0, 0);
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Edit row 0 , remove last row
   */
  // Begin editing
  editor0.setText("1234");
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // Remove last row
  QVERIFY(model.removeRow(3));
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(controller.controllerState() == ControllerState::Editing);
  // End editing and check
  // End editing
  QVERIFY(controller.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole).toString(), QString("1234"));
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
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
//   editor0.show();
//   while(editor0.isVisible()){
//     QTest::qWait(500);
//   }
}

void WidgetMapperControllerTest::filterTest()
{
  QFAIL("Not complete");
}

void WidgetMapperControllerTest::sortTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WidgetMapperControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
